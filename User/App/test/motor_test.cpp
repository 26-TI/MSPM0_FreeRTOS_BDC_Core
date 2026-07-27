/**
 * @file motor_test.cpp
 * @brief 电机波形测试 — 正弦 / 方波 / 恒速
 *
 * 通过宏选择波形:
 *   TEST_SINE_WAVE   1 → 正弦波  -幅值 .. +幅值  rps
 *   TEST_SQUARE_WAVE 1 → 方波    -幅值 / +幅值  rps
 *   两者均为 0        → 恒定 ±1 rps
 */

#include "test.hpp"
#include "device_cfg.hpp"

#include "FreeRTOS.h"
#include "task.h"
#include <math.h>

/* ===================================================================
 * 波形选择 (二者互斥)
 * =================================================================== */
#define TEST_SINE_WAVE   0
#define TEST_SQUARE_WAVE 1

#define TEST_WAVE_PERIOD_S  3.0f   ///< 波形周期 (秒)
#define TEST_WAVE_AMP       3.0f   ///< 波形幅值 (rps)

/* ===================================================================
 * 电机测试任务 (2ms) — 波形驱动速度控制
 * =================================================================== */

void motor_test_task(void *arg)
{
  (void)arg;

  motor_left.speed_control_enable();
  motor_right.speed_control_enable();

#if !TEST_SINE_WAVE && !TEST_SQUARE_WAVE
  /* 恒定速度模式 */
  motor_left.set_speed(1.0f);
  motor_right.set_speed(-1.0f);
#endif

#if TEST_SINE_WAVE || TEST_SQUARE_WAVE
  uint32_t wave_tick = 0;
#endif

  while (1)
  {
    motor_left.update();
    motor_right.update();

    /* ── 波形更新 ── */
#if TEST_SINE_WAVE
    {
      float phase  = (float)wave_tick * 2.0f * 3.14159265f * 0.002f / TEST_WAVE_PERIOD_S;
      float target = TEST_WAVE_AMP * sinf(phase);
      motor_left.set_speed(target);
      motor_right.set_speed(-target);
    }
#elif TEST_SQUARE_WAVE
    {
      uint32_t half_ticks = (uint32_t)(TEST_WAVE_PERIOD_S * 250.0f); /* T_s × 1000 / 2 / 2ms */
      float    target     = ((wave_tick / half_ticks) % 2 == 0)
                              ? TEST_WAVE_AMP
                              : -TEST_WAVE_AMP;
      motor_left.set_speed(target);
      motor_right.set_speed(-target);
    }
#endif

#if TEST_SINE_WAVE || TEST_SQUARE_WAVE
    wave_tick++;
#endif

    vTaskDelay(pdMS_TO_TICKS(2));
  }
}
