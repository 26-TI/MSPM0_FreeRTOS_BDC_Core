/**
 * @file can_recv_test.cpp
 * @brief CAN 接收测试 — 轮询模式 + ISR 模式
 *
 * @note 轮询: recvPoll() 搬 FIFO→StreamBuffer, recvRead(timeout=0) 非阻塞取
 * @note ISR: enableIrq() 后 ISR 自动搬, recvRead(timeout=MAX) 系统阻塞等
 */

#include "test.hpp"
#include "bsp_cfg.hpp"

#include "task.h"

/* ===================================================================
 * ISR 接收 — 中断自动搬 FIFO→StreamBuffer, recvRead 系统阻塞等
 * =================================================================== */

void can_recv_isr_test_task(void *arg)
{
  (void)arg;

  bsp_can.enable_irq();
  bsp_uart4.uart_printf("[CAN-RECV-ISR] ISR mode (system-blocking)\r\n");

  uint32_t total = 0U;

  while (1)
  {
    uint32_t rx_id;
    uint8_t  rx_data[8];
    uint8_t  rx_len;

    /* 系统阻塞: 任务挂起等 StreamBuffer，ISR 写入后自动唤醒 */
    if (bsp_can.recv_read(&rx_id, rx_data, &rx_len, portMAX_DELAY))
    {
      total++;
      bsp_uart4.uart_printf(
        "[CAN-RECV-ISR] #%lu ID=0x%03lX LEN=%u "
        "%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
        total,
        rx_id,
        rx_len,
        rx_data[0],
        rx_data[1],
        rx_data[2],
        rx_data[3],
        rx_data[4],
        rx_data[5],
        rx_data[6],
        rx_data[7]);
    }
  }
}
