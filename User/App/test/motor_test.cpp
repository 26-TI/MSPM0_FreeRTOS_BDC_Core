/**
 * @file motor_test.cpp
 * @brief 双电机开环匀速测试 — 固定占空比持续旋转，打印 RPM
 *
 * @note set_duty 固定，每 500ms 打印左右轮 RPM
 */

#include "test.hpp"
#include "bsp_cfg.hpp"
#include "device_cfg.hpp"

#include "task.h"

static constexpr uint32_t CH_A     = 2500;
static constexpr uint32_t CH_B     = 1300;  ///< <2500=正转
static constexpr TickType_t PRINT_MS = 500;

void motor_test_task(void *arg)
{
  (void)arg;

  bsp_uart4.uart_printf("\r\nMOTOR\r\n");

  motor_left.enable();
  motor_right.enable();

  motor_left.set_duty(CH_A, CH_B);
  motor_right.set_duty(CH_A, CH_B);

  TickType_t last_pr = xTaskGetTickCount();

  while (1)
  {
    motor_left.update();
    motor_right.update();

    if ((xTaskGetTickCount() - last_pr) >= pdMS_TO_TICKS(PRINT_MS))
    {
      const Motor::State &L = motor_left.get_state();
      const Motor::State &R = motor_right.get_state();
      bsp_uart4.uart_printf("L%.4f R%.4f\r\n",
       (double)L.current_speed_rpm, (double)R.current_speed_rpm);
      last_pr = xTaskGetTickCount();
    }

    vTaskDelay(pdMS_TO_TICKS(5));
  }
}
