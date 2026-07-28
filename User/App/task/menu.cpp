/**
 * @file menu.cpp
 * @brief OLED 菜单模块 — 3级菜单 + 滚动 + 渲染实现 (3键适配版)
 *
 * 按键映射:
 *   key_1 短: UP     key_1 长: (无)
 *   key_2 短: DOWN   key_2 长: (无)
 *   key_3 短: ENTER  key_3 长: HOME
 */

#include "menu.hpp"
#include "menu_event.hpp"
#include "device_cfg.hpp"
#include "bsp_cfg.hpp"
#include "user_system.hpp"

#include "FreeRTOS.h"
#include "task.h"

/* ===================================================================
 * 全局实例
 * =================================================================== */

Menu g_menu;

/* ===================================================================
 * Menu::init()
 * =================================================================== */

void Menu::init()
{
  _state     = ST_IDLE;
  _level     = 0;
  _cursor[0] = _cursor[1] = _cursor[2] = 0;
  _start_idx                           = 0;
  _stack_top                           = 0;
  _idle_timer                          = 0;
  _dirty                               = true;
  _edit_val                            = 0;

  for (uint8_t i = 0; i < g_menu_tree_count; i++)
  {
    if (g_menu_tree[i].parent_idx < 0)
    {
      _parent[0]    = &g_menu_tree[i];
      _current_menu = &g_menu_tree[i];
      _item_count   = _current_menu->child_count;
      _first_idx    = _current_menu->first_child_idx;
      break;
    }
  }

  iic_oled.clear();
}

/* ===================================================================
 * Menu::tick()
 * =================================================================== */

void Menu::tick()
{
  read_keys();
  state_machine();
  render();
}

/* ===================================================================
 * read_keys() — 信号量消费 (3键)
 * =================================================================== */

void Menu::read_keys()
{
  SemaphoreHandle_t sems[3] = {
    key_1.get_sem(),
    key_2.get_sem(),
    key_3.get_sem()};

  for (int i = 0; i < 3; i++)
  {
    _key_short[i] = false;
    _key_long[i]  = false;

    if (!sems[i]) continue;

    UBaseType_t cnt = uxSemaphoreGetCount(sems[i]);
    if (cnt == 0U) continue;

    _key_short[i] = (cnt == 1U);
    _key_long[i]  = (cnt >= 2U);

    while (cnt > 0U)
    {
      xSemaphoreTake(sems[i], 0);
      cnt--;
    }
  }
}

/* ===================================================================
 * state_machine()
 * =================================================================== */

void Menu::state_machine()
{
  /* 全局: key_3 长按 → HOME */
  if (_key_long[2])
  {
    menu_home();
    _dirty = true;
    return;
  }

  switch (_state)
  {
    case ST_IDLE:
      if (_key_short[2]) /* ENTER 进入菜单 */
      {
        _state          = ST_NAV;
        _cursor[_level] = 0;
        _start_idx      = 0;
        _idle_timer     = 0;
        _dirty          = true;
      }
      break;

    case ST_NAV:
    {
      _idle_timer++;
      if (_idle_timer >= MENU_IDLE_TIMEOUT)
      {
        menu_home();
        _dirty = true;
        return;
      }

      uint8_t cnt = _item_count;
      if (cnt == 0) break;

      /* key_1: UP */
      if (_key_short[0])
      {
        _idle_timer = 0;
        if (_cursor[_level] > 0)
        {
          _cursor[_level]--;
          if (_cursor[_level] < _start_idx)
            _start_idx = _cursor[_level];
        }
        _dirty = true;
      }

      /* key_2: DOWN */
      if (_key_short[1])
      {
        _idle_timer = 0;
        if (_cursor[_level] < cnt - 1)
        {
          _cursor[_level]++;
          if (_cursor[_level] >= _start_idx + MENU_MAX_VISIBLE_ITEMS)
            _start_idx = _cursor[_level] - MENU_MAX_VISIBLE_ITEMS + 1;
        }
        _dirty = true;
      }

      /* key_3: ENTER */
      if (_key_short[2])
      {
        _idle_timer = 0;
        menu_enter();
        _dirty = true;
      }
      break;
    }

    case ST_EDIT:
    {
      _idle_timer++;
      if (_idle_timer >= MENU_IDLE_TIMEOUT)
      {
        value_cancel();
        _dirty = true;
        return;
      }

      uint8_t         idx  = _first_idx + _cursor[_level];
      const MenuItem *item = (idx < g_menu_tree_count) ? &g_menu_tree[idx] : nullptr;
      if (!item) break;

      if (_key_short[0])
      {
        _idle_timer = 0;
        _edit_val += item->val_step;
        if (_edit_val > item->val_max) _edit_val = item->val_max;
        _dirty = true;
      }
      if (_key_short[1])
      {
        _idle_timer = 0;
        _edit_val -= item->val_step;
        if (_edit_val < item->val_min) _edit_val = item->val_min;
        _dirty = true;
      }
      if (_key_short[2])
      {
        _idle_timer = 0;
        value_confirm();
        _dirty = true;
      }
      break;
    }
  }
}

/* ===================================================================
 * 导航
 * =================================================================== */

void Menu::menu_enter()
{
  uint8_t idx = _first_idx + _cursor[_level];
  if (idx >= g_menu_tree_count) return;
  const MenuItem *item = &g_menu_tree[idx];

  switch (item->type)
  {
    case MENU_SUBMENU:
      if (item->child_count > 0 && item->first_child_idx >= 0
          && _stack_top < MENU_MAX_LEVEL - 1)
      {
        _parent[_stack_top++] = item;
        _level++;
        _cursor[_level] = 0;
        _start_idx      = 0;
        _current_menu   = item;
        _item_count     = item->child_count;
        _first_idx      = item->first_child_idx;
      }
      break;

    case MENU_BACK:
      menu_back();
      break;

    case MENU_ACTION:
    case MENU_TOGGLE:
      menu_action();
      break;

    case MENU_VALUE:
      _state    = ST_EDIT;
      _edit_val = item->val_default;
      break;

    case MENU_INFO:
      break;
  }
}

void Menu::menu_back()
{
  if (_stack_top > 0)
  {
    _stack_top--;
    _level--;
    const MenuItem *p = _parent[_stack_top];
    _cursor[_level]   = 0;
    _start_idx        = 0;
    _current_menu     = p;
    _item_count       = p->child_count;
    _first_idx        = p->first_child_idx;
  }
  else
  {
    menu_home();
  }
}

void Menu::menu_home()
{
  _state     = ST_IDLE;
  _level     = 0;
  _cursor[0] = _cursor[1] = _cursor[2] = 0;
  _start_idx                           = 0;
  _stack_top                           = 0;
  _idle_timer                          = 0;

  for (uint8_t i = 0; i < g_menu_tree_count; i++)
  {
    if (g_menu_tree[i].parent_idx < 0)
    {
      _parent[0]    = &g_menu_tree[i];
      _current_menu = &g_menu_tree[i];
      _item_count   = _current_menu->child_count;
      _first_idx    = _current_menu->first_child_idx;
      break;
    }
  }
}

void Menu::menu_action()
{
  uint8_t idx = _first_idx + _cursor[_level];
  if (idx >= g_menu_tree_count) return;
  const MenuItem *item = &g_menu_tree[idx];

  if (item->event_bit == 0U) return;

  if (item->type == MENU_TOGGLE || item->type == MENU_VALUE)
    menu_event_set_with_value(item->event_bit, _edit_val);
  else
    menu_event_set(item->event_bit);
}

void Menu::value_confirm()
{
  menu_action();
  _state = ST_NAV;
}

void Menu::value_cancel()
{
  _state = ST_NAV;
}

/* ===================================================================
 * 渲染
 * =================================================================== */

void Menu::render()
{
  if (!_dirty) return;
  _dirty = false;

  switch (_state)
  {
    case ST_IDLE: render_idle(); break;
    case ST_NAV:  render_nav();  break;
    case ST_EDIT: render_edit(); break;
  }
  iic_oled.refresh();
}

/* ── IDLE 首页 ── */
void Menu::render_idle()
{
  char buf[MENU_CHARS_PER_LINE + 1];
  iic_oled.clear();

  user_system::format(buf, sizeof(buf), "MSPM0 BDC Core");
  iic_oled.show_string(0, 0, buf);
  user_system::format(buf, sizeof(buf), "K3:Menu K3L:Home");
  iic_oled.show_string(0, 1, buf);

  /* 显示最新陀螺仪角度 */
  const TiGyro::ReportData &r = ti_gyro.get_last_report();
  if (r.valid)
    user_system::format(buf, sizeof(buf), "Yaw:%6.1f", (double)r.yaw_deg);
  else
    user_system::format(buf, sizeof(buf), "Yaw:  --.--");
  iic_oled.show_string(0, 2, buf);

  user_system::format(buf, sizeof(buf), "Tick:%lu", (unsigned long)xTaskGetTickCount());
  iic_oled.show_string(0, 3, buf);
}

/* ── 导航菜单 ── */
void Menu::render_nav()
{
  char buf[MENU_CHARS_PER_LINE + 1];
  iic_oled.clear();

  /* Row 0: 标题 */
  if (_level > 0 && _stack_top > 0)
    user_system::format(buf, sizeof(buf), "%s", _parent[_stack_top - 1]->name);
  else
    user_system::format(buf, sizeof(buf), "Main Menu");
  iic_oled.show_string(0, 0, buf);

  uint8_t cnt = _item_count;
  if (cnt == 0) return;

  uint8_t vis = (cnt < MENU_MAX_VISIBLE_ITEMS) ? cnt : MENU_MAX_VISIBLE_ITEMS;

  for (uint8_t r = 0; r < vis; r++)
  {
    uint8_t item_idx = _start_idx + r;
    if (item_idx >= cnt) break;

    uint8_t         tree_idx = _first_idx + item_idx;
    const MenuItem *it       = &g_menu_tree[tree_idx];
    bool            is_sel   = (item_idx == _cursor[_level]);

    user_system::format(buf, sizeof(buf), "%c%s", is_sel ? '>' : ' ', it->name);
    iic_oled.show_string(0, (uint8_t)(r + 1), buf);
  }
}

/* ── 数值编辑 ── */
void Menu::render_edit()
{
  char buf[MENU_CHARS_PER_LINE + 1];
  iic_oled.clear();

  uint8_t         idx = _first_idx + _cursor[_level];
  const MenuItem *it  = (idx < g_menu_tree_count) ? &g_menu_tree[idx] : nullptr;
  if (!it) return;

  /* Row 0 */
  user_system::format(buf, sizeof(buf), "%s", it->name);
  iic_oled.show_string(0, 0, buf);

  /* Row 1: 进度条 */
  int32_t range = it->val_max - it->val_min;
  if (range <= 0) range = 1;
  int bar = (int)(((_edit_val - it->val_min) * 14LL) / range);
  if (bar < 0) bar = 0;
  if (bar > 14) bar = 14;

  buf[0] = '[';
  for (int i = 0; i < 14; i++) buf[1 + i] = (i < bar) ? '=' : ' ';
  buf[15] = ']';
  buf[16] = '\0';
  iic_oled.show_string(0, 1, buf);

  /* Row 2 */
  user_system::format(buf, sizeof(buf), "%ld / %ld", (long)_edit_val, (long)it->val_max);
  iic_oled.show_string(0, 2, buf);

  /* Row 3 */
  iic_oled.show_string(0, 3, (char *)"K3:OK  K3L:CANCEL");
}

/* ===================================================================
 * menu_task (50ms 周期)
 * =================================================================== */

void menu_task(void *arg)
{
  (void)arg;
  g_menu.init();
  while (1)
  {
    g_menu.tick();
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
