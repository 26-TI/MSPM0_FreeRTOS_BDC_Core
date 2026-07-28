/**
 * @file pwm_test.cpp
 * @brief PWM 测试 — 占空比控制与状态打印
 */

#include "test.hpp"
#include "bsp_cfg.hpp"

#include "task.h"

/* ===================================================================
 * 测试任务
 * =================================================================== */

void pwm_test_task(void *arg)
{
  (void)arg;

  /* 应用层初始化占空比 */
  // bsp_pwm_a0.set_duty(0, 2500); /* CH0: 10%  1*/
  bsp_pwm_a0.set_duty(1, 2500); /* CH1: 50%  2*/
  bsp_pwm_a0.set_duty(2, 200);  /* CH2: 50%  2*/
  // bsp_pwm_a0.set_duty(3, 200); /* CH3: 50%  1*/

  while (1)
  {
    uint32_t freq     = bsp_pwm_a0.get_freq();
    uint32_t period   = bsp_pwm_a0.get_period();
    uint32_t duty_ch0 = bsp_pwm_a0.get_duty(0);
    uint32_t duty_ch1 = bsp_pwm_a0.get_duty(1);
    uint32_t duty_ch2 = bsp_pwm_a0.get_duty(2);
    uint32_t duty_ch3 = bsp_pwm_a0.get_duty(3);

    bsp_uart4.uart_printf(
      "[PWM] %uHz period=%u CH0=%u CH1=%u CH2=%u CH3=%u\r\n",
      (unsigned)freq,
      (unsigned)period,
      (unsigned)duty_ch0,
      (unsigned)duty_ch1,
      (unsigned)duty_ch2,
      (unsigned)duty_ch3);

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
