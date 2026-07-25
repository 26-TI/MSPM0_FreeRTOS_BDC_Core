/**
 * @file can_send_test.cpp
 * @brief CAN 非阻塞发送测试 — 每 500ms 发一帧，忙则跳过
 *
 * @note 发送策略: send() 返回 false 表示 TX Buffer 忙，
 *       该周期跳过不发送。非阻塞，不等待。
 */

#include "test.hpp"
#include "bsp_cfg.hpp"

#include "task.h"

/* ===================================================================
 * 测试任务
 * =================================================================== */

void can_send_test_task(void *arg)
{
  (void)arg;

  bsp_uart4.uart_printf("[CAN-SEND] Non-blocking, 500ms interval\r\n");

  uint32_t cnt = 0U;

  while (1)
  {
    uint8_t data[8];
    for (uint8_t i = 0U; i < 8U; i++)
    {
      data[i] = (uint8_t)(cnt + i);
    }

    if (bsp_can.send(0x123U, data, 8U))
    {
      bsp_uart4.uart_printf("[CAN-SEND] #%lu OK D0=%02X D7=%02X\r\n",
                            cnt, data[0], data[7]);
    }
    else
    {
      bsp_uart4.uart_printf("[CAN-SEND] #%lu BUSY\r\n", cnt);
    }

    cnt++;
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
