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
 * 循线参数 — 集中在此调参
 * =================================================================== */

// 3.0 2.0
// 2.0 1.3

#define LINE_BASE_SPEED 2.0f     ///< 基础前进速度 (rps), 越大越快
#define LINE_KP 1.3f             ///< 循线比例增益, 越大纠偏越强 (典型 1.0~3.0)
#define LINE_OFF_THRESHOLD 10U   ///< 脱线判定帧数 (10 × 10ms = 100ms)
#define LINE_SENSOR_MASK 0x0FFFU ///< 12 位传感器有效位掩码

/* 传感器位置权值 (左负右正, 中间=0, 范围 ±9), 只读常量 */
static const int16_t LINE_WEIGHT[12] = {-9, -7, -5, -3, -1, 0, 0, 1, 3, 5, 7, 9};

/* ===================================================================
 * 脱线检测信号量（供 line_off_task 永久阻塞接收）
 * =================================================================== */

SemaphoreHandle_t g_line_off_sem = NULL; ///< 脱线二值信号量

/* ===================================================================
 * 辅助函数：从 12 位传感器位掩码计算循线位置误差 (纯整数+最终一次浮点)
 * =================================================================== */

/**
 * @param raw  12 位传感器原始值 (bit=1→白/无线, bit=0→黑/有线)
 * @return 质心位置 (-9.0 ~ +9.0), 0 表示居中或丢线
 */
static float line_position_error(uint16_t raw)
{
  int16_t weighted_sum = 0;
  int16_t total_count  = 0;

  for (int i = 0; i < 12; i++)
  {
    if (!(raw & (1U << (11U - i)))) /* bit=0 → 检测到黑线 */
    {
      weighted_sum += LINE_WEIGHT[i];
      total_count++;
    }
  }

  if (total_count == 0)
    return 0.0f;

  return (float)weighted_sum / (float)total_count;
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
    uint16_t raw        = g_track_data & LINE_SENSOR_MASK;
    float    error      = line_position_error(raw);
    float    correction = error * LINE_KP;

    /* ── 脱线检测: 连续 LINE_OFF_THRESHOLD 帧全白/全黑 → 发信号量 ── */
    if ((raw & LINE_SENSOR_MASK) == LINE_SENSOR_MASK
        || (raw & LINE_SENSOR_MASK) == 0x0000U)
    {
      off_line_cnt++;
      if (off_line_cnt >= LINE_OFF_THRESHOLD)
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
