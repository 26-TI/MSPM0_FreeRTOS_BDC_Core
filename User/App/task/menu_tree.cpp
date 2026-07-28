/**
 * @file menu_tree.cpp
 * @brief 3级菜单树静态定义 — 空菜单框架 (不适配实际功能)
 *
 * 结构:
 * @code
 * [0] Main Menu (根)
 * ├── [1]  Motor    → [4]~[6]
 * ├── [2]  Sensor   → [7]~[9]
 * └── [3]  System   → [10]~[12]
 * @endcode
 *
 * @note 所有叶子节点均为 MENU_INFO (只读), 不触发任何事件。
 *       后续接入实际功能时, 将对应项改为 MENU_ACTION/MENU_VALUE 并绑定 event_bit。
 */

#include "menu.hpp"
#include "menu_event.hpp"

const MenuItem g_menu_tree[MENU_MAX_ITEMS] = {

  /* ──── [0] 主菜单 (根) ──── */
  { "Main Menu",  MENU_SUBMENU, 0, 0,0,0,0,  -1, 1, 3 },

  /* ──── [1] Motor ──── */
  { "Motor",      MENU_SUBMENU, 0, 0,0,0,0,   0, 4, 3 },

  /* ──── [2] Sensor ──── */
  { "Sensor",     MENU_SUBMENU, 0, 0,0,0,0,   0, 7, 3 },

  /* ──── [3] System ──── */
  { "System",     MENU_SUBMENU, 0, 0,0,0,0,   0,10, 3 },

  /* ──── Motor 子项 (parent=1) ──── */
  { "Speed Ctrl",  MENU_INFO, 0, 0,0,0,0,  1,-1,0 },
  { "Angle Ctrl",  MENU_INFO, 0, 0,0,0,0,  1,-1,0 },
  { "Back",        MENU_BACK, 0, 0,0,0,0,  1,-1,0 },

  /* ──── Sensor 子项 (parent=2) ──── */
  { "Track Data",  MENU_INFO, 0, 0,0,0,0,  2,-1,0 },
  { "Gyro Data",   MENU_INFO, 0, 0,0,0,0,  2,-1,0 },
  { "Back",        MENU_BACK, 0, 0,0,0,0,  2,-1,0 },

  /* ──── System 子项 (parent=3) ──── */
  { "CPU Usage",   MENU_INFO, 0, 0,0,0,0,  3,-1,0 },
  { "Uptime",      MENU_INFO, 0, 0,0,0,0,  3,-1,0 },
  { "Back",        MENU_BACK, 0, 0,0,0,0,  3,-1,0 },
};

const uint8_t g_menu_tree_count = sizeof(g_menu_tree) / sizeof(g_menu_tree[0]);
