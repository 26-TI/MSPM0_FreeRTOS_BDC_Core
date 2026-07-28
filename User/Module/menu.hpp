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

#define MENU_CHARS_PER_LINE 16U
#define MENU_VISIBLE_ROWS 4U
#define MENU_MAX_VISIBLE_ITEMS 3U
#define MENU_NAME_MAX 15U
#define MENU_MAX_ITEMS 32U
#define MENU_MAX_LEVEL 3U
#define MENU_IDLE_TIMEOUT 100U

// ---- 菜单项类型 ----

/**
 * @brief 菜单项类型枚举
 */
enum MenuItemType
{
  MENU_SUBMENU = 0, ///< 子菜单入口
  MENU_ACTION  = 1, ///< 动作项 (K3 触发事件)
  MENU_VALUE   = 2, ///< 数值编辑项 (K3 进入编辑, K3 确认)
  MENU_TOGGLE  = 3, ///< 开关项 (K3 触发事件)
  MENU_INFO    = 4, ///< 只读信息项
  MENU_BACK    = 5, ///< 返回上级
};

// ---- 菜单项结构 ----

/**
 * @brief 菜单项静态定义
 *
 * 菜单树由 g_menu_tree[] 数组定义, 通过 parent_idx / first_child_idx
 * 形成树形结构, 不支持运行时修改。
 */
struct MenuItem
{
  char         name[MENU_NAME_MAX]; ///< 显示名称
  MenuItemType type;                ///< 菜单项类型
  uint16_t     event_bit;           ///< 触发的事件位掩码 (0=无事件)
  int32_t      val_min;             ///< 数值编辑: 最小值
  int32_t      val_max;             ///< 数值编辑: 最大值
  int32_t      val_step;            ///< 数值编辑: 步长
  int32_t      val_default;         ///< 数值编辑: 默认值
  int8_t       parent_idx;          ///< 父菜单在 g_menu_tree 中的索引 (-1=根)
  int8_t       first_child_idx;     ///< 首个子项索引 (-1=无子项)
  uint8_t      child_count;         ///< 子项数量
};

// ---- 菜单类 ----

/**
 * @brief OLED 菜单引擎 — 3 级树形菜单 + 滚动 + 事件分发
 *
 * 状态机:
 * @code
 *   ST_IDLE ──(K3 短按)──▶ ST_NAV ──(K3 短按)──▶ ST_EDIT
 *      ▲                      │                      │
 *      └──(K3 长按 / 超时)────┘◀──(K3 确认/取消)─────┘
 * @endcode
 *
 * 按键映射:
 *   key_1 短: UP     key_2 短: DOWN
 *   key_3 短: ENTER  key_3 长: HOME
 *
 * 事件分发:
 *   菜单项激活时通过 menu_event_give() 给出对应信号量,
 *   消费者任务通过 xSemaphoreTake(g_menu_sems[idx], ...) 接收。
 */
class Menu
{
public:
  /** @brief 初始化菜单 — 定位根节点 + 清除 OLED */
  void init();

  /** @brief 每周期入口 (读按键 → 状态机 → 渲染) */
  void tick();

private:
  /** @brief 读取按键信号量, 区分短按/长按 */
  void read_keys();

  /** @brief 菜单状态机主循环 */
  void state_machine();

  /** @brief 进入子菜单 / 触发动作 / 进入编辑 */
  void menu_enter();

  /** @brief 返回上级菜单 */
  void menu_back();

  /** @brief 返回 IDLE 首页 */
  void menu_home();

  /** @brief 执行菜单动作 — 给出事件信号量 */
  void menu_action();

  /** @brief 确认数值编辑并触发事件 */
  void value_confirm();

  /** @brief 取消数值编辑 */
  void value_cancel();

  /** @brief 渲染入口 (脏检查) */
  void render();

  /** @brief 渲染 IDLE 首页 */
  void render_idle();

  /** @brief 渲染导航菜单列表 */
  void render_nav();

  /** @brief 渲染数值编辑界面 */
  void render_edit();

  // ── 当前菜单上下文 ──
  const MenuItem *_current_menu = nullptr; ///< 当前菜单节点
  uint8_t         _item_count   = 0;       ///< 当前菜单子项数量
  int8_t          _first_idx    = 0;       ///< 首个子项在 g_menu_tree 中的索引

  // ── 导航状态 ──
  enum State
  {
    ST_IDLE,
    ST_NAV,
    ST_EDIT
  };                                                 ///< 菜单状态枚举
  State           _state                  = ST_IDLE; ///< 当前状态
  uint8_t         _level                  = 0;       ///< 当前层级 (0=根)
  uint8_t         _cursor[3]              = {};      ///< 每层光标位置
  uint8_t         _start_idx              = 0;       ///< 可视区起始索引
  const MenuItem *_parent[MENU_MAX_LEVEL] = {};      ///< 父节点栈
  uint8_t         _stack_top              = 0;       ///< 父节点栈顶

  int32_t  _edit_val   = 0;    ///< 数值编辑的当前值
  uint16_t _idle_timer = 0;    ///< IDLE 超时计时器
  bool     _dirty      = true; ///< 渲染脏标志

  bool _key_short[3] = {}; ///< 三键短按标志 (K1/K2/K3)
  bool _key_long[3]  = {}; ///< 三键长按标志 (K1/K2/K3)
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
