/**
 * @file bsp_cfg.hpp
 * @brief BSP 层总配置 — 全局实例声明
 *
 * @note 所有 BSP 驱动对象的全局指针在此声明。
 *       初始化由 bsp_init() 在 bsp_cfg.cpp 中统一完成。
 */

#ifndef __BSP_CFG_HPP__
#define __BSP_CFG_HPP__

#include "bsp_uart.hpp"
#include "bsp_pwm.hpp"
#include "bsp_qei.hpp"
#include "bsp_can.hpp"
#include "bsp_gpio.hpp"
#include "bsp_key.hpp"

/* ==================== 函数声明 ==================== */

/**
 * @brief BSP 初始化 — 绑定所有驱动对象到硬件实例
 *
 * @note 必须在 SYSCFG_DL_init() 之后、FreeRTOS 调度器启动之前调用。
 *       硬件寄存器配置由 SysConfig 完成，本函数仅绑定软件对象。
 */
void bsp_init();

/* ==================== 全局声明 ==================== */

extern BspUart<64> bsp_uart0;  ///< UART0 (PA10/PA11), 备用串口
extern BspUart<64> bsp_uart1;  ///< UART1 (PA17/PA9),   陀螺仪通信
extern BspUart<64> bsp_uart4;  ///< UART4 (PB17/PB18), 通用串口 (硬件实例为 UART4)
extern BspUart<64> bsp_uart3;  ///< UART3 (PA14/PA27), 调试输出 (115200 8N1)
extern BspPwm      bsp_pwm_a0; ///< PWMA0_1 (TIMA0), 80MHz, period=2000, CH0(PA0)+CH1(PA1)+CH2(PA7)+CH3(PA28)
extern BspQei      bsp_qei0;   ///< QEI0 (TIMG9), 正交编码器 PA3/PB9
extern BspQei      bsp_qei1;   ///< QEI1 (TIMG8), 正交编码器 PB6/PB7
extern BspCan      bsp_can;    ///< MCAN0 (PA12/PA13), 1Mbps, 标准帧

extern BspGpio gpio_led;  ///< LED   (PA4)
extern BspGpio gpio_buzz; ///< 蜂鸣器 (PA2)
extern BspGpio gpio_men1; ///< MEN1   (PA15)
extern BspGpio gpio_men2; ///< MEN2   (PA16)
extern BspKey  key_1;     ///< 按键 1 (PB8)
extern BspKey  key_2;     ///< 按键 2 (PA8)
extern BspKey  key_3;     ///< 按键 3 (PA31)

#endif /* __BSP_CFG_HPP__ */
