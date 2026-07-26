/**
 * @file bsp_cfg.cpp
 * @brief BSP 层总配置 — 全局实例定义 + 统一初始化
 *
 * @note 硬件寄存器配置由 SysConfig 生成的 SYSCFG_DL_xxx_init() 完成。
 *       本文件中的 bsp_init() 仅负责"软件绑定"——
 *       将外设实例（如 UART0_INST）与 BSP 驱动对象（如 bsp_uart0）关联。
 *
 * @note SysConfig 中未配置的外设（SPI0, I2C0, TIMG0_PWM）已移除。
 *       引脚分配以 SysConfig/ti_msp_dl_config.h 为准。
 */

#include "bsp_cfg.hpp"
#include "ti_msp_dl_config.h"

/* ==================== 全局实例 ==================== */

BspUart<64> bsp_uart0;
BspUart<64> bsp_uart1;
BspUart<64> bsp_uart4;
BspUart<64> bsp_uart3;
BspPwm      bsp_pwm_a0;
BspQei      bsp_qei0;
BspQei      bsp_qei1;
BspCan      bsp_can;

BspGpio gpio_led;
BspGpio gpio_buzz;
BspGpio gpio_men1;
BspGpio gpio_men2;
BspKey  key_1;
BspKey  key_2;
BspKey  key_3;

/* ==================== BSP 初始化 ==================== */

/**
 * @brief BSP 层统一初始化 — 绑定所有驱动对象到硬件实例
 *
 * 调用时机: SYSCFG_DL_init() 之后，vTaskStartScheduler() 之前。
 *
 * 每行绑定一个外设:
 *   - 参数从 SysConfig 自动生成的宏获取（如 UART0_INST, UART0_INST_INT_IRQN）
 *   - init() 仅做软件校验和引用保存，不操作硬件寄存器
 */
void bsp_init()
{
  /* ---- UART — 115200 8N1, DMA TX, FIFO RX ---- */
  bsp_uart0.init({UART0_INST, UART0_INST_INT_IRQN, DMA_CH0_CHAN_ID, UART0_INST_DMA_TRIGGER});

  bsp_uart1.init({UART1_INST, UART1_INST_INT_IRQN, DMA_CH1_CHAN_ID, UART1_INST_DMA_TRIGGER});

  bsp_uart4.init({UART4_INST, /* 硬件实例为 UART4 (PB17/PB18) */
                  UART4_INST_INT_IRQN,
                  DMA_CH3_CHAN_ID,
                  UART4_INST_DMA_TRIGGER});

  bsp_uart3.init({UART3_INST, UART3_INST_INT_IRQN, DMA_CH2_CHAN_ID, UART3_INST_DMA_TRIGGER});

  /* ---- PWM ---- */
  /* PWMA0_1 — TIMA0, 80MHz, period=5000, 4 通道: PA0/PA1/PA7/PA28 */
  bsp_pwm_a0.init({PWMA0_INST,
                   PWMA0_INST_CLK_FREQ,
                   5000,
                   4,
                   {GPIO_PWMA0_C0_IDX, GPIO_PWMA0_C1_IDX, GPIO_PWMA0_C2_IDX, GPIO_PWMA0_C3_IDX}});

  /* ---- QEI — 正交编码器 ---- */
  bsp_qei0.init({QEI0_INST}); /* QEI0: TIMG9, PA3/PB9 */
  bsp_qei1.init({QEI1_INST}); /* QEI1: TIMG8, PB6/PB7 */

  /* ---- MCAN0 — 1Mbps, 正常模式, PA12(TX)/PA13(RX) ---- */
  bsp_can.init(BspCan::NORMAL);

  /* ---- GPIO 输出 ---- */
  gpio_led.init({GPIOA, LED_PIN_LED_PIN});        /* PA4  */
  gpio_buzz.init({GPIOA, BUZZER_PIN_BUZZER_PIN}); /* PA2  */
  gpio_men1.init({GPIOA, MEN_PIN_MEN1_PIN});      /* PA15 */
  gpio_men2.init({GPIOA, MEN_PIN_MEN2_PIN});      /* PA16 */

  /* ---- 按键（消抖，适配 50ms 轮询: 2×50ms=100ms 消抖, 20×50ms=1s 长按） ----
   */
  key_1.init({GPIOB, KEY_PIN_KEY1_PIN, true, 2U, 20U}); /* PB8  */
  key_2.init({GPIOA, KEY_PIN_KEY2_PIN, true, 2U, 20U}); /* PA8  */
  key_3.init({GPIOA, KEY_PIN_KEY3_PIN, true, 2U, 20U}); /* PA31 */
}
