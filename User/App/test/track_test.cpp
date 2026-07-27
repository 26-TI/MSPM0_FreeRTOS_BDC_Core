/**
 * @file track_test.cpp
 * @brief 巡线传感器读取任务 — PCA9555 每 10ms 读取一次
 *
 * @note 读取结果存入全局变量 g_track_data，供 OLED 任务显示。
 */

#include "test.hpp"
#include "device_cfg.hpp"
#include "bsp_cfg.hpp"

#include "FreeRTOS.h"
#include "task.h"

/* ==================== 全局共享变量 ==================== */
volatile uint16_t g_track_data = 0;

/* ===================================================================
 * 巡线传感器读取任务
 * =================================================================== */

void track_test_task(void *arg)
{
  (void)arg;

  while (1)
  {
    g_track_data = iic_track.read();
    vTaskDelay(4);
  }
}
