/**
 * @file bsp_uart.hpp
 * @brief MSPM0 BspUart — RX: FIFO+中断 / TX: DMA / 流缓冲区解耦
 *
 * 架构:
 *   - RX: 硬件 FIFO + 超时中断 → ISR 搬数据 → StreamBuffer
 *   - TX: StreamBuffer → DMA (字节传输) → UART TXDATA 寄存器
 *   - send/receive 为系统阻塞 API (任务挂起不占 CPU)
 *
 * @note 不能在中断中调用 send/receive（调用 FreeRTOS 阻塞 API）
 */

#ifndef __BSP_UART_MSPM0_HPP__
#define __BSP_UART_MSPM0_HPP__

#include "FreeRTOS.h" // IWYU pragma: keep
#include "stream_buffer.h"
#include "ti_msp_dl_config.h" // IWYU pragma: keep

/**
 * @brief UART 驱动模板类
 * @tparam B  TX DMA 缓冲区大小 (默认 256 字节)
 */
template <size_t B = 256>
class BspUart
{
public:
  /** @brief UART 初始化配置 */
  struct Config
  {
    UART_Regs *uart;           ///< UART 外设基址
    IRQn_Type  irq;            ///< 中断号
    uint8_t    dma_tx_chan;    ///< TX DMA 通道号
    uint8_t    dma_tx_trigger; ///< TX DMA 触发源
  };

  BspUart();
  ~BspUart();

  /**
   * @brief 初始化 UART 驱动 — 创建流缓冲区 + 配置 FIFO/DMA + 使能中断
   * @param cfg  硬件绑定参数
   * @return true=成功, false=流缓冲区创建失败
   */
  bool init(const Config &cfg);

  /**
   * @brief 通过 DMA 发送数据 (系统阻塞)
   * @param data    数据指针
   * @param size    数据长度 (字节)
   * @param timeout 超时时间 (tick), 默认 portMAX_DELAY
   * @return 实际发送的字节数
   */
  size_t send(const uint8_t *data, size_t size, uint32_t timeout = portMAX_DELAY);

  /**
   * @brief 格式化字符串通过 DMA 发送 (系统阻塞)
   * @param fmt  格式化字符串
   * @param ...  可变参数
   * @return 实际发送的字节数
   */
  int uart_printf(const char *fmt, ...);

  /**
   * @brief 从 RX 流缓冲区接收数据 (系统阻塞)
   * @param buf     接收缓冲区
   * @param size    期望接收的字节数
   * @param timeout 超时时间 (tick), 默认 portMAX_DELAY
   * @return 实际接收的字节数
   */
  size_t receive(uint8_t *buf, size_t size, uint32_t timeout = portMAX_DELAY);

  /**
   * @brief UART 中断服务处理 (由 IRQHandler 调用)
   *
   * 处理 RX FIFO 满 / RX 超时 → StreamBuffer 写入  以及
   *     TX DMA 完成 → StreamBuffer 读取 → 启动下一次 DMA 传输
   *
   * @param woken  任务唤醒标志 (传给 FreeRTOS ISR API)
   */
  void handle_isr(BaseType_t *woken);

  /** @brief 获取绑定的 UART 外设基址 */
  UART_Regs *get_uart() const { return _cfg.uart; }

  /** @brief 根据 UART 基址查找 BspUart 实例 */
  static BspUart *find(UART_Regs *uart);

private:
  /** @brief 启动 TX DMA 传输 (从 _tx_stream 读取数据写入 DMA) */
  void tx_kickoff();

  Config               _cfg{};
  StreamBufferHandle_t _rx_stream = nullptr;                     ///< RX 流缓冲区句柄
  StreamBufferHandle_t _tx_stream = nullptr;                     ///< TX 流缓冲区句柄
  uint8_t              _tx_buf[B] __attribute__((aligned(4))){}; ///< TX DMA 本地缓冲区 (4 字节对齐)
  bool                 _tx_dma_busy = false;                     ///< TX DMA 忙碌标志

  static constexpr size_t MAX_INST = 4;         ///< 最大实例数
  static BspUart         *_instances[MAX_INST]; ///< 实例查找表
  static size_t           _inst_count;          ///< 当前实例计数
};

#endif
