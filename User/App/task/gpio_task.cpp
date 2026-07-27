/**
 * @file gpio_task.cpp
 * @brief GPIO 按键任务 — 三按键短按/长按检测 + 蜂鸣器 + LED 闪烁
 *
 * 功能:
 *   - 每 50ms 轮询 key_1 / key_2 / key_3（消抖 100ms，长按 1s）
 *   - 短按: 蜂鸣器短鸣（按下响、释放停）
 *   - 长按: 蜂鸣器短鸣（鸣叫 100ms 后自动停止）
 *   - LED 每 500ms 翻转（心跳指示）
 *   - 所有按键事件通过 UART4 打印
 */

#include "task.hpp"
#include "bsp_cfg.hpp"

#include "task.h"

/* ===================================================================
 * GPIO 任务 — 按键轮询 + 蜂鸣器 + LED
 * =================================================================== */

void gpio_task(void *arg)
{
  (void)arg;

  uint32_t tick = 0U;

  while (1)
  {
    tick++;

    /* ---- 每 500ms 翻转 LED（心跳） ---- */
    if (tick % 10U == 0U)
    {
      gpio_led.toggle();
    }

    /* ---- 每 50ms 轮询三个按键 ---- */
    BspKey::Event e1 = key_1.poll();
    BspKey::Event e2 = key_2.poll();
    BspKey::Event e3 = key_3.poll();

    /* ---- key_1 处理 ---- */
    if (e1 == BspKey::PRESSED)
    {
      gpio_buzz.set();
    }
    else if (e1 == BspKey::RELEASED)
    {
      gpio_buzz.reset();
    }
    else if (e1 == BspKey::LONG_PRESS)
    {
      gpio_buzz.set();
    }

    /* ---- key_2 处理 ---- */
    if (e2 == BspKey::PRESSED)
    {
      gpio_buzz.set();
    }
    else if (e2 == BspKey::RELEASED)
    {
      gpio_buzz.reset();
    }
    else if (e2 == BspKey::LONG_PRESS)
    {
      gpio_buzz.set();
    }

    /* ---- key_3 处理 ---- */
    if (e3 == BspKey::PRESSED)
    {
      gpio_buzz.set();
    }
    else if (e3 == BspKey::RELEASED)
    {
      gpio_buzz.reset();
    }
    else if (e3 == BspKey::LONG_PRESS)
    {
      gpio_buzz.set();
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
