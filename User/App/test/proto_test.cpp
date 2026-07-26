/**
 * @file proto_test.cpp
 * @brief 串口协议测试 — 收发帧 + 调试打印
 *
 * @note 使用 UART2 (PB17/PB18) 做协议收发。
 *       收到帧后通过调试串口打印并原样回传。
 *       需外部短接 TX-RX 或接上位机进行回环测试。
 */

#include "test.hpp"
#include "bsp_cfg.hpp"
#include "protocol_cfg.hpp"

#include "task.h"

/* ===================================================================
 * 协议回调 — 收到有效帧后的处理
 * =================================================================== */

/**
 * @brief 收到有效帧回调（extern 给 bsp_cfg.cpp 引用）
 *
 * 打印帧信息并原样回传给上位机（回环测试用）。
 * 实际使用时替换为业务逻辑（如控制 LED、设置 PWM 等）。
 *
 * @param cmd  命令码
 * @param data 数据（回调期间有效）
 * @param len  数据长度
 */
void proto_on_command(uint8_t cmd, const uint8_t *data, uint8_t len)
{
  bsp_uart4.uart_printf("[PROTO] RX CMD=0x%02X LEN=%u D0=%02X D1=%02X\r\n",
                        cmd, len, data[0], data[1]);

  /* 原样回传（上位机可验证收发正确性） */
  proto_uart.send(cmd, data, len);
}

/* ===================================================================
 * 发送测试 — 每 500ms 发一帧
 * =================================================================== */

void proto_send_test_task(void *arg)
{
  (void)arg;

  bsp_uart4.uart_printf("[PROTO-SEND] 500ms interval\r\n");

  uint8_t cnt = 0U;

  while (1)
  {
    uint8_t d[2] = { cnt, (uint8_t)(cnt + 1U) };

    if (proto_uart.send(0x10U, d, 2U))
    {
      bsp_uart4.uart_printf("[PROTO-SEND] OK #%u\r\n", cnt);
    }
    else
    {
      bsp_uart4.uart_printf("[PROTO-SEND] FAIL #%u\r\n", cnt);
    }

    cnt++;
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
