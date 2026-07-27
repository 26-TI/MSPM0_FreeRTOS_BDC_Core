/**
 * @file line_task.cpp
 * @brief 循线控制任务 — 12 路传感器加权质心 + PID 差速控制
 *
 * 传感器布局 (12 路, 左→右):
 *   bit11=第0路(最左) ... bit0=第11路(最右)
 *   1 = 白色(无线)       0 = 黑色(有线)
 *
 * 算法:
 *   1. 取反 → 黑=1, 加权求质心 (0~11)
 *   2. 误差 = 质心 - 5.5  (中心 5.5, 范围 ±5.5)
 *   3. PID 修正 → 差速偏移量
 *   4. 左轮 = 基础速度 - 修正量
 *      右轮 = 基础速度 + 修正量  (右轮硬件反装)
 */

#include "task.hpp"
#include "test.hpp" /* g_track_data */
#include "device_cfg.hpp"
#include "pid.hpp"

#include "FreeRTOS.h"
#include "task.h"

/* ===================================================================
 * 循线参数
 * =================================================================== */

#define LINE_PERIOD_MS 2 ///< 控制周期 (ms)

#define LINE_BASE_SPEED 0.5f     ///< 基础前进速度 (rps)
#define LINE_MAX_CORRECTION 1.0f ///< 最大差速修正 (rps)
#define LINE_LOST_TIMEOUT 50     ///< 丢线超时 (tick 数)，超时后停车

/* PID 增益 */
#define LINE_KP 0.08f
#define LINE_KI 0.02f
#define LINE_KD 0.01f
#define LINE_PID_T 0.01f ///< = LINE_PERIOD_MS / 1000
#define LINE_PID_OUT_MAX 1.0f
#define LINE_PID_I_MAX 0.5f

/* ===================================================================
 * 辅助函数：从 12 位传感器数据计算循线位置误差
 *
 * @param  raw  12 位传感器位图 (bit11=最左 ... bit0=最右)
 * @return      位置误差 (-5.5 .. +5.5), 0=居中,
 *              负=线偏左, 正=线偏右
 * @retval 99.0f 哨兵值：完全丢线
 * =================================================================== */

static float line_position_error(uint16_t raw)
{
  float weighted_sum = 0.0f; /* 加权位置累加 */
  float total_weight = 0.0f; /* 检测到黑线的传感器数 */

  for (int i = 0; i < 12; i++)
  {
    /* 传感器[i] 对应 bit(11-i);  0=黑色(有线)  1=白色(无线) */
    bool is_black = !(raw & (1U << (11U - i)));

    if (is_black)
    {
      weighted_sum += (float)i; /* 传感器编号即位置 */
      total_weight += 1.0f;
    }
  }

  if (total_weight == 0.0f)
    return 99.0f; /* 完全丢线 */

  float centroid = weighted_sum / total_weight; /* 0 .. 11 */
  return centroid - 5.5f;                       /* -5.5 .. +5.5 */
}

/* ===================================================================
 * 循线控制任务 (2ms, 优先级 6)
 * =================================================================== */

void line_task(void *arg)
{
  (void)arg;

  /* ── 初始化循线 PID ── */
  static PID line_pid;
  line_pid.init(LINE_KP, LINE_KI, LINE_KD, LINE_PID_T, LINE_PID_OUT_MAX, LINE_PID_I_MAX, 1); /* 积分分离开启 */

  uint32_t lost_ticks = 0;

  /* 等待首帧传感器数据就绪 */
  vTaskDelay(pdMS_TO_TICKS(20));

  while (1)
  {
    uint16_t raw        = g_track_data & 0x0FFFU;
    float    error      = line_position_error(raw);
    float    correction = 0.0f;

    if (error < 90.0f)
    {
      /* 有线可见 → PID 控制 */
      lost_ticks = 0;
      correction = line_pid.calc(0.0f, error);

      /* 限幅 */
      if (correction > LINE_MAX_CORRECTION) correction = LINE_MAX_CORRECTION;
      if (correction < -LINE_MAX_CORRECTION) correction = -LINE_MAX_CORRECTION;
    }
    else
    {
      /* 丢线 → 滑行停车 */
      lost_ticks++;
      correction = 0.0f;
      line_pid.clear();

      if (lost_ticks > LINE_LOST_TIMEOUT)
      {
        motor_left.set_speed(0.0f);
        motor_right.set_speed(0.0f);
        vTaskDelay(pdMS_TO_TICKS(LINE_PERIOD_MS));
        continue;
      }
    }

    /* ── 差速驱动 ── */
    float left_speed  = LINE_BASE_SPEED - correction;
    float right_speed = LINE_BASE_SPEED + correction;

    motor_left.set_speed(left_speed);
    motor_right.set_speed(-right_speed); /* 右轮硬件反装, 取负 */

    vTaskDelay(pdMS_TO_TICKS(LINE_PERIOD_MS));
  }
}
