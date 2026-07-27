/**
 * @file iic_track.hpp
 * @brief I2C 巡线模块设备驱动 — PCA9555 16 位 I/O 扩展器
 *
 * @note PCA9555 通过 I2C 读取 16 路数字输入，用于巡线传感器数据采集。
 *       寄存器 0x00 = 端口 0（低 8 位），寄存器 0x01 = 端口 1（高 8 位），
 *       读取后取低 12 位有效数据（& 0x0FFF）。
 *
 * @note 每个 IicTrack 实例独占一个 BspI2c 实例，独立 I2C 总线。
 *
 * 使用示例:
 * @code
 *   IicTrack track;
 *   track.init(&bsp_i2c1, 0x20);
 *   uint16_t data = track.read();
 * @endcode
 */

#ifndef __IIC_TRACK_HPP__
#define __IIC_TRACK_HPP__

#include "bsp_i2c.hpp"
#include <stdint.h>

class IicTrack
{
public:
  IicTrack();

  /**
   * @brief 初始化巡线模块并绑定 I2C 实例
   * @param i2c      指向已初始化的 BspI2c 实例
   * @param dev_addr 7 位 I2C 设备地址（通常为 0x20）
   * @return true  初始化成功
   * @return false i2c 为空
   */
  bool init(BspI2c *i2c, uint8_t dev_addr = 0x20);

  /**
   * @brief 读取 PCA9555 全部 16 位端口状态
   * @return 16 位端口值（低 12 位有效，& 0x0FFF）
   * @note 先读端口 0（低字节），再读端口 1（高字节），
   *       合并为 16 位: (PORT1 << 8) | PORT0
   */
  uint16_t read();

private:
  BspI2c *_i2c      = nullptr; ///< I2C 实例指针
  uint8_t _dev_addr = 0x20;    ///< PCA9555 设备地址
};

#endif // __IIC_TRACK_HPP__
