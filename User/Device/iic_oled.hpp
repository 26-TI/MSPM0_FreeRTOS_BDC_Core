/**
 * @file iic_oled.hpp
 * @brief I2C OLED 设备驱动 — SSD1306 兼容 128x64 单色屏
 *
 * @note 基于 BspI2c 封装，将原 OLED.c 的 C 风格函数封装为 C++ 类。
 *       每个 IicOled 实例独占一个 BspI2c 实例，独立 I2C 总线。
 *
 * @note 显示缓冲 OLED_GRAM[144][8] 采用列寻址，每列 8 个 page。
 *       实际有效显示区域为 128×64 像素。
 *
 * 使用示例:
 * @code
 *   IicOled oled;
 *   oled.init(&bsp_i2c0, 0x3C);
 *   oled.clear();
 *   oled.showString(0, 0, "Hello");
 *   oled.refresh();
 * @endcode
 */

#ifndef __IIC_OLED_HPP__
#define __IIC_OLED_HPP__

#include "bsp_i2c.hpp"
#include <stdint.h>

class IicOled
{
public:
  IicOled();

  /**
   * @brief 初始化 OLED 并绑定 I2C 实例
   * @param i2c    指向已初始化的 BspI2c 实例
   * @param dev_addr 7 位 I2C 设备地址（通常为 0x3C）
   * @return true  初始化成功
   * @return false i2c 为空
   */
  bool init(BspI2c *i2c, uint8_t dev_addr = 0x3C);

  /* ==================== 显示控制 ==================== */

  /** @brief 开启 OLED 显示 */
  void display_on();

  /** @brief 关闭 OLED 显示 */
  void display_off();

  /**
   * @brief 反显切换
   * @param enable true=反色显示, false=正常显示
   */
  void color_turn(bool enable);

  /**
   * @brief 屏幕旋转 180 度
   * @param enable true=旋转, false=正常
   */
  void display_turn(bool enable);

  /* ==================== 显存操作 ==================== */

  /** @brief 清空显存并刷新 */
  void clear();

  /**
   * @brief 将显存刷新到屏幕
   * @note 逐页（8 页）发送 128 列像素数据
   */
  void refresh();

  /**
   * @brief 在指定位置显示字符
   * @param x   列位置（0~15，每个字符占 8 像素宽）
   * @param y   行位置（0~3，每行占 16 像素高 = 2 page）
   * @param chr ASCII 字符
   */
  void show_char(uint8_t x, uint8_t y, uint8_t chr);

  /**
   * @brief 在指定位置显示字符串
   * @param x   起始列位置（0~15）
   * @param y   起始行位置（0~3）
   * @param str C 风格字符串
   */
  void show_string(uint8_t x, uint8_t y, const char *str);

  /**
   * @brief 直接写入显存（用于自定义绘图）
   * @param col   列（0~127）
   * @param page  页（0~7）
   * @param data  8 位像素数据（bit0=顶部行）
   */
  void set_pixel(uint8_t col, uint8_t page, uint8_t data)
  {
    _gram[col][page] = data;
  }

  /**
   * @brief 获取显存指针（用于批量操作）
   * @return 144×8 显存数组指针
   */
  uint8_t (*get_gram())[8] { return _gram; }

private:
  /**
   * @brief 发送 OLED 命令字节
   * @param cmd 命令码
   * @note 命令格式: {0x00, cmd}，0x00 为 Co=0, D/C#=0（命令模式）
   */
  void write_cmd(uint8_t cmd);

  BspI2c *_i2c      = nullptr; ///< I2C 实例指针
  uint8_t _dev_addr = 0x3C;    ///< 设备地址
  uint8_t _gram[144][8]{};     ///< 显存缓冲（144 列 × 8 页，实际只用 128 列）
};

#endif // __IIC_OLED_HPP__
