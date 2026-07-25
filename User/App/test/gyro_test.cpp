/**
 * @file gyro_test.cpp
 * @brief 陀螺仪测试 — 姿态数据读取与串口打印
 *
 * @note 依赖 TiGyro 设备驱动。陀螺仪通过 UART1 (PB7/PB6) 连接。
 */

#include "test.hpp"
#include "bsp_cfg.hpp"
#include "device_cfg.hpp"

#include "FreeRTOS.h"
#include "task.h"

/* ===================================================================
 * 测试任务
 * =================================================================== */

void gyro_test_task(void *arg)
{
  (void)arg;

  ti_gyro.set_mode(TiGyro::REPORT_MODE_ATTITUDE);
  ti_gyro.start_report(true);

  while (1)
  {
    ti_gyro.update(portMAX_DELAY);

    const TiGyro::ReportData &report = ti_gyro.get_last_report();
    if (report.valid)
    {
      bsp_uart4.uart_printf(
        "[GYRO] yaw=%.6f pitch=%.6f roll=%.6f\r\n",
        report.yaw_deg,
        report.pitch_deg,
        report.roll_deg);
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
