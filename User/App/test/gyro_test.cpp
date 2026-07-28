/**
 * @file gyro_test.cpp
 * @brief 陀螺仪测试 — 姿态数据读取与串口打印
 *
 * @note 依赖 TiGyro 设备驱动。陀螺仪通过 UART1 (PB7/PB6) 连接。
 */

#include "test.hpp"
#include "device_cfg.hpp"

#include "FreeRTOS.h" // IWYU pragma: keep
#include "task.h"

/* ===================================================================
 * 测试任务
 * =================================================================== */

void gyro_test_task(void *arg)
{
  (void)arg;

  ti_gyro.set_mode(TiGyro::REPORT_MODE_ATTITUDE);
  vTaskDelay(10);
  ti_gyro.start_report(true);
  vTaskDelay(10);

  while (1)
  {
    ti_gyro.update(portMAX_DELAY);
    // const TiGyro::ReportData &report = ti_gyro.get_last_report();
    // if (report.valid)
    // {
    //   uint32_t t_ms = pdTICKS_TO_MS(xTaskGetTickCount());
    //   bsp_uart4.uart_printf(
    //     "[%lu ms] GYRO y=%.2f p=%.2f r=%.2f\r\n",
    //     t_ms,
    //     (double)report.yaw_deg,
    //     (double)report.pitch_deg,
    //     (double)report.roll_deg);
    // }
  }
}
