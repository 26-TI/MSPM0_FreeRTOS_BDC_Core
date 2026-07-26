/**
 * @file bsp_can.hpp
 * @brief MSPM0 BspCan — 非阻塞发送 + StreamBuffer 接收 + ISR 驱动
 *
 * @note 基于 example/bsp_can copy 的已验证逻辑。
 *       PA26=CANTX, PA27=CANRX, 1Mbps, MCAN0。
 *
 * @note 接收: 轮询或 ISR 从 MCAN FIFO 读取后写入 StreamBuffer，
 *       任务层用 recvRead() 系统阻塞等待（任务挂起，不占 CPU）。
 *
 * @note 每条消息 13 字节: ID(4B) + DLC(1B) + DATA(8B)
 *
 * @note 发送为非阻塞: 检查 TX Buffer 忙则返回 false，上层决定重试或丢弃。
 */

#ifndef __BSP_CAN_MSPM0_HPP__
#define __BSP_CAN_MSPM0_HPP__

#include <stdint.h>

#include "FreeRTOS.h" // IWYU pragma: keep
#include "stream_buffer.h"

/// 每条 CAN 消息字节数: ID(4B) + DLC(1B) + DATA(8B)
#define CAN_MSG_SIZE 13U
/// RX StreamBuffer 容量（消息条数）
#define CAN_RX_BUF_MSGS 32U

class BspCan
{
public:
  enum Mode
  {
    NORMAL   = 0,
    LOOPBACK = 1
  };

  struct Status
  {
    uint32_t tx_err_cnt;
    uint32_t rx_err_cnt;
    uint32_t activity;
    uint32_t last_err_code;
    bool     bus_off;
  };

  void init(Mode mode = NORMAL);
  void enable_irq();
  void disable_irq();

  /** @brief 非阻塞发送 */
  bool send(uint32_t id, const uint8_t *data, uint8_t len);

  /** @brief 轮询: 从 FIFO 读取写入 StreamBuffer（非阻塞） */
  void recv_poll();

  /**
   * @brief 从 StreamBuffer 读取一条消息（系统阻塞: 任务挂起等数据）
   * @param[out] id      输出 CAN ID
   * @param[out] data    输出数据 (>=8B)
   * @param[out] len     输出长度
   * @param      timeout 超时 (ticks), 0=非阻塞, portMAX_DELAY=永久等
   * @return true 有数据, false 超时
   */
  bool recv_read(uint32_t *id, uint8_t *data, uint8_t *len, uint32_t timeout = portMAX_DELAY);

  void get_status(Status *st);
  void set_loopback(bool enable);

  /** @brief StreamBuffer 中可读字节数 */
  size_t rx_available() const;

  void isr_handler();

private:
  void write_unlock();
  void write_lock();

  bool                 _loopback  = false;
  StreamBufferHandle_t _rx_stream = nullptr;
  StreamBufferHandle_t _tx_stream = nullptr;

  /** 打包一条消息写入 StreamBuffer（非阻塞，满则丢弃） */
  void _push_rx(uint32_t id, uint8_t dlc, const uint8_t *data);
};

#endif /* __BSP_CAN_MSPM0_HPP__ */
