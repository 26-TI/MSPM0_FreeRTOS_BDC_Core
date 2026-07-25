/**
 * @file iic_oled.cpp
 * @brief I2C OLED 设备驱动 — 实现
 *
 * @note 所有逻辑保持与原 OLED.c 一致，仅重构为 C++ 类形式。
 */

#include "iic_oled.hpp"
#include "fort.h"

/* ==================== 构造 ==================== */
IicOled::IicOled() {}

/* ==================== 初始化 ==================== */
bool IicOled::init(BspI2c *i2c, uint8_t dev_addr)
{
  if (!i2c)
    return false;

  _i2c      = i2c;
  _dev_addr = dev_addr;

  /* SSD1306 初始化序列（与原 OLED_Init 一致） */
  write_cmd(0xAE); // 关闭显示
  write_cmd(0x00); // 列低地址
  write_cmd(0x10); // 列高地址
  write_cmd(0x40); // 起始行
  write_cmd(0x81); // 对比度设置
  write_cmd(0xCF); // 对比度值
  write_cmd(0xA1); // 段重映射（左右翻转）
  write_cmd(0xC8); // COM 扫描方向（上下翻转）
  write_cmd(0xA6); // 正常显示（非反色）
  write_cmd(0xA8); // 多路复用比
  write_cmd(0x3F); // 1/64 占空比
  write_cmd(0xD3); // 显示偏移
  write_cmd(0x00); // 偏移 0
  write_cmd(0xD5); // 显示时钟分频/振荡频率
  write_cmd(0x80); // 默认值
  write_cmd(0xD9); // 预充电周期
  write_cmd(0xF1); // 值
  write_cmd(0xDA); // COM 引脚配置
  write_cmd(0x12); // 交替模式
  write_cmd(0xDB); // VCOMH 电平
  write_cmd(0x30); // 值
  write_cmd(0x20); // 寻址模式
  write_cmd(0x02); // 页寻址模式
  write_cmd(0x8D); // 电荷泵
  write_cmd(0x14); // 使能
  clear();
  write_cmd(0xAF); // 开启显示

  return true;
}

/* ==================== 命令发送 ==================== */
void IicOled::write_cmd(uint8_t cmd)
{
  uint8_t data[2] = {0x00, cmd};
  _i2c->write(_dev_addr, data, 2);
}

/* ==================== 显示控制 ==================== */

void IicOled::display_on()
{
  write_cmd(0x8D); // 电荷泵使能
  write_cmd(0x14); // 开启电荷泵
  write_cmd(0xAF); // 点亮屏幕
}

void IicOled::display_off()
{
  write_cmd(0x8D); // 电荷泵使能
  write_cmd(0x10); // 关闭电荷泵
  write_cmd(0xAE); // 关闭屏幕
}

void IicOled::color_turn(bool enable)
{
  if (enable)
    write_cmd(0xA7); // 反色显示
  else
    write_cmd(0xA6); // 正常显示
}

void IicOled::display_turn(bool enable)
{
  if (enable)
  {
    write_cmd(0xC0); // COM 反转
    write_cmd(0xA0); // 段反转
  }
  else
  {
    write_cmd(0xC8); // COM 正常
    write_cmd(0xA1); // 段正常
  }
}

/* ==================== 显存操作 ==================== */

void IicOled::clear()
{
  for (uint8_t i = 0; i < 8; i++)
  {
    for (uint8_t n = 0; n < 128; n++)
    {
      _gram[n][i] = 0x00;
    }
  }
  refresh();
}

void IicOled::refresh()
{
  uint8_t page, col;

  for (page = 0; page < 8; page++)
  {
    /* 设置页地址和列起始地址 */
    write_cmd(0xB0 + page);
    write_cmd(0x00);
    write_cmd(0x10);

    /* 启动批量传输: 1 控制字节 + 128 像素数据 */
    _i2c->start_tx(_dev_addr, 129);

    _i2c->tx_byte(0x40); // 控制字节: 数据模式

    /* 连续发送 128 个像素数据 */
    for (col = 0; col < 128; col++)
    {
      _i2c->tx_byte(_gram[col][page]);
    }

    /* 等待该页传输完成 */
    _i2c->wait_tx_done();
  }
}

void IicOled::show_char(uint8_t x, uint8_t y, uint8_t chr)
{
  for (uint8_t i = 0; i <= 8; i++)
  {
    _gram[x * 8 + i][y * 2]     = font_8x16[chr - ' '][i];
    _gram[x * 8 + i][y * 2 + 1] = font_8x16[chr - ' '][i + 8];
  }
}

void IicOled::show_string(uint8_t x, uint8_t y, const char *str)
{
  uint8_t j = 0;
  while (str[j] != '\0')
  {
    show_char(x, y, str[j]);
    x++;
    if (x > 15)
      return;
    j++;
  }
}
