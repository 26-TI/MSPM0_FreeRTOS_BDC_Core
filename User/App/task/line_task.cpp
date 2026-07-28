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
 */

#include "task.hpp"
#include "test.hpp" /* g_track_data */
#include "device_cfg.hpp"

#include "FreeRTOS.h"
#include "task.h"

/* ===================================================================
 * 循线参数
 * =================================================================== */

#define LINE_BASE_SPEED 2.0f   ///< 基础前进速度 (rps)

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
      total_count  += 1.0f;
    }
  }

  /* ── 4. 丢线时返回 0（不纠偏，直行） ── */
  if (total_count == 0.0f)
    return 0.0f;

  return weighted_sum / total_count;
}

/* ===================================================================
 * 循线控制任务 (5ms, 优先级 6)
 * =================================================================== */

void line_task(void *arg)
{
  (void)arg;

  while (1)
  {
    uint16_t raw        = g_track_data & 0x0FFFU;
    float    error      = line_position_error(raw);
    float    correction = error * 1.2f;

    /* ── 差速驱动 ── */
    float left_speed  = LINE_BASE_SPEED - correction;
    float right_speed = LINE_BASE_SPEED + correction;

    motor_left.set_speed(left_speed);
    motor_right.set_speed(-right_speed); /* 右轮硬件反装, 取负 */

    vTaskDelay(pdMS_TO_TICKS(5));
  }
}
