/**
 * @file iic_track.cpp
 * @brief I2C 巡线模块设备驱动 — 实现
 *
 * @note 所有逻辑保持与原 IIC_READ_REG_8BIT_8BIT + PCA9555_RX 一致，
 *       仅重构为 C++ 类形式，移除调试死循环 while(1)。
 */

#include "iic_track.hpp"

/* ==================== 构造 ==================== */
IicTrack::IicTrack() {}

/* ==================== 初始化 ==================== */
bool IicTrack::init(BspI2c *i2c, uint8_t dev_addr)
{
  if (!i2c)
    return false;

  _i2c      = i2c;
  _dev_addr = dev_addr;
  return true;
}

/* ==================== 数据读取 ==================== */

uint16_t IicTrack::read()
{
  /* 读端口 0（低 8 位）和端口 1（高 8 位），合并为 16 位 */
  uint8_t  lo   = _i2c->read_reg(_dev_addr, 0x00);
  uint8_t  hi   = _i2c->read_reg(_dev_addr, 0x01);
  uint16_t data = ((uint16_t)hi << 8U) | (uint16_t)lo;
  data &= 0x0FFF;
  return data;
}
