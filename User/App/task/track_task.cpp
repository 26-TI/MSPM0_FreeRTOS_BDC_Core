/**
 * @file track_task.cpp
 * @brief 巡线传感器读取任务 — 每 10ms 读取 PCA9555
 *
 * @note 读取结果存入 g_track_data，供 OLED / 循线控制等任务使用。
 */

#include "task.hpp"
#include "test.hpp" /* g_track_data */
#include "device_cfg.hpp"

#include "FreeRTOS.h" // IWYU pragma: keep
#include "task.h"

// ==========================================================================
// track_task — 巡线传感器读取 (10ms, 优先级 7)
// ==========================================================================

/**
 * @brief 每 10ms 读取 PCA9555 巡线模块，结果写入 g_track_data
 * @param arg  未使用
 */
void track_task(void *arg)
{
  (void)arg;

  while (1)
  {
    g_track_data = iic_track.read();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
