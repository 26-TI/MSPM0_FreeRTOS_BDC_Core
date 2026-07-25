/**
 * @file bsp_spi.cpp
 * @brief MSPM0 BspSpi 实现
 *
 * @note 硬件由 SysConfig 生成的 SYSCFG_DL_SPI0_init() 统一初始化。
 *       SPI0: PA9(PICO), PA10(POCI), PA11(SCLK), PA8(CS0 硬件控制)
 *       Mode 0 (CPOL=0, CPHA=0), 8-bit, MSB first, 8MHz。
 */

#include "bsp_spi.hpp"

/* ==================== 构造 ==================== */
BspSpi::BspSpi() {}

/* ==================== 初始化 ==================== */

/**
 * @brief 绑定 SPI 硬件实例
 * @param cfg 外设配置
 * @return true  绑定成功
 * @return false spi 为空指针
 */
bool BspSpi::init(const Config &cfg)
{
  if (!cfg.spi)
  {
    return false;
  }
  _cfg = cfg;
  return true;
}

/* ==================== 裸机阻塞: while 等 FIFO ==================== */

uint8_t BspSpi::transfer(uint8_t tx)
{
  /* 裸机阻塞: 等 TX FIFO 有空位 */
  while (DL_SPI_isTXFIFOFull(_cfg.spi))
  {
  }

  DL_SPI_transmitData8(_cfg.spi, tx);

  /* 裸机阻塞: 等 RX FIFO 有数据 */
  while (DL_SPI_isRXFIFOEmpty(_cfg.spi))
  {
  }

  return DL_SPI_receiveData8(_cfg.spi);
}

/* ==================== 多字节传输 ==================== */

void BspSpi::write(const uint8_t *data, size_t len)
{
  for (size_t i = 0U; i < len; i++)
  {
    transfer(data[i]);
  }
}

void BspSpi::read(uint8_t *buf, size_t len)
{
  for (size_t i = 0U; i < len; i++)
  {
    buf[i] = transfer(0x00);
  }
}

/* ==================== CS 软件控制（预留） ==================== */

void BspSpi::cs_low()
{
  /* 当前使用硬件 CS (SPI0 CS0 = PA8)，此函数为预留接口。
     如需软件 CS 控制，在此实现对应 GPIO 操作。 */
}

void BspSpi::cs_high()
{
  /* 同上，配合 cs_low() 使用 */
}
