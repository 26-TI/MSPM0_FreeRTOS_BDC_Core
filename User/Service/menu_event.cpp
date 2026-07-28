/**
 * @file menu_event.cpp
 * @brief 菜单事件服务 — 基于 FreeRTOS 二值信号量实现
 */

#include "menu_event.hpp"

SemaphoreHandle_t g_menu_sems[MENU_EVENT_COUNT] = {};
volatile int32_t  g_menu_value                  = 0;

// ==========================================================================
// menu_event_init() — 创建所有事件信号量 (由 init_task 调用)
// ==========================================================================

/**
 * @brief 为全部 16 个菜单事件各创建一个二值信号量
 */
void menu_event_init(void)
{
  for (int i = 0; i < MENU_EVENT_COUNT; i++)
  {
    g_menu_sems[i] = xSemaphoreCreateBinary();
  }
}

// ==========================================================================
// event_bit_to_idx() — 位掩码 → 数组索引
// ==========================================================================

/**
 * @brief 将事件位掩码转换为信号量数组索引
 * @param event_bit  事件位 (如 EVT_MOTOR_START = 0x0001)
 * @return 数组索引 (如 0x0001 → 0)
 *
 * @note 使用 __builtin_ctz (Count Trailing Zeros):
 *       ctz(0x0001)=0, ctz(0x0002)=1, ctz(0x0004)=2 ...
 */
static inline int event_bit_to_idx(uint16_t event_bit)
{
  return __builtin_ctz(event_bit);
}

// ==========================================================================
// menu_event_give() — 给出事件信号量
// ==========================================================================

/**
 * @brief 给出菜单事件信号量 (供 menu_action() 调用)
 * @param event_bit  事件位掩码 (EVT_xxx)
 */
void menu_event_give(uint16_t event_bit)
{
  int idx = event_bit_to_idx(event_bit);
  if (idx < MENU_EVENT_COUNT && g_menu_sems[idx])
  {
    xSemaphoreGive(g_menu_sems[idx]);
  }
}

// ==========================================================================
// menu_event_give_with_value() — 给出事件信号量 + 附带值
// ==========================================================================

/**
 * @brief 给出菜单事件信号量并附带数值 (如设定速度值)
 * @param event_bit  事件位掩码
 * @param value      附带数值
 */
void menu_event_give_with_value(uint16_t event_bit, int32_t value)
{
  g_menu_value = value;
  menu_event_give(event_bit);
}
