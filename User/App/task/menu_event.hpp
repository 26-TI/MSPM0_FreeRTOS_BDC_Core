/**
 * @file menu_event.hpp
 * @brief 菜单事件组 — 事件位定义 + 分发 API (空菜单版)
 *
 * @note 菜单通过 g_menu_events 下发命令，各应用任务读取并清零。
 *       当前为空菜单，事件位仅预留框架，不连接实际功能。
 */

#ifndef __MENU_EVENT_HPP__
#define __MENU_EVENT_HPP__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /* ===================================================================
   * 事件位定义 (16-bit) — 当前全为预留位
   * =================================================================== */

#define EVT_RESERVED_0   (1U << 0)
#define EVT_RESERVED_1   (1U << 1)
#define EVT_RESERVED_2   (1U << 2)
#define EVT_RESERVED_3   (1U << 3)
#define EVT_RESERVED_4   (1U << 4)
#define EVT_RESERVED_5   (1U << 5)
#define EVT_RESERVED_6   (1U << 6)
#define EVT_RESERVED_7   (1U << 7)
#define EVT_RESERVED_8   (1U << 8)
#define EVT_RESERVED_9   (1U << 9)
#define EVT_RESERVED_10  (1U << 10)
#define EVT_RESERVED_11  (1U << 11)
#define EVT_RESERVED_12  (1U << 12)
#define EVT_RESERVED_13  (1U << 13)
#define EVT_RESERVED_14  (1U << 14)
#define EVT_RESERVED_15  (1U << 15)

  /* ===================================================================
   * 全局变量
   * =================================================================== */

  extern volatile uint16_t g_menu_events;
  extern volatile int32_t  g_menu_value;

  /* ===================================================================
   * API
   * =================================================================== */

  void menu_event_set(uint16_t event);
  void menu_event_set_with_value(uint16_t event, int32_t value);
  bool menu_event_test_and_clear(uint16_t event);
  uint16_t menu_event_peek();

#ifdef __cplusplus
}
#endif

#endif /* __MENU_EVENT_HPP__ */
