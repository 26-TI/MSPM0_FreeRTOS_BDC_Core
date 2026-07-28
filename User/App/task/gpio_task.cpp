/**
 * @file gpio_task.cpp
 * @brief GPIO 按键任务 — 三按键短按/长按检测 + 蜂鸣器 + LED 闪烁
 *
 * 功能:
 *   - 每 50ms 轮询 key_1 / key_2 / key_3（消抖 100ms，长按 1s）
 *   - 短按: 蜂鸣器响 100ms 后自动停
 *   - 长按: 蜂鸣器响 300ms 后自动停
 *   - LED 每 500ms 翻转（心跳指示）
 */

#include "task.hpp"
#include "bsp_cfg.hpp"

#include "task.h"

// ==========================================================================
// gpio_task — 按键轮询 + 蜂鸣器 + LED (50ms, 优先级 4)
// ==========================================================================

/**
 * @brief 三按键轮询处理, 短按蜂鸣 100ms, 长按蜂鸣 300ms, LED 心跳闪烁
 * @param arg  未使用
 */
void gpio_task(void *arg)
{
  (void)arg;

  uint32_t tick     = 0U;
  uint8_t  buzz_cnt = 0U; /* 蜂鸣器剩余 ticks (每 tick=50ms) */

  while (1)
  {
    tick++;

    /* ---- LED 每 500ms 翻转（心跳） ---- */
    if (tick % 10U == 0U)
    {
      gpio_led.toggle();
    }

    /* ---- 蜂鸣器计时关闭 ---- */
    if (buzz_cnt > 0U)
    {
      buzz_cnt--;
      if (buzz_cnt == 0U)
      {
        gpio_buzz.reset();
      }
    }

    /* ---- 每 50ms 轮询三个按键 ---- */
    BspKey::Event e1 = key_1.poll();
    BspKey::Event e2 = key_2.poll();
    BspKey::Event e3 = key_3.poll();

    /* ---- key_1 ---- */
    if (e1 == BspKey::PRESSED)
    {
      gpio_buzz.set();
      buzz_cnt = 2U; /* 2 × 50ms = 100ms */
    }
    else if (e1 == BspKey::LONG_PRESS)
    {
      gpio_buzz.set();
      buzz_cnt = 6U; /* 6 × 50ms = 300ms */
    }

    /* ---- key_2 ---- */
    if (e2 == BspKey::PRESSED)
    {
      gpio_buzz.set();
      buzz_cnt = 2U;
    }
    else if (e2 == BspKey::LONG_PRESS)
    {
      gpio_buzz.set();
      buzz_cnt = 6U;
    }

    /* ---- key_3 ---- */
    if (e3 == BspKey::PRESSED)
    {
      gpio_buzz.set();
      buzz_cnt = 2U;
    }
    else if (e3 == BspKey::LONG_PRESS)
    {
      gpio_buzz.set();
      buzz_cnt = 6U;
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
