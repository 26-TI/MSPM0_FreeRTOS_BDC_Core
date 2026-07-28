/**
 * @file menu_tree.cpp
 * @brief 3级菜单树静态定义 — 连接实际功能的事件位
 *
 * 结构:
 * @code
 * [ 0] Main Menu (根)
 * ├── [ 1] Motor    → [ 4]~[ 8]
 * ├── [ 2] Debug    → [ 9]~[15]
 * └── [ 3] Control  → [16]~[18]
 * @endcode
 *
 * 操作说明:
 *   - MENU_ACTION:  按 K3 触发 event_bit 事件
 *   - MENU_TOGGLE:  按 K3 触发 event_bit 事件（约定 ON/OFF 成对位）
 *   - MENU_VALUE:   按 K3 进入编辑 → 再按 K3 确认触发事件 + g_menu_value
 *   - MENU_SUBMENU: 进入下级菜单
 *   - MENU_BACK:    返回上级
 *   - MENU_INFO:    只读展示
 */

#include "menu.hpp"
#include "menu_event.hpp"

const MenuItem g_menu_tree[MENU_MAX_ITEMS] = {

  /* ══════════════════════════════════════════════════════════════════
   * [0] 主菜单 (根)
   * ══════════════════════════════════════════════════════════════════ */
  {"Main Menu", MENU_SUBMENU, 0, 0, 0, 0, 0, -1, 1, 3},

  /* ══════════════════════════════════════════════════════════════════
   * [1] Motor — 电机控制 (parent=0)
   * ══════════════════════════════════════════════════════════════════ */
  {"Motor", MENU_SUBMENU, 0, 0, 0, 0, 0, 0, 4, 5},

  /* ══════════════════════════════════════════════════════════════════
   * [2] Debug — 调试打印 (parent=0)
   * ══════════════════════════════════════════════════════════════════ */
  {"Debug", MENU_SUBMENU, 0, 0, 0, 0, 0, 0, 9, 7},

  /* ══════════════════════════════════════════════════════════════════
   * [3] Control — 控制模式 (parent=0)
   * ══════════════════════════════════════════════════════════════════ */
  {"Control", MENU_SUBMENU, 0, 0, 0, 0, 0, 0, 16, 3},

  /* ──────────────────────────────────────────────────────────────────
   * Motor 子项 (parent=1)
   * ────────────────────────────────────────────────────────────────── */

  /* [4] 启动电机 — MENU_ACTION, K3 触发 EVT_MOTOR_START */
  {"Start Motor", MENU_ACTION, EVT_MOTOR_START, 0, 0, 0, 0, 1, -1, 0},

  /* [5] 停止电机 — MENU_ACTION, K3 触发 EVT_MOTOR_STOP */
  {"Stop Motor", MENU_ACTION, EVT_MOTOR_STOP, 0, 0, 0, 0, 1, -1, 0},

  /* [6] 设定速度 — MENU_VALUE, K3 编辑 → K3 确认 → EVT_MOTOR_SET_SPEED + g_menu_value */
  {"Set Speed", MENU_VALUE, EVT_MOTOR_SET_SPEED, 0, 100, 10, 30, 1, -1, 0},

  /* [7] 电机打印开 — MENU_ACTION, K3 触发 EVT_MOTOR_PRINT_ON */
  {"Mtr Print ON", MENU_ACTION, EVT_MOTOR_PRINT_ON, 0, 0, 0, 0, 1, -1, 0},

  /* [8] 电机打印关 — MENU_ACTION, K3 触发 EVT_MOTOR_PRINT_OFF */
  {"Mtr Print OFF", MENU_ACTION, EVT_MOTOR_PRINT_OFF, 0, 0, 0, 0, 1, -1, 0},

  /* ──────────────────────────────────────────────────────────────────
   * Debug 子项 (parent=2)
   * ────────────────────────────────────────────────────────────────── */

  /* [9]  陀螺仪打印开 */
  {"Gyro Prt ON", MENU_ACTION, EVT_GYRO_PRINT_ON, 0, 0, 0, 0, 2, -1, 0},

  /* [10] 陀螺仪打印关 */
  {"Gyro Prt OFF", MENU_ACTION, EVT_GYRO_PRINT_OFF, 0, 0, 0, 0, 2, -1, 0},

  /* [11] 巡线打印开 */
  {"Track Prt ON", MENU_ACTION, EVT_TRACK_PRINT_ON, 0, 0, 0, 0, 2, -1, 0},

  /* [12] 巡线打印关 */
  {"Track Prt OFF", MENU_ACTION, EVT_TRACK_PRINT_OFF, 0, 0, 0, 0, 2, -1, 0},

  /* [13] CPU 打印开 */
  {"CPU Prt ON", MENU_ACTION, EVT_CPU_PRINT_ON, 0, 0, 0, 0, 2, -1, 0},

  /* [14] CPU 打印关 */
  {"CPU Prt OFF", MENU_ACTION, EVT_CPU_PRINT_OFF, 0, 0, 0, 0, 2, -1, 0},

  /* [15] 一键全开/全关 — MENU_ACTION 成对 */
  {"Print ALL ON", MENU_ACTION, EVT_PRINT_ALL_ON, 0, 0, 0, 0, 2, -1, 0},

  /* ──────────────────────────────────────────────────────────────────
   * Control 子项 (parent=3)
   * ────────────────────────────────────────────────────────────────── */

  /* [16] 循线控制开 */
  {"Line Ctrl ON", MENU_ACTION, EVT_LINE_CTRL_ON, 0, 0, 0, 0, 3, -1, 0},

  /* [17] 循线控制关 */
  {"Line Ctrl OFF", MENU_ACTION, EVT_LINE_CTRL_OFF, 0, 0, 0, 0, 3, -1, 0},

  /* [18] Back */
  {"Back", MENU_BACK, 0, 0, 0, 0, 0, 3, -1, 0},

};

const uint8_t g_menu_tree_count = sizeof(g_menu_tree) / sizeof(g_menu_tree[0]);
