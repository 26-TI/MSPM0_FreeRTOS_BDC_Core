/**
 * @file bsp_uart.cpp
 * @brief MSPM0 BspUart — RX: FIFO+中断 / TX: DMA / 流缓冲区解耦
 *
 * @note 如何使用？ 配置好rx fifo为全满 tx fifo为全空。然后打开tx的dma,把配置从word改成byte。再打开中断，receive、tx timeout、dma done tx三个中断。
 *
 * @note 然后在中断里处理数据，rx fifo的数据读出来放到流缓冲区里，tx dma done中断里从流缓冲区里取数据放到tx dma buffer里。然后在任务里调用receive和send函数就可以了。
 *
 * @note 先 init，然后就可以在任务中使用 send 和 receive 了。
 *       send/receive 都是系统阻塞（FreeRTOS StreamBuffer，任务挂起不占 CPU），
 *       send 把数据放到 TX 流缓冲区里然后启动 TX DMA 传输，
 *       receive 从 RX 流缓冲区里取数据。不能在中断中使用。
 * 
 */

#include "bsp_uart.hpp"
#include "user_system.hpp"

#include <stdarg.h>
#include <stdio.h>

void *__dso_handle = nullptr;

/* ==================== 静态成员 ==================== */
template <size_t B>
BspUart<B> *BspUart<B>::_instances[MAX_INST] = {};

template <size_t B>
size_t BspUart<B>::_inst_count = 0;

/* ==================== 构造 / 析构 / 查找 ==================== */
template <size_t B>
BspUart<B>::BspUart()
{
  if (_inst_count < MAX_INST)
    _instances[_inst_count++] = this;
}
template <size_t B>
BspUart<B>::~BspUart()
{
  if (_rx_stream)
    vStreamBufferDelete(_rx_stream);
  if (_tx_stream)
    vStreamBufferDelete(_tx_stream);
}

template <size_t B>
BspUart<B> *BspUart<B>::find(UART_Regs *uart)
{
  for (size_t i = 0; i < _inst_count; i++)
    if (_instances[i] && _instances[i]->get_uart() == uart)
      return _instances[i];
  return nullptr;
}

/**
 * @brief 初始化函数 初始化串口驱动对象，配置必要的FreeRTOS对象，需要在FreeRTOS调度器启动前调用
 * 
 * @tparam BUF_SIZE 
 * @param cfg 
 * @return true 
 * @return false 
 */
template <size_t BUF_SIZE>
bool BspUart<BUF_SIZE>::init(const Config &cfg)
{
  _cfg = cfg;

  _rx_stream = xStreamBufferCreate(BUF_SIZE * 2, 1);
  _tx_stream = xStreamBufferCreate(BUF_SIZE * 2, 1);
  if (!_rx_stream || !_tx_stream)
    return false;

  NVIC_EnableIRQ(_cfg.irq);

  /* RX: FIFO + RX_TIMEOUT（CPU 手动读 FIFO，不用 DMA） */
  DL_UART_Main_enableFIFOs(_cfg.uart);
  DL_UART_Main_setRXFIFOThreshold(_cfg.uart, DL_UART_RX_FIFO_LEVEL_1_2_FULL);
  DL_UART_setRXInterruptTimeout(_cfg.uart, 6U);
  DL_UART_Main_enableInterrupt(_cfg.uart,
                               DL_UART_MAIN_INTERRUPT_RX | DL_UART_MAIN_INTERRUPT_RX_TIMEOUT_ERROR
                                 | DL_UART_MAIN_INTERRUPT_DMA_DONE_TX);

  /* TX: DMA */
  DL_DMA_disableChannel(DMA, _cfg.dma_tx_chan);
  DL_DMA_setSrcAddr(DMA, _cfg.dma_tx_chan, (uint32_t)_tx_buf);
  DL_DMA_setDestAddr(DMA, _cfg.dma_tx_chan, (uint32_t)&(_cfg.uart->TXDATA));
  return true;
}

/**
 * @brief 处理 UART 中断
 * 
 * @tparam B 
 * @param woken 
 */
template <size_t B>
void BspUart<B>::handle_isr(BaseType_t *woken)
{
  switch (DL_UART_Main_getPendingInterrupt(_cfg.uart))
  {

    case DL_UART_MAIN_IIDX_RX:
    case DL_UART_MAIN_IIDX_RX_TIMEOUT_ERROR:
    {
      uint8_t  buf[8];
      uint16_t n = DL_UART_Main_drainRXFIFO(_cfg.uart, buf, 8);
      if (n)
        xStreamBufferSendFromISR(_rx_stream, buf, n, woken);
      break;
    }

    case DL_UART_MAIN_IIDX_DMA_DONE_TX:
    {
      size_t n = xStreamBufferReceiveFromISR(_tx_stream, _tx_buf, B, woken);
      if (n)
      {
        DL_DMA_disableChannel(DMA, _cfg.dma_tx_chan);
        DL_DMA_setSrcAddr(DMA, _cfg.dma_tx_chan, (uint32_t)_tx_buf);
        DL_DMA_setTransferSize(DMA, _cfg.dma_tx_chan, n);
        DL_DMA_enableChannel(DMA, _cfg.dma_tx_chan);
      }
      else
      {
        _tx_dma_busy = false;
      }
      break;
    }

    default:
      break;
  }
}

/**
 * @brief 发送数据到 UART 发送缓冲区，并启动 DMA 传输
 * 
 * @tparam B 模板中的缓冲区大小
 * @param data 数据数组
 * @param size 数组长度
 * @param timeout 可以等待的超时时间
 * @return size_t 存入长度
 */
template <size_t B>
size_t BspUart<B>::send(const uint8_t *data, size_t size, uint32_t timeout)
{
  size_t n = xStreamBufferSend(_tx_stream, data, size, timeout);
  if (n)
    tx_kickoff();
  return n;
}


template <size_t B>
int BspUart<B>::uart_printf(const char *fmt, ...)
{
  if (!fmt)
    return -1;

  char    buf[B];
  va_list args;
  va_start(args, fmt);
  int n = user_system::format_v(buf, sizeof(buf), fmt, args);
  va_end(args);

  if (n < 0)
    return -1;

  /* 轮询 TX（DMA 硬件触发待查） */
  for (int i = 0; i < n; i++)
  {
    while (DL_UART_Main_isTXFIFOFull(_cfg.uart)) {}
    DL_UART_Main_transmitDataBlocking(_cfg.uart, (uint8_t)buf[i]);
  }
  return n;
}

/**
 * @brief 从 UART 接收缓冲区接收数据
 * 
 * @tparam B 模板中的缓冲区大小
 * @param buf 数据数组
 * @param size 数组长度
 * @param timeout 可以等待的超时时间
 * @return size_t 存入长度
 */
template <size_t B>
size_t BspUart<B>::receive(uint8_t *buf, size_t size, uint32_t timeout)
{
  return xStreamBufferReceive(_rx_stream, buf, size, timeout);
}

/**
 * @brief 尝试TX发送
 * 
 * @tparam B 
 */
template <size_t B>
void BspUart<B>::tx_kickoff()
{
  if (DL_DMA_isChannelEnabled(DMA, _cfg.dma_tx_chan))
    return;
  if (_tx_dma_busy)
    return;
  size_t n = xStreamBufferReceive(_tx_stream, _tx_buf, B, 0);
  if (n)
  {
    _tx_dma_busy = true;
    DL_DMA_disableChannel(DMA, _cfg.dma_tx_chan);
    DL_DMA_setSrcAddr(DMA, _cfg.dma_tx_chan, (uint32_t)_tx_buf);
    DL_DMA_setTransferSize(DMA, _cfg.dma_tx_chan, n);
    DL_DMA_enableChannel(DMA, _cfg.dma_tx_chan);
  }
}

/* ==================== 显式实例化 + UART ISR ==================== */

template class BspUart<64>;
template class BspUart<128>;
template class BspUart<256>;

extern "C" void UART0_IRQHandler(void)
{
  BaseType_t w = pdFALSE;
  if (auto *i = BspUart<64>::find(UART0))
    i->handle_isr(&w);
  portYIELD_FROM_ISR(w);
}
extern "C" void UART1_IRQHandler(void)
{
  BaseType_t w = pdFALSE;
  if (auto *i = BspUart<64>::find(UART1))
    i->handle_isr(&w);
  portYIELD_FROM_ISR(w);
}
extern "C" void UART4_IRQHandler(void)
{
  BaseType_t w = pdFALSE;
  if (auto *i = BspUart<64>::find(UART4))
    i->handle_isr(&w);
  portYIELD_FROM_ISR(w);
}
extern "C" void UART3_IRQHandler(void)
{
  BaseType_t w = pdFALSE;
  if (auto *i = BspUart<64>::find(UART3))
    i->handle_isr(&w);
  portYIELD_FROM_ISR(w);
}

