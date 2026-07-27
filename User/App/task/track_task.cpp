/**
 * @file track_task.cpp
 * @brief 巡线传感器读取任务 — 每 10ms 读取 PCA9555
 *
 * @note 读取结果存入 g_track_data，供 OLED 等其他任务使用。
 */

#include "task.hpp"
#include "test.hpp"      /* g_track_data */
#include "device_cfg.hpp"

#include "FreeRTOS.h"
#include "task.h"

/* ===================================================================
 * 巡线传感器读取任务
 * =================================================================== */

void track_task(void *arg)
{
  (void)arg;

  while (1)
  {
    g_track_data = iic_track.read();
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}
