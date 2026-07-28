/**
 * @file iic_track.cpp
 * @brief I2C 巡线模块设备驱动 — 实现
 *
 * @note 所有逻辑保持与原 IIC_READ_REG_8BIT_8BIT + PCA9555_RX 一致，
 *       仅重构为 C++ 类形式，移除调试死循环 while(1)。
 */

#include "iic_track.hpp"

// ---- 构造 ----

IicTrack::IicTrack() {}

// ---- init() — 绑定 I2C 总线 ----

/**
 * @brief 绑定 I2C 总线驱动
 * @param i2c       BspI2c 实例指针
 * @param dev_addr  PCA9555 设备地址 (0x20)
 * @return true=成功
 */
bool IicTrack::init(BspI2c *i2c, uint8_t dev_addr)
{
  if (!i2c)
    return false;

  _i2c      = i2c;
  _dev_addr = dev_addr;
  return true;
}

// ---- read() — 读取 12 路传感器 ----

/**
 * @brief 读取 PCA9555 端口 0 (低 8 位) 和端口 1 (高 8 位), 合并为 12 位数据
 * @return 12 位传感器值 (bit 0→D0 ... bit 11→D11, 高位补零)
 */

uint16_t IicTrack::read()
{
  /* 读端口 0（低 8 位）和端口 1（高 8 位），合并为 16 位 */
  uint8_t  lo   = _i2c->read_reg(_dev_addr, 0x00);
  uint8_t  hi   = _i2c->read_reg(_dev_addr, 0x01);
  uint16_t data = ((uint16_t)hi << 8U) | (uint16_t)lo;
  data &= 0x0FFF;
  return data;
}
