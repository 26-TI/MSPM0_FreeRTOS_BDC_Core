/**
 * @file bsp_i2c.hpp
 * @brief MSPM0 BspI2c — I2C 控制器模式封装（裸机阻塞）
 *
 * @note 通用 API:
 *       - write():     发送任意长度数据（自动处理 FIFO 填充）
 *       - read_reg():  写寄存器地址 → 重启 → 读 1 字节
 *
 * 使用示例:
 * @code
 *   BspI2c i2c0;
 *   i2c0.init({I2C0_INST});
 *   uint8_t cmd[] = {0x00, 0xAF};
 *   i2c0.write(0x3C, cmd, 2);
 *   uint8_t val = i2c0.read_reg(0x20, 0x00);
 * @endcode
 */

#ifndef __BSP_I2C_MSPM0_HPP__
#define __BSP_I2C_MSPM0_HPP__

#include "ti_msp_dl_config.h" // IWYU pragma: keep

class BspI2c
{
public:
  struct Config
  {
    I2C_Regs *i2c;
  };

  BspI2c() = default;

  /** @brief 绑定 I2C 硬件实例 */
  bool init(const Config &cfg);

  /** @brief 获取绑定的 I2C 外设指针 */
  I2C_Regs *hw() const { return _i2c; }

  /* ── 写操作 ── */

  /**
   * @brief 阻塞写入（任意长度，自动处理 FIFO 填充）
   * @param dev  7 位设备地址
   * @param data 数据缓冲区
   * @param len  字节数
   */
  void write(uint8_t dev, const uint8_t *data, size_t len);

  /* ── 读操作 ── */

  /** @brief 读 1 字节寄存器 */
  uint8_t read_reg(uint8_t dev, uint8_t reg);

private:
  I2C_Regs *_i2c = nullptr;

  /** @brief 等待 IDLE 标志 */
  void _wait_idle();

  /** @brief 等待 BUSY 清零 + IDLE */
  void _wait_done();

  /** @brief 向 FIFO 填充 1 字节（等 FIFO 非满） */
  void _tx_byte(uint8_t b);
};

#endif /* __BSP_I2C_MSPM0_HPP__ */
