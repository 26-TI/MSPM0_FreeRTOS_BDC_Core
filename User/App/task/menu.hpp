/**
 * @file menu.hpp
 * @brief OLED 菜单模块 — 3级菜单 + 滚动 + 事件分发 (3键适配版)
 *
 * 按键映射 (仅 key_1 / key_2 / key_3):
 *   key_1 短按: UP       key_1 长按: (无)
 *   key_2 短按: DOWN     key_2 长按: (无)
 *   key_3 短按: ENTER    key_3 长按: HOME (回 IDLE)
 *
 * 导航:
 *   - 选中 "Back" 项 → 返回上级
 *   - key_3 长按 → 任意位置回 IDLE 首页
 */

#ifndef __MENU_HPP__
#define __MENU_HPP__

#include <stdint.h>

/* ===================================================================
 * 常量
 * =================================================================== */

#define MENU_CHARS_PER_LINE    16U
#define MENU_VISIBLE_ROWS      4U
#define MENU_MAX_VISIBLE_ITEMS 3U
#define MENU_NAME_MAX          15U
#define MENU_MAX_ITEMS         32U
#define MENU_MAX_LEVEL         3U
#define MENU_IDLE_TIMEOUT      100U

/* ===================================================================
 * 菜单项类型
 * =================================================================== */

enum MenuItemType
{
  MENU_SUBMENU = 0,
  MENU_ACTION  = 1,
  MENU_VALUE   = 2,
  MENU_TOGGLE  = 3,
  MENU_INFO    = 4,
  MENU_BACK    = 5,
};

/* ===================================================================
 * 菜单项结构
 * =================================================================== */

struct MenuItem
{
  char          name[MENU_NAME_MAX];
  MenuItemType  type;
  uint16_t      event_bit;
  int32_t       val_min;
  int32_t       val_max;
  int32_t       val_step;
  int32_t       val_default;
  int8_t        parent_idx;
  int8_t        first_child_idx;
  uint8_t       child_count;
};

/* ===================================================================
 * 菜单类
 * =================================================================== */

class Menu
{
public:
  void init();
  void tick();

private:
  void read_keys();
  void state_machine();
  void menu_enter();
  void menu_back();
  void menu_home();
  void menu_action();
  void value_confirm();
  void value_cancel();

  void render();
  void render_idle();
  void render_nav();
  void render_edit();

  /* 当前菜单上下文 */
  const MenuItem *_current_menu = nullptr;
  uint8_t        _item_count    = 0;
  int8_t         _first_idx     = 0;

  /* 导航状态 */
  enum State { ST_IDLE, ST_NAV, ST_EDIT };
  State        _state     = ST_IDLE;
  uint8_t      _level     = 0;
  uint8_t      _cursor[3] = {};
  uint8_t      _start_idx = 0;
  const MenuItem *_parent[MENU_MAX_LEVEL] = {};
  uint8_t      _stack_top = 0;

  int32_t      _edit_val   = 0;
  uint16_t     _idle_timer = 0;
  bool         _dirty      = true;

  bool _key_short[3] = {};
  bool _key_long[3]  = {};
};

/* ===================================================================
 * 全局声明
 * =================================================================== */

extern const MenuItem g_menu_tree[MENU_MAX_ITEMS];
extern const uint8_t  g_menu_tree_count;
extern Menu           g_menu;

/* ===================================================================
 * 任务入口
 * =================================================================== */

#ifdef __cplusplus
extern "C"
{
#endif

  void menu_task(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __MENU_HPP__ */
