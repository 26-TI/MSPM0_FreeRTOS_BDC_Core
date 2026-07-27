/**
 * @file gyro_task.cpp
 * @brief 陀螺仪接收任务 — 持续解析姿态数据帧
 *
 * @note 阻塞等待 UART3 陀螺仪数据，解析后更新 ti_gyro 内部缓存，
 *       供 OLED 等其他任务读取最新姿态。
 */

#include "task.hpp"
#include "device_cfg.hpp"

#include "FreeRTOS.h"
#include "task.h"

/* ===================================================================
 * 陀螺仪接收任务
 * =================================================================== */

void gyro_task(void *arg)
{
  (void)arg;

  ti_gyro.set_mode(TiGyro::REPORT_MODE_ATTITUDE);
  ti_gyro.start_report(true);

  while (1)
  {
    ti_gyro.update(portMAX_DELAY);
  }
}
