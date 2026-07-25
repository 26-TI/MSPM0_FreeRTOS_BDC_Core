/**
 * @file bsp_gpio.cpp
 * @brief MSPM0 BspGpio 实现 — 单引脚 GPIO 输出
 */

#include "bsp_gpio.hpp"

void BspGpio::init(const Config &cfg)
{
  _port = cfg.port;
  _pin  = cfg.pin;
}

void BspGpio::set()
{
  DL_GPIO_setPins(_port, _pin);
}

void BspGpio::reset()
{
  DL_GPIO_clearPins(_port, _pin);
}

void BspGpio::toggle()
{
  DL_GPIO_togglePins(_port, _pin);
}

void BspGpio::write(bool v)
{
  if (v)
  {
    set();
  }
  else
  {
    reset();
  }
}

bool BspGpio::read() const
{
  return (DL_GPIO_readPins(_port, _pin) != 0U);
}
