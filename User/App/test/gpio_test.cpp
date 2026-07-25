/**
 * @file gpio_test.cpp
 * @brief GPIO / 按键测试 — LED 闪烁 + 蜂鸣器 + 按键消抖打印
 *
 * @note 每 50ms 轮询所有按键（消抖: 2×50=100ms，长按: 20×50=1s）。
 *       LED 每 500ms 翻转，蜂鸣器短鸣示意按键按下。
 */

#include "test.hpp"
#include "bsp_cfg.hpp"

#include "task.h"

/* ===================================================================
 * GPIO 测试 — 输出控制 + 按键轮询
 * =================================================================== */

void gpio_test_task(void *arg)
{
  (void)arg;

  bsp_uart4.uart_printf("[GPIO] LED blink + Key poll\r\n");

  uint32_t tick = 0U;

  while (1)
  {
    tick++;

    /* ---- 每 500ms 翻转 LED ---- */
    if (tick % 10U == 0U)
    {
      gpio_led.toggle();
    }

    /* ---- 每 5ms 轮询所有按键 ---- */
    BspKey::Event e1 = key_1.poll();
    BspKey::Event e2 = key_2.poll();
    BspKey::Event e3 = key_3.poll();

    if (e1 == BspKey::PRESSED)
    {
      gpio_buzz.set();
      bsp_uart4.uart_printf("[KEY] 1 pressed\r\n");
    }
    else if (e1 == BspKey::RELEASED)
    {
      gpio_buzz.reset();
      bsp_uart4.uart_printf("[KEY] 1 released\r\n");
    }
    else if (e1 == BspKey::LONG_PRESS)
    {
      bsp_uart4.uart_printf("[KEY] 1 LONG_PRESS\r\n");
    }

    if (e2 != BspKey::NONE)
    {
      bsp_uart4.uart_printf("[KEY] 2 event=%d\r\n", (int)e2);
    }
    if (e3 != BspKey::NONE)
    {
      bsp_uart4.uart_printf("[KEY] 3 event=%d\r\n", (int)e3);
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
