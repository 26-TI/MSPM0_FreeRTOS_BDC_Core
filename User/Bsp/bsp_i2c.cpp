/**
 * @file bsp_i2c.cpp
 * @brief MSPM0 BspI2c 实现 — 裸机阻塞 I2C 读写
 */

#include "bsp_i2c.hpp"

/* ================================================================
 *  常量
 * ================================================================ */

static constexpr uint32_t I2C_TIMEOUT = 10000000U; ///< 死等超时 (约 125ms @80MHz)

/* ================================================================
 *  初始化
 * ================================================================ */

bool BspI2c::init(const Config &cfg)
{
  if (!cfg.i2c) return false;
  _i2c = cfg.i2c;
  return true;
}

/* ================================================================
 *  内部辅助
 * ================================================================ */

void BspI2c::_wait_idle()
{
  uint32_t timeout = I2C_TIMEOUT;
  while (!(DL_I2C_getControllerStatus(_i2c) & DL_I2C_CONTROLLER_STATUS_IDLE))
  {
    if (--timeout == 0U) break;
  }
}

void BspI2c::_wait_done()
{
  uint32_t timeout = I2C_TIMEOUT;
  while (DL_I2C_getControllerStatus(_i2c) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS)
  {
    if (--timeout == 0U) break;
  }
  _wait_idle();
}

void BspI2c::_tx_byte(uint8_t b)
{
  uint32_t timeout = I2C_TIMEOUT;
  while (DL_I2C_isControllerTXFIFOFull(_i2c))
  {
    if (--timeout == 0U) break;
  }
  DL_I2C_transmitControllerData(_i2c, b);
}

/* ================================================================
 *  write() — 阻塞写入（任意长度）
 * ================================================================ */

void BspI2c::write(uint8_t dev, const uint8_t *data, size_t len)
{
  if (len == 0) return;

  _wait_idle();

  /* 填充首段数据（FIFO 深 8 字节，填满或填完） */
  size_t first = (len < 8U) ? len : 8U;
  DL_I2C_fillControllerTXFIFO(_i2c, data, first);

  /* 启动传输 */
  DL_I2C_startControllerTransfer(_i2c, dev, DL_I2C_CONTROLLER_DIRECTION_TX, len);

  /* 剩余字节逐个填入 FIFO */
  for (size_t i = first; i < len; i++)
    _tx_byte(data[i]);

  _wait_done();
}

/* ================================================================
 *  read_reg() — 写寄存器地址 → 重启 → 读 1 字节
 * ================================================================ */

uint8_t BspI2c::read_reg(uint8_t dev, uint8_t reg)
{
  /* 写寄存器地址 */
  write(dev, &reg, 1U);

  /* 读 1 字节 */
  _wait_idle();
  DL_I2C_startControllerTransfer(_i2c, dev, DL_I2C_CONTROLLER_DIRECTION_RX, 1U);

  uint32_t timeout = I2C_TIMEOUT;
  while (DL_I2C_isControllerRXFIFOEmpty(_i2c))
  {
    if (--timeout == 0U) break;
  }

  uint8_t rx = DL_I2C_receiveControllerData(_i2c);

  _wait_done();
  return rx;
}


