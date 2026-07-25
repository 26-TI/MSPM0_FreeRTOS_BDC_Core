/**
 * @file bsp_gpio.hpp
 * @brief MSPM0 BspGpio — 单引脚 GPIO 输出控制
 *
 * @note 引脚由 SysConfig 生成的 SYSCFG_DL_GPIO_init() 统一初始化
 *       (Digital Output / Push-Pull)。本类仅封装运行时 set/reset/toggle。
 *
 * @note 裸机操作（直接写寄存器），无阻塞、无中断。
 *
 * 使用示例:
 * @code
 *   BspGpio led;
 *   led.init({GPIOA, DL_GPIO_PIN_17});
 *   led.set();
 *   led.toggle();
 * @endcode
 */

#ifndef __BSP_GPIO_MSPM0_HPP__
#define __BSP_GPIO_MSPM0_HPP__

#include "ti_msp_dl_config.h" // IWYU pragma: keep

class BspGpio
{
public:
  /** GPIO 引脚配置 */
  struct Config
  {
    GPIO_Regs *port; ///< GPIO 端口 (GPIOA / GPIOB)
    uint32_t   pin;  ///< 引脚掩码 (DL_GPIO_PIN_x)
  };

  BspGpio() = default;

  /**
   * @brief 绑定引脚（硬件已由 SysConfig 初始化）
   * @param cfg 端口 + 引脚
   */
  void init(const Config &cfg);

  /** @brief 拉高 */
  void set();

  /** @brief 拉低 */
  void reset();

  /** @brief 翻转 */
  void toggle();

  /** @brief 写电平 @param v true=高, false=低 */
  void write(bool v);

  /** @brief 读当前电平 @return true=高, false=低 */
  bool read() const;

private:
  GPIO_Regs *_port = nullptr;
  uint32_t   _pin  = 0U;
};

#endif /* __BSP_GPIO_MSPM0_HPP__ */
