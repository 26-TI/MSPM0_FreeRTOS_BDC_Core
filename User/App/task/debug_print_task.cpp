/**
 * @file debug_print_task.cpp
 * @brief 菜单事件信号量测试任务 — 阻塞等待菜单事件并打印
 *
 * 测试内容:
 *   1. 阻塞等待任意菜单事件信号量 (xSemaphoreTake, portMAX_DELAY)
 *   2. 收到信号量后串口打印事件名称和附带值
 *   3. 验证菜单 → 信号量 → 任务 的完整链路
 *
 * 按键操作 → 串口输出示例:
 * @code
 *   Motor → Start Motor   →  "[MENU] EVT_MOTOR_START"
 *   Motor → Set Speed=50  →  "[MENU] EVT_MOTOR_SET_SPEED  val=50"
 *   Debug → Gyro Prt ON   →  "[MENU] EVT_GYRO_PRINT_ON"
 * @endcode
 */

#include "task.hpp"
#include "menu_event.hpp"
#include "bsp_cfg.hpp"

#include "task.h"

// ==========================================================================
// debug_print_task — 信号量测试 (50ms 轮询, 优先级 3)
// ==========================================================================

/**
 * @brief 轮询所有 g_menu_sems[] 信号量, 收到后串口打印事件名称
 *
 * 用于验证 菜单 → 信号量 give → 任务 take 的完整链路。
 *
 * @param arg  未使用
 */
void debug_print_task(void *arg)
{
  (void)arg;

  while (1)
  {
    /* ── 遍历所有事件信号量, 阻塞等待任意一个 ── */
    for (int i = 0; i < MENU_EVENT_COUNT; i++)
    {
      if (!g_menu_sems[i]) continue;

      /* 非阻塞尝试 take, 有就处理, 没有就下一个 */
      if (xSemaphoreTake(g_menu_sems[i], 0) == pdTRUE)
      {
        int32_t val = g_menu_value;

        switch (i)
        {
          /* 电机 */
          case EVT_IDX_MOTOR_START:
            bsp_uart4.uart_printf("[MENU] EVT_MOTOR_START\r\n");
            break;
          case EVT_IDX_MOTOR_STOP:
            bsp_uart4.uart_printf("[MENU] EVT_MOTOR_STOP\r\n");
            break;
          case EVT_IDX_MOTOR_SET_SPEED:
            bsp_uart4.uart_printf("[MENU] EVT_MOTOR_SET_SPEED  val=%ld\r\n", (long)val);
            break;

          /* 陀螺仪打印 */
          case EVT_IDX_GYRO_PRINT_ON:
            bsp_uart4.uart_printf("[MENU] EVT_GYRO_PRINT_ON\r\n");
            break;
          case EVT_IDX_GYRO_PRINT_OFF:
            bsp_uart4.uart_printf("[MENU] EVT_GYRO_PRINT_OFF\r\n");
            break;

          /* 巡线打印 */
          case EVT_IDX_TRACK_PRINT_ON:
            bsp_uart4.uart_printf("[MENU] EVT_TRACK_PRINT_ON\r\n");
            break;
          case EVT_IDX_TRACK_PRINT_OFF:
            bsp_uart4.uart_printf("[MENU] EVT_TRACK_PRINT_OFF\r\n");
            break;

          /* 电机数据打印 */
          case EVT_IDX_MOTOR_PRINT_ON:
            bsp_uart4.uart_printf("[MENU] EVT_MOTOR_PRINT_ON\r\n");
            break;
          case EVT_IDX_MOTOR_PRINT_OFF:
            bsp_uart4.uart_printf("[MENU] EVT_MOTOR_PRINT_OFF\r\n");
            break;

          /* CPU */
          case EVT_IDX_CPU_PRINT_ON:
            bsp_uart4.uart_printf("[MENU] EVT_CPU_PRINT_ON\r\n");
            break;
          case EVT_IDX_CPU_PRINT_OFF:
            bsp_uart4.uart_printf("[MENU] EVT_CPU_PRINT_OFF\r\n");
            break;

          /* 一键全开/全关 */
          case EVT_IDX_PRINT_ALL_ON:
            bsp_uart4.uart_printf("[MENU] EVT_PRINT_ALL_ON\r\n");
            break;
          case EVT_IDX_PRINT_ALL_OFF:
            bsp_uart4.uart_printf("[MENU] EVT_PRINT_ALL_OFF\r\n");
            break;

          /* 循线控制 */
          case EVT_IDX_LINE_CTRL_ON:
            bsp_uart4.uart_printf("[MENU] EVT_LINE_CTRL_ON\r\n");
            break;
          case EVT_IDX_LINE_CTRL_OFF:
            bsp_uart4.uart_printf("[MENU] EVT_LINE_CTRL_OFF\r\n");
            break;

          default:
            bsp_uart4.uart_printf("[MENU] EVT_UNKNOWN idx=%d\r\n", i);
            break;
        }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(50)); /* 50ms 轮询周期 */
  }
}
