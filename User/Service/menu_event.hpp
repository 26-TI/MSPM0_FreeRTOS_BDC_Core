/**
 * @file menu_event.hpp
 * @brief 菜单事件 — 事件位定义 + 信号量分发 API
 *
 * 架构:
 * @code
 *   Menu (menu.cpp)                  Consumer Task
 *   ┌──────────────┐                ┌──────────────────────────┐
 *   │ menu_action() │── give ──────│ xSemaphoreTake(sem, ∞)    │
 *   │               │   信号量      │   阻塞等待菜单事件         │
 *   │ menu_action() │── value ────│   g_menu_value 读取设定值   │
 *   └──────────────┘              └──────────────────────────┘
 *          │                                │
 *          └── g_menu_sems[16] (二值信号量数组) ──┘
 *               g_menu_value   (int32_t)
 * @endcode
 *
 * 事件分发基于 FreeRTOS 二值信号量:
 *   - 每个 event_bit 对应一个二值信号量 (g_menu_sems[bit_idx])
 *   - Menu 通过 menu_event_give(event_bit) 给出信号量
 *   - 消费者任务通过 xSemaphoreTake(g_menu_sems[idx], portMAX_DELAY) 阻塞等待
 *   - 信号量不堆积 (二值), 适合 ON/OFF / ACTION 类型事件
 *   - 带值事件通过 g_menu_value 传递参数
 */

#ifndef __MENU_EVENT_HPP__
#define __MENU_EVENT_HPP__

#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h" // IWYU pragma: keep
#include "semphr.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /* ===================================================================
   * 事件位定义 (16-bit) — 同时作为信号量数组索引
   *
   * 每个事件宏既是位掩码 (用于 menu_tree.cpp 的 event_bit 字段),
   * 也是信号量数组索引 (通过 __builtin_ctz 或直接用 EVT_IDX_* 宏).
   * =================================================================== */

  /* ── 信号量数组索引 (0~15) ── */
#define EVT_IDX_MOTOR_START 0
#define EVT_IDX_MOTOR_STOP 1
#define EVT_IDX_MOTOR_SET_SPEED 2
#define EVT_IDX_GYRO_PRINT_ON 3
#define EVT_IDX_GYRO_PRINT_OFF 4
#define EVT_IDX_TRACK_PRINT_ON 5
#define EVT_IDX_TRACK_PRINT_OFF 6
#define EVT_IDX_MOTOR_PRINT_ON 7
#define EVT_IDX_MOTOR_PRINT_OFF 8
#define EVT_IDX_CPU_PRINT_ON 9
#define EVT_IDX_CPU_PRINT_OFF 10
#define EVT_IDX_PRINT_ALL_ON 11
#define EVT_IDX_PRINT_ALL_OFF 12
#define EVT_IDX_LINE_CTRL_ON 13
#define EVT_IDX_LINE_CTRL_OFF 14
#define EVT_IDX_RESERVED_15 15

#define MENU_EVENT_COUNT 16

  /* ── 事件位掩码 (供 menu_tree.cpp 的 event_bit 字段使用) ── */
#define EVT_MOTOR_START (1U << EVT_IDX_MOTOR_START)
#define EVT_MOTOR_STOP (1U << EVT_IDX_MOTOR_STOP)
#define EVT_MOTOR_SET_SPEED (1U << EVT_IDX_MOTOR_SET_SPEED)
#define EVT_GYRO_PRINT_ON (1U << EVT_IDX_GYRO_PRINT_ON)
#define EVT_GYRO_PRINT_OFF (1U << EVT_IDX_GYRO_PRINT_OFF)
#define EVT_TRACK_PRINT_ON (1U << EVT_IDX_TRACK_PRINT_ON)
#define EVT_TRACK_PRINT_OFF (1U << EVT_IDX_TRACK_PRINT_OFF)
#define EVT_MOTOR_PRINT_ON (1U << EVT_IDX_MOTOR_PRINT_ON)
#define EVT_MOTOR_PRINT_OFF (1U << EVT_IDX_MOTOR_PRINT_OFF)
#define EVT_CPU_PRINT_ON (1U << EVT_IDX_CPU_PRINT_ON)
#define EVT_CPU_PRINT_OFF (1U << EVT_IDX_CPU_PRINT_OFF)
#define EVT_PRINT_ALL_ON (1U << EVT_IDX_PRINT_ALL_ON)
#define EVT_PRINT_ALL_OFF (1U << EVT_IDX_PRINT_ALL_OFF)
#define EVT_LINE_CTRL_ON (1U << EVT_IDX_LINE_CTRL_ON)
#define EVT_LINE_CTRL_OFF (1U << EVT_IDX_LINE_CTRL_OFF)
#define EVT_RESERVED_15 (1U << EVT_IDX_RESERVED_15)

  /* ===================================================================
   * 全局变量
   * =================================================================== */

  /** @brief 菜单事件信号量数组 (二值信号量, 每个事件一个) */
  extern SemaphoreHandle_t g_menu_sems[MENU_EVENT_COUNT];

  /** @brief 菜单事件附带的值（如速度设定值、PID 参数等） */
  extern volatile int32_t g_menu_value;

  /* ===================================================================
   * API
   * =================================================================== */

  /**
   * @brief 初始化所有菜单事件信号量 (在 bsp_init 或 init_task 中调用一次)
   */
  void menu_event_init(void);

  /**
   * @brief 给出菜单事件信号量（无参数值）
   * @param event_bit 事件位掩码 (EVT_xxx)
   *
   * @note 内部通过 __builtin_ctz 定位信号量索引
   */
  void menu_event_give(uint16_t event_bit);

  /**
   * @brief 给出菜单事件信号量（带参数值）
   * @param event_bit 事件位掩码
   * @param value     关联数值（如设定速度、PID 参数）
   */
  void menu_event_give_with_value(uint16_t event_bit, int32_t value);

#ifdef __cplusplus
}
#endif

#endif /* __MENU_EVENT_HPP__ */
