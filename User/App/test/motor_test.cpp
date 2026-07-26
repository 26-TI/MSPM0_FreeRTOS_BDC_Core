/**
 * @file motor_test.cpp
 * @brief 双电机开环匀速测试 — 固定占空比持续旋转，打印 RPM
 *
 * @note set_duty 固定，每 500ms 打印左右轮 RPM
 */

#include "test.hpp"
#include "device_cfg.hpp"

#include "task.h"


void motor_test_task(void *arg)
{
  (void)arg;

  motor_left.set_speed(2);
  motor_right.set_speed(-2);

  motor_left.speed_control_enable();
  motor_right.speed_control_enable();

  while (1)
  {
    motor_left.update();
    motor_right.update();

    vTaskDelay(pdMS_TO_TICKS(2));
  }
}
