/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G351X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G351X
#define CONFIG_MSPM0G3519

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define GPIO_HFXT_PORT                                                     GPIOA
#define GPIO_HFXIN_PIN                                             DL_GPIO_PIN_5
#define GPIO_HFXIN_IOMUX                                         (IOMUX_PINCM10)
#define GPIO_HFXOUT_PIN                                            DL_GPIO_PIN_6
#define GPIO_HFXOUT_IOMUX                                        (IOMUX_PINCM11)
#define CPUCLK_FREQ                                                     80000000
/* Defines for SYSPLL_ERR_01 Workaround */
/* Represent 1.000 as 1000 */
#define FLOAT_TO_INT_SCALE                                               (1000U)
#define FCC_EXPECTED_RATIO                                                  5000
#define FCC_UPPER_BOUND                       (FCC_EXPECTED_RATIO * (1 + 0.003))
#define FCC_LOWER_BOUND                       (FCC_EXPECTED_RATIO * (1 - 0.003))

bool SYSCFG_DL_SYSCTL_SYSPLL_init(void);


/* Defines for PWMA0 */
#define PWMA0_INST                                                         TIMA0
#define PWMA0_INST_IRQHandler                                   TIMA0_IRQHandler
#define PWMA0_INST_INT_IRQN                                     (TIMA0_INT_IRQn)
#define PWMA0_INST_CLK_FREQ                                             80000000
/* GPIO defines for channel 0 */
#define GPIO_PWMA0_C0_PORT                                                 GPIOA
#define GPIO_PWMA0_C0_PIN                                          DL_GPIO_PIN_0
#define GPIO_PWMA0_C0_IOMUX                                       (IOMUX_PINCM1)
#define GPIO_PWMA0_C0_IOMUX_FUNC                      IOMUX_PINCM1_PF_TIMA0_CCP0
#define GPIO_PWMA0_C0_IDX                                    DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWMA0_C1_PORT                                                 GPIOA
#define GPIO_PWMA0_C1_PIN                                          DL_GPIO_PIN_1
#define GPIO_PWMA0_C1_IOMUX                                       (IOMUX_PINCM2)
#define GPIO_PWMA0_C1_IOMUX_FUNC                      IOMUX_PINCM2_PF_TIMA0_CCP1
#define GPIO_PWMA0_C1_IDX                                    DL_TIMER_CC_1_INDEX
/* GPIO defines for channel 2 */
#define GPIO_PWMA0_C2_PORT                                                 GPIOA
#define GPIO_PWMA0_C2_PIN                                          DL_GPIO_PIN_7
#define GPIO_PWMA0_C2_IOMUX                                      (IOMUX_PINCM14)
#define GPIO_PWMA0_C2_IOMUX_FUNC                     IOMUX_PINCM14_PF_TIMA0_CCP2
#define GPIO_PWMA0_C2_IDX                                    DL_TIMER_CC_2_INDEX
/* GPIO defines for channel 3 */
#define GPIO_PWMA0_C3_PORT                                                 GPIOA
#define GPIO_PWMA0_C3_PIN                                         DL_GPIO_PIN_28
#define GPIO_PWMA0_C3_IOMUX                                       (IOMUX_PINCM3)
#define GPIO_PWMA0_C3_IOMUX_FUNC                      IOMUX_PINCM3_PF_TIMA0_CCP3
#define GPIO_PWMA0_C3_IDX                                    DL_TIMER_CC_3_INDEX




/* Defines for QEI0 */
#define QEI0_INST                                                          TIMG9
#define QEI0_INST_IRQHandler                                    TIMG9_IRQHandler
#define QEI0_INST_INT_IRQN                                      (TIMG9_INT_IRQn)
/* Pin configuration defines for QEI0 PHA Pin */
#define GPIO_QEI0_PHA_PORT                                                 GPIOA
#define GPIO_QEI0_PHA_PIN                                          DL_GPIO_PIN_3
#define GPIO_QEI0_PHA_IOMUX                                       (IOMUX_PINCM8)
#define GPIO_QEI0_PHA_IOMUX_FUNC                      IOMUX_PINCM8_PF_TIMG9_CCP0
/* Pin configuration defines for QEI0 PHB Pin */
#define GPIO_QEI0_PHB_PORT                                                 GPIOB
#define GPIO_QEI0_PHB_PIN                                          DL_GPIO_PIN_9
#define GPIO_QEI0_PHB_IOMUX                                      (IOMUX_PINCM26)
#define GPIO_QEI0_PHB_IOMUX_FUNC                     IOMUX_PINCM26_PF_TIMG9_CCP1

/* Defines for QEI1 */
#define QEI1_INST                                                          TIMG8
#define QEI1_INST_IRQHandler                                    TIMG8_IRQHandler
#define QEI1_INST_INT_IRQN                                      (TIMG8_INT_IRQn)
/* Pin configuration defines for QEI1 PHA Pin */
#define GPIO_QEI1_PHA_PORT                                                 GPIOB
#define GPIO_QEI1_PHA_PIN                                          DL_GPIO_PIN_6
#define GPIO_QEI1_PHA_IOMUX                                      (IOMUX_PINCM23)
#define GPIO_QEI1_PHA_IOMUX_FUNC                     IOMUX_PINCM23_PF_TIMG8_CCP0
/* Pin configuration defines for QEI1 PHB Pin */
#define GPIO_QEI1_PHB_PORT                                                 GPIOB
#define GPIO_QEI1_PHB_PIN                                          DL_GPIO_PIN_7
#define GPIO_QEI1_PHB_IOMUX                                      (IOMUX_PINCM24)
#define GPIO_QEI1_PHB_IOMUX_FUNC                     IOMUX_PINCM24_PF_TIMG8_CCP1



/* Defines for I2C1 */
#define I2C1_INST                                                           I2C1
#define I2C1_INST_IRQHandler                                     I2C1_IRQHandler
#define I2C1_INST_INT_IRQN                                         I2C1_INT_IRQn
#define I2C1_BUS_SPEED_HZ                                                 400000
#define GPIO_I2C1_SDA_PORT                                                 GPIOB
#define GPIO_I2C1_SDA_PIN                                          DL_GPIO_PIN_3
#define GPIO_I2C1_IOMUX_SDA                                      (IOMUX_PINCM16)
#define GPIO_I2C1_IOMUX_SDA_FUNC                       IOMUX_PINCM16_PF_I2C1_SDA
#define GPIO_I2C1_SCL_PORT                                                 GPIOB
#define GPIO_I2C1_SCL_PIN                                          DL_GPIO_PIN_2
#define GPIO_I2C1_IOMUX_SCL                                      (IOMUX_PINCM15)
#define GPIO_I2C1_IOMUX_SCL_FUNC                       IOMUX_PINCM15_PF_I2C1_SCL

/* Defines for I2C2 */
#define I2C2_INST                                                           I2C2
#define I2C2_INST_IRQHandler                                     I2C2_IRQHandler
#define I2C2_INST_INT_IRQN                                         I2C2_INT_IRQn
#define I2C2_BUS_SPEED_HZ                                                 400000
#define GPIO_I2C2_SDA_PORT                                                 GPIOA
#define GPIO_I2C2_SDA_PIN                                         DL_GPIO_PIN_24
#define GPIO_I2C2_IOMUX_SDA                                      (IOMUX_PINCM54)
#define GPIO_I2C2_IOMUX_SDA_FUNC                       IOMUX_PINCM54_PF_I2C2_SDA
#define GPIO_I2C2_SCL_PORT                                                 GPIOA
#define GPIO_I2C2_SCL_PIN                                         DL_GPIO_PIN_23
#define GPIO_I2C2_IOMUX_SCL                                      (IOMUX_PINCM53)
#define GPIO_I2C2_IOMUX_SCL_FUNC                       IOMUX_PINCM53_PF_I2C2_SCL


/* Defines for UART0 */
#define UART0_INST                                                         UART0
#define UART0_INST_FREQUENCY                                            40000000
#define UART0_INST_IRQHandler                                   UART0_IRQHandler
#define UART0_INST_INT_IRQN                                       UART0_INT_IRQn
#define GPIO_UART0_RX_PORT                                                 GPIOA
#define GPIO_UART0_TX_PORT                                                 GPIOA
#define GPIO_UART0_RX_PIN                                         DL_GPIO_PIN_11
#define GPIO_UART0_TX_PIN                                         DL_GPIO_PIN_10
#define GPIO_UART0_IOMUX_RX                                      (IOMUX_PINCM22)
#define GPIO_UART0_IOMUX_TX                                      (IOMUX_PINCM21)
#define GPIO_UART0_IOMUX_RX_FUNC                       IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART0_IOMUX_TX_FUNC                       IOMUX_PINCM21_PF_UART0_TX
#define UART0_BAUD_RATE                                                 (115200)
#define UART0_IBRD_40_MHZ_115200_BAUD                                       (21)
#define UART0_FBRD_40_MHZ_115200_BAUD                                       (45)
/* Defines for UART1 */
#define UART1_INST                                                         UART1
#define UART1_INST_FREQUENCY                                            40000000
#define UART1_INST_IRQHandler                                   UART1_IRQHandler
#define UART1_INST_INT_IRQN                                       UART1_INT_IRQn
#define GPIO_UART1_RX_PORT                                                 GPIOA
#define GPIO_UART1_TX_PORT                                                 GPIOA
#define GPIO_UART1_RX_PIN                                          DL_GPIO_PIN_9
#define GPIO_UART1_TX_PIN                                         DL_GPIO_PIN_17
#define GPIO_UART1_IOMUX_RX                                      (IOMUX_PINCM20)
#define GPIO_UART1_IOMUX_TX                                      (IOMUX_PINCM39)
#define GPIO_UART1_IOMUX_RX_FUNC                       IOMUX_PINCM20_PF_UART1_RX
#define GPIO_UART1_IOMUX_TX_FUNC                       IOMUX_PINCM39_PF_UART1_TX
#define UART1_BAUD_RATE                                                 (115200)
#define UART1_IBRD_40_MHZ_115200_BAUD                                       (21)
#define UART1_FBRD_40_MHZ_115200_BAUD                                       (45)
/* Defines for UART3 */
#define UART3_INST                                                         UART3
#define UART3_INST_FREQUENCY                                            80000000
#define UART3_INST_IRQHandler                                   UART3_IRQHandler
#define UART3_INST_INT_IRQN                                       UART3_INT_IRQn
#define GPIO_UART3_RX_PORT                                                 GPIOA
#define GPIO_UART3_TX_PORT                                                 GPIOA
#define GPIO_UART3_RX_PIN                                         DL_GPIO_PIN_27
#define GPIO_UART3_TX_PIN                                         DL_GPIO_PIN_14
#define GPIO_UART3_IOMUX_RX                                      (IOMUX_PINCM60)
#define GPIO_UART3_IOMUX_TX                                      (IOMUX_PINCM36)
#define GPIO_UART3_IOMUX_RX_FUNC                       IOMUX_PINCM60_PF_UART3_RX
#define GPIO_UART3_IOMUX_TX_FUNC                       IOMUX_PINCM36_PF_UART3_TX
#define UART3_BAUD_RATE                                                 (115200)
#define UART3_IBRD_80_MHZ_115200_BAUD                                       (43)
#define UART3_FBRD_80_MHZ_115200_BAUD                                       (26)
/* Defines for UART4 */
#define UART4_INST                                                         UART4
#define UART4_INST_FREQUENCY                                            80000000
#define UART4_INST_IRQHandler                                   UART4_IRQHandler
#define UART4_INST_INT_IRQN                                       UART4_INT_IRQn
#define GPIO_UART4_RX_PORT                                                 GPIOB
#define GPIO_UART4_TX_PORT                                                 GPIOB
#define GPIO_UART4_RX_PIN                                         DL_GPIO_PIN_18
#define GPIO_UART4_TX_PIN                                         DL_GPIO_PIN_17
#define GPIO_UART4_IOMUX_RX                                      (IOMUX_PINCM44)
#define GPIO_UART4_IOMUX_TX                                      (IOMUX_PINCM43)
#define GPIO_UART4_IOMUX_RX_FUNC                       IOMUX_PINCM44_PF_UART4_RX
#define GPIO_UART4_IOMUX_TX_FUNC                       IOMUX_PINCM43_PF_UART4_TX
#define UART4_BAUD_RATE                                                 (115200)
#define UART4_IBRD_80_MHZ_115200_BAUD                                       (43)
#define UART4_FBRD_80_MHZ_115200_BAUD                                       (26)





/* Defines for DMA_CH0 */
#define DMA_CH0_CHAN_ID                                                      (0)
#define UART0_INST_DMA_TRIGGER                               (DMA_UART0_TX_TRIG)
/* Defines for DMA_CH1 */
#define DMA_CH1_CHAN_ID                                                      (1)
#define UART1_INST_DMA_TRIGGER                               (DMA_UART1_TX_TRIG)
/* Defines for DMA_CH2 */
#define DMA_CH2_CHAN_ID                                                      (2)
#define UART3_INST_DMA_TRIGGER                               (DMA_UART3_TX_TRIG)
/* Defines for DMA_CH3 */
#define DMA_CH3_CHAN_ID                                                      (3)
#define UART4_INST_DMA_TRIGGER                               (DMA_UART4_TX_TRIG)


/* Port definition for Pin Group LED */
#define LED_PORT                                                         (GPIOA)

/* Defines for PIN_LED: GPIOA.4 with pinCMx 9 on package pin 10 */
#define LED_PIN_LED_PIN                                          (DL_GPIO_PIN_4)
#define LED_PIN_LED_IOMUX                                         (IOMUX_PINCM9)
/* Port definition for Pin Group BUZZER */
#define BUZZER_PORT                                                      (GPIOA)

/* Defines for PIN_BUZZER: GPIOA.2 with pinCMx 7 on package pin 8 */
#define BUZZER_PIN_BUZZER_PIN                                    (DL_GPIO_PIN_2)
#define BUZZER_PIN_BUZZER_IOMUX                                   (IOMUX_PINCM7)
/* Defines for PIN_KEY3: GPIOA.31 with pinCMx 6 on package pin 5 */
#define KEY_PIN_KEY3_PORT                                                (GPIOA)
#define KEY_PIN_KEY3_PIN                                        (DL_GPIO_PIN_31)
#define KEY_PIN_KEY3_IOMUX                                        (IOMUX_PINCM6)
/* Defines for PIN_KEY2: GPIOA.8 with pinCMx 19 on package pin 16 */
#define KEY_PIN_KEY2_PORT                                                (GPIOA)
#define KEY_PIN_KEY2_PIN                                         (DL_GPIO_PIN_8)
#define KEY_PIN_KEY2_IOMUX                                       (IOMUX_PINCM19)
/* Defines for PIN_KEY1: GPIOB.8 with pinCMx 25 on package pin 22 */
#define KEY_PIN_KEY1_PORT                                                (GPIOB)
#define KEY_PIN_KEY1_PIN                                         (DL_GPIO_PIN_8)
#define KEY_PIN_KEY1_IOMUX                                       (IOMUX_PINCM25)
/* Port definition for Pin Group MEN */
#define MEN_PORT                                                         (GPIOA)

/* Defines for PIN_MEN1: GPIOA.15 with pinCMx 37 on package pin 30 */
#define MEN_PIN_MEN1_PIN                                        (DL_GPIO_PIN_15)
#define MEN_PIN_MEN1_IOMUX                                       (IOMUX_PINCM37)
/* Defines for PIN_MEN2: GPIOA.16 with pinCMx 38 on package pin 31 */
#define MEN_PIN_MEN2_PIN                                        (DL_GPIO_PIN_16)
#define MEN_PIN_MEN2_IOMUX                                       (IOMUX_PINCM38)


/* Defines for MCAN0 */
#define MCAN0_INST                                                        CANFD0
#define GPIO_MCAN0_CAN_TX_PORT                                             GPIOA
#define GPIO_MCAN0_CAN_TX_PIN                                     DL_GPIO_PIN_12
#define GPIO_MCAN0_IOMUX_CAN_TX                                  (IOMUX_PINCM34)
#define GPIO_MCAN0_IOMUX_CAN_TX_FUNC               IOMUX_PINCM34_PF_CANFD0_CANTX
#define GPIO_MCAN0_CAN_RX_PORT                                             GPIOA
#define GPIO_MCAN0_CAN_RX_PIN                                     DL_GPIO_PIN_13
#define GPIO_MCAN0_IOMUX_CAN_RX                                  (IOMUX_PINCM35)
#define GPIO_MCAN0_IOMUX_CAN_RX_FUNC               IOMUX_PINCM35_PF_CANFD0_CANRX
#define MCAN0_INST_IRQHandler                                 CANFD0_IRQHandler
#define MCAN0_INST_INT_IRQN                                     CANFD0_INT_IRQn


/* Defines for MCAN0 MCAN RAM configuration */
#define MCAN0_INST_MCAN_STD_ID_FILT_START_ADDR     (0)
#define MCAN0_INST_MCAN_STD_ID_FILTER_NUM          (1)
#define MCAN0_INST_MCAN_EXT_ID_FILT_START_ADDR     (48)
#define MCAN0_INST_MCAN_EXT_ID_FILTER_NUM          (1)
#define MCAN0_INST_MCAN_TX_BUFF_START_ADDR         (148)
#define MCAN0_INST_MCAN_TX_BUFF_SIZE               (2)
#define MCAN0_INST_MCAN_FIFO_1_START_ADDR          (252)
#define MCAN0_INST_MCAN_FIFO_1_NUM                 (2)
#define MCAN0_INST_MCAN_TX_EVENT_START_ADDR        (180)
#define MCAN0_INST_MCAN_TX_EVENT_SIZE              (2)
#define MCAN0_INST_MCAN_EXT_ID_AND_MASK            (0x1FFFFFFFU)
#define MCAN0_INST_MCAN_RX_BUFF_START_ADDR         (284)
#define MCAN0_INST_MCAN_FIFO_0_START_ADDR          (180)
#define MCAN0_INST_MCAN_FIFO_0_NUM                 (4)

#define MCAN0_INST_MCAN_INTERRUPTS (DL_MCAN_INTERRUPT_RF0N | \
						DL_MCAN_INTERRUPT_TEFN)




/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);

bool SYSCFG_DL_SYSCTL_SYSPLL_init(void);
void SYSCFG_DL_PWMA0_init(void);
void SYSCFG_DL_QEI0_init(void);
void SYSCFG_DL_QEI1_init(void);
void SYSCFG_DL_I2C1_init(void);
void SYSCFG_DL_I2C2_init(void);
void SYSCFG_DL_UART0_init(void);
void SYSCFG_DL_UART1_init(void);
void SYSCFG_DL_UART3_init(void);
void SYSCFG_DL_UART4_init(void);
void SYSCFG_DL_DMA_init(void);

void SYSCFG_DL_MCAN0_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
