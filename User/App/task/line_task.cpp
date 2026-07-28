/**
 * @file line_task.cpp
 * @brief 循线控制任务 — 12 路传感器加权质心 + 比例差速控制
 *
 * 传感器布局 (12 路, 左→右):
 *   bit11=第0路(最左) ... bit0=第11路(最右)
 *   1 = 白色(无线)       0 = 黑色(有线)
 *
 * 算法:
 *   1. 取反 → 黑=1, 加权求质心
 *   2. 误差 × 比例增益 → 差速偏移量
 *   3. 左轮 = 基础速度 - 修正量
 *      右轮 = 基础速度 + 修正量  (右轮硬件反装)
 *
 * 脱线检测:
 *   连续 100ms 检测不到黑线 → 给出信号量通知脱线处理任务
 */

#include "task.hpp"
#include "test.hpp" /* g_track_data */
#include "device_cfg.hpp"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* ===================================================================
 * 循线参数
 * =================================================================== */

// base:2.0f  error:1.3
// base:3.0f  error:2.0

#define LINE_BASE_SPEED 2.0f ///< 基础前进速度 (rps)

/* ===================================================================
 * 脱线检测信号量（供 line_off_task 永久阻塞接收）
 * =================================================================== */

SemaphoreHandle_t g_line_off_sem = NULL; ///< 脱线二值信号量

/* ===================================================================
 * 辅助函数：从 12 位传感器数据计算循线位置误差
 * =================================================================== */

static float line_position_error(uint16_t raw)
{
  /* ── 1. 提取 12 位传感器到 bool 数组 ── */
  bool sensor[12]; /* true=黑色(有线), false=白色(无线) */

  for (int i = 0; i < 12; i++)
    sensor[i] = !(raw & (1U << (11U - i)));

  /* ── 2. 建立位置权值数组 (手动赋值) ── */
  float weight[12] = {-9.0f, -7.0f, -5.0f, -3.0f, -1.0f, 0.0f, 0.0f, 1.0f, 3.0f, 5.0f, 7.0f, 9.0f};

  /* ── 3. 加权求和并取平均 (质心) ── */
  float weighted_sum = 0.0f;
  float total_count  = 0.0f;

  for (int i = 0; i < 12; i++)
  {
    if (sensor[i])
    {
      weighted_sum += weight[i];
      total_count += 1.0f;
    }
  }

  /* ── 4. 丢线时返回 0（不纠偏，直行） ── */
  if (total_count == 0.0f)
    return 0.0f;

  return weighted_sum / total_count;
}

/* ===================================================================
 * 循线控制任务 (10ms, 优先级 6)
 * =================================================================== */

void line_task(void *arg)
{
  (void)arg;

  static uint32_t off_line_cnt = 0U; /* 连续脱线帧计数 (每帧 10ms) */

  while (1)
  {
    uint16_t raw        = g_track_data & 0x0FFFU;
    float    error      = line_position_error(raw);
    float    correction = error * 1.3f;

    /* ── 脱线检测: 连续 10 帧 (100ms) 全白 → 发信号量 ── */
    if ((raw & 0x0FFFU) == 0x0FFFU || (raw & 0x0FFFU) == 0x0000U) /* 12 位全白 = 完全丢线 */
    {
      off_line_cnt++;
      if (off_line_cnt >= 10U) /* 100ms */
      {
        xSemaphoreGive(g_line_off_sem); /* 给出脱线信号量 */
        off_line_cnt = 0U;              /* 复位计数，避免反复触发 */
      }
    }
    else
    {
      off_line_cnt = 0U; /* 有线 → 清零脱线计数 */
      /* ── 差速驱动 ── */
      float left_speed  = LINE_BASE_SPEED - correction;
      float right_speed = LINE_BASE_SPEED + correction;
  
      motor_left.set_speed(left_speed);
      motor_right.set_speed(-right_speed); /* 右轮硬件反装, 取负 */
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
