/**
 * @file bsp_uart.hpp
 * @brief MSPM0 BspUart — RX: FIFO+中断 / TX: DMA / 流缓冲区解耦
 */

#ifndef __BSP_UART_MSPM0_HPP__
#define __BSP_UART_MSPM0_HPP__

#include "FreeRTOS.h" // IWYU pragma: keep
#include "stream_buffer.h"
#include "ti_msp_dl_config.h" // IWYU pragma: keep

template <size_t B = 256>
class BspUart
{
public:
  struct Config
  {
    UART_Regs *uart;
    IRQn_Type  irq;
    uint8_t    dma_tx_chan;
    uint8_t    dma_tx_trigger;
  };

  BspUart();
  ~BspUart();

  bool   init(const Config &cfg);
  size_t send(const uint8_t *data, size_t size, uint32_t timeout = portMAX_DELAY);
  int    uart_printf(const char *fmt, ...);
  size_t receive(uint8_t *buf, size_t size, uint32_t timeout = portMAX_DELAY);
  void   handle_isr(BaseType_t *woken);

  UART_Regs      *get_uart() const { return _cfg.uart; }
  static BspUart *find(UART_Regs *uart);

private:
  void tx_kickoff();

  Config               _cfg{};
  StreamBufferHandle_t _rx_stream = nullptr;
  StreamBufferHandle_t _tx_stream = nullptr;
  uint8_t              _tx_buf[B] __attribute__((aligned(4))){};
  bool                 _tx_dma_busy = false;

  static constexpr size_t MAX_INST = 4;
  static BspUart         *_instances[MAX_INST];
  static size_t           _inst_count;
};

#endif
