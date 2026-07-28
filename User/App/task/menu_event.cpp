/**
 * @file menu_event.cpp
 * @brief 菜单事件组服务 — 实现
 */

#include "menu_event.hpp"

volatile uint16_t g_menu_events = 0U;
volatile int32_t  g_menu_value  = 0;

void menu_event_set(uint16_t event)
{
  g_menu_events |= event;
}

void menu_event_set_with_value(uint16_t event, int32_t value)
{
  g_menu_value  = value;
  g_menu_events |= event;
}

bool menu_event_test_and_clear(uint16_t event)
{
  if (g_menu_events & event)
  {
    g_menu_events &= ~event;
    return true;
  }
  return false;
}

uint16_t menu_event_peek()
{
  return g_menu_events;
}
