/**
 * @file angle_task.cpp
 * @brief 角度闭环控制任务 + 测试任务
 *
 * angle_task:        10ms 周期, 调用 ins.update() 执行角度闭环
 * angle_test_task:   测试任务, 每 5s 切换一次目标角度 (0°→90°→180°→270°→0°)
 */

#include "task.hpp"
#include "device_cfg.hpp"
#include "bsp_cfg.hpp"

#include "FreeRTOS.h" // IWYU pragma: keep
#include "task.h"

// ==========================================================================
// angle_task — 角度闭环控制 (10ms, 优先级 6)
// ==========================================================================

/**
 * @brief 角度闭环控制 — 每 10ms 调用 ins.update() 执行角度 PID 差速控制
 * @param arg  未使用
 */
void angle_task(void *arg)
{
  (void)arg;

  ins.enable();

  while (1)
  {
    ins.update();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// ==========================================================================
// angle_test_task — 目标角度轮转测试 (5s 周期)
// ==========================================================================

/**
 * @brief 角度闭环测试 — 每 5s 轮转目标角度 (0°→90°→180°→270°→0°), 串口打印状态
 * @param arg  未使用
 */
void angle_test_task(void *arg)
{
  (void)arg;

  /* 等待陀螺仪启动稳定 */
  vTaskDelay(pdMS_TO_TICKS(2000));

  static const float targets[] = {0.0f, 90.0f, 180.0f, 270.0f};
  static const int   num       = sizeof(targets) / sizeof(targets[0]);
  int                idx       = 0;

  ins.set_base_speed(0.0f); /* 原地转向测试 */
  ins.set_pid(50.0f, 0.0f, 0.0f);
  ins.set_target(targets[0]);
  ins.enable();

  while (1)
  {
    /* ── 每 5 秒切换目标角度 ── */
    ins.set_target(targets[idx]);
    idx = (idx + 1) % num;

    /* ── 串口打印当前状态 ── */
    const TiGyro::ReportData &r = ti_gyro.get_last_report();
    if (r.valid)
    {
      bsp_uart4.uart_printf(
        "[ANGLE] tgt=%.1f cur=%.1f err=%.1f corr=%.2f\r\n",
        (double)ins.target(),
        (double)ins.current_yaw(),
        (double)(ins.target() - ins.current_yaw()),
        (double)ins.correction());
    }

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}
