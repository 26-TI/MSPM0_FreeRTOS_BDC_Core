/**
 * @file oled_test.cpp
 * @brief OLED 显示测试 — 巡线数据可视化
 *
 * @note 通过 I2C0 (PA0/PA1) 驱动 OLED 128x64。
 *       显示巡线传感器的原始数据和 bit 位图。
 */

#include "test.hpp"
#include "device_cfg.hpp"
#include "task.h"

#include <stdio.h>

/* ===================================================================
 * 外部共享数据
 * =================================================================== */

/** @brief 巡线数据全局变量（由 track_test_task 写入） */
extern volatile uint16_t g_track_data;

/* ===================================================================
 * 测试任务
 * =================================================================== */

void oled_test_task(void *arg)
{
  (void)arg;

  char buf[17];
  iic_oled.clear();

  while (1)
  {
    uint16_t data = g_track_data;

    /* 第 0 行: 原始 HEX 值 */
    snprintf(buf, sizeof(buf), "T:%04X", data);
    iic_oled.show_string(0, 0, buf);

    /* 第 2 行: 低 12 位 bit 图 */
    for (uint8_t i = 0; i < 12U; i++)
    {
      buf[i] = (data & (1U << i)) ? '1' : '0';
    }
    buf[12] = '\0';
    iic_oled.show_string(0, 2, buf);

    iic_oled.refresh();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
