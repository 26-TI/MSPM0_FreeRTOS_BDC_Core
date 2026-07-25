/**
 * @file track_test.cpp
 * @brief 巡线传感器测试 — I2C PCA9555 读取
 *
 * @note 通过 I2C1 (PA29/PA30) 读取 PCA9555 IO 扩展芯片的巡线数据。
 *       数据写入 g_track_data 供 OLED 任务显示。
 */

#include "test.hpp"
#include "device_cfg.hpp"
#include "task.h"

/* ===================================================================
 * 共享数据
 * =================================================================== */

/** @brief 巡线数据全局变量（track_test_task 写入，oled_test_task 读取） */
volatile uint16_t g_track_data = 0U;

/* ===================================================================
 * 测试任务
 * =================================================================== */

void track_test_task(void *arg)
{
  (void)arg;

  while (1)
  {
    g_track_data = iic_track.read();
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}
