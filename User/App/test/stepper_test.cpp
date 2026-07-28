/**
 * @file stepper_test.cpp
 * @brief 步进电机 CAN 测试 — ID=1/2 依次定位 + 广播回零
 *
 * 协议: STEP_POSITION (0x02), 广播 (0x08)
 * CAN ID: 0x300 + node_id  |  广播 ID: 0x300
 */

#include "test.hpp"
#include "bsp_cfg.hpp"
#include "device_cfg.hpp"

#include "FreeRTOS.h"
#include "task.h"

void stepper_test_task(void *arg)
{
  (void)arg;

  vTaskDelay(pdMS_TO_TICKS(2000));
  bsp_uart4.uart_printf("\r\n=====  Stepper CAN Test (2 motors)  =====\r\n");

  bsp_uart4.uart_printf("[INIT] S1=0x%03lX  S2=0x%03lX\r\n", g_step1.can_id(), g_step2.can_id());
  vTaskDelay(pdMS_TO_TICKS(500));

  /* ---- 使能 ---- */
  bsp_uart4.uart_printf("[ENABLE]\r\n");
  g_step1.enable(true);
  vTaskDelay(pdMS_TO_TICKS(50));
  g_step2.enable(true);
  vTaskDelay(pdMS_TO_TICKS(500));

  /* ---- 单电机: S1→90°, S2→180° ---- */
  bsp_uart4.uart_printf("[MOVE] S1 -> 90 deg\r\n");
  g_step1.move_to(90.0f);
  vTaskDelay(pdMS_TO_TICKS(3000));

  bsp_uart4.uart_printf("[MOVE] S2 -> 180 deg\r\n");
  g_step2.move_to(180.0f);
  vTaskDelay(pdMS_TO_TICKS(3000));

  /* ---- 广播回零: 一帧两台全回 0°, 第三槽跳过 ---- */
  vTaskDelay(pdMS_TO_TICKS(500));
  bsp_uart4.uart_printf("[MULTI] S1 S2 -> 0 deg\r\n");
  MultiStep::move_to(0.0f, 0.0f, -99999.0f);
  vTaskDelay(pdMS_TO_TICKS(3000));

  bsp_uart4.uart_printf("[DONE] Stepper test complete.\r\n");

  /* ---- 空闲 ---- */
  while (1)
  {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
