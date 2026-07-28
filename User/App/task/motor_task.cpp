/**
 * @file motor_task.cpp
 * @brief 电机控制更新任务 (5ms) + 电机数据打印任务 (50ms)
 */

#include "task.hpp"
#include "device_cfg.hpp"
#include "bsp_cfg.hpp"

#include "FreeRTOS.h"
#include "task.h"

/* ===================================================================
 * 电机控制更新任务 (5ms, 优先级 5) — 仅 update, 不设速度不打印
 * =================================================================== */

void motor_task(void *arg)
{
  (void)arg;

  motor_left.speed_control_enable();
  motor_right.speed_control_enable();

  while (1)
  {
    motor_left.update();
    motor_right.update();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

/* ===================================================================
 * 电机数据打印任务 (50ms, 优先级 5)
 * =================================================================== */

void motor_print_task(void *arg)
{
  (void)arg;

  while (1)
  {
    /* 左轮: 目标速度, 实际速度, PID 输出 */
    // bsp_uart4.uart_printf(
    //   "%.2f,%.2f,%.2f,",
    //   (double)motor_left.get_target(),
    //   (double)motor_left.get_speed(),
    //   (double)motor_left.get_pid_out());

    // /* 右轮 */
    // bsp_uart4.uart_printf(
    //   "%.2f,%.2f,%.2f\n",
    //   (double)motor_right.get_target(),
    //   (double)motor_right.get_speed(),
    //   (double)motor_right.get_pid_out());

    bsp_uart4.uart_printf(
      "%.2f,%.2f,%.2f\r\n",
      (double)ins.target(),
      (double)ins.current_yaw(),
      (double)ins.correction());

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
