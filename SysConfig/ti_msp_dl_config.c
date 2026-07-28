/*
 * Copyright (c) 2023, Texas Instruments Incorporated
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
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G351X
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

DL_TimerA_backupConfig gPWMA0Backup;
DL_TimerG_backupConfig gQEI1Backup;
DL_UART_Main_backupConfig gUART3Backup;
DL_MCAN_backupConfig gMCAN0Backup;

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_PWMA0_init();
    SYSCFG_DL_QEI0_init();
    SYSCFG_DL_QEI1_init();
    SYSCFG_DL_I2C1_init();
    SYSCFG_DL_I2C2_init();
    SYSCFG_DL_UART0_init();
    SYSCFG_DL_UART1_init();
    SYSCFG_DL_UART3_init();
    SYSCFG_DL_UART4_init();
    SYSCFG_DL_DMA_init();
    SYSCFG_DL_MCAN0_init();
    /* Ensure backup structures have no valid state */
	gPWMA0Backup.backupRdy 	= false;
	gQEI1Backup.backupRdy 	= false;
	gUART3Backup.backupRdy 	= false;
	gMCAN0Backup.backupRdy 	= false;

}
/*
 * User should take care to save and restore register configuration in application.
 * See Retention Configuration section for more details.
 */
SYSCONFIG_WEAK bool SYSCFG_DL_saveConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_saveConfiguration(PWMA0_INST, &gPWMA0Backup);
	retStatus &= DL_TimerG_saveConfiguration(QEI1_INST, &gQEI1Backup);
	retStatus &= DL_UART_Main_saveConfiguration(UART3_INST, &gUART3Backup);
	retStatus &= DL_MCAN_saveConfiguration(MCAN0_INST, &gMCAN0Backup);

    return retStatus;
}


SYSCONFIG_WEAK bool SYSCFG_DL_restoreConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_restoreConfiguration(PWMA0_INST, &gPWMA0Backup, false);
	retStatus &= DL_TimerG_restoreConfiguration(QEI1_INST, &gQEI1Backup, false);
	retStatus &= DL_UART_Main_restoreConfiguration(UART3_INST, &gUART3Backup);
	retStatus &= DL_MCAN_restoreConfiguration(MCAN0_INST, &gMCAN0Backup);

    return retStatus;
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_TimerA_reset(PWMA0_INST);
    DL_TimerG_reset(QEI0_INST);
    DL_TimerG_reset(QEI1_INST);
    DL_I2C_reset(I2C1_INST);
    DL_I2C_reset(I2C2_INST);
    DL_UART_Main_reset(UART0_INST);
    DL_UART_Main_reset(UART1_INST);
    DL_UART_Main_reset(UART3_INST);
    DL_UART_Main_reset(UART4_INST);

    DL_MCAN_reset(MCAN0_INST);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_TimerA_enablePower(PWMA0_INST);
    DL_TimerG_enablePower(QEI0_INST);
    DL_TimerG_enablePower(QEI1_INST);
    DL_I2C_enablePower(I2C1_INST);
    DL_I2C_enablePower(I2C2_INST);
    DL_UART_Main_enablePower(UART0_INST);
    DL_UART_Main_enablePower(UART1_INST);
    DL_UART_Main_enablePower(UART3_INST);
    DL_UART_Main_enablePower(UART4_INST);

    DL_MCAN_enablePower(MCAN0_INST);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

    DL_GPIO_initPeripheralAnalogFunction(GPIO_HFXIN_IOMUX);
    DL_GPIO_initPeripheralAnalogFunction(GPIO_HFXOUT_IOMUX);

    DL_GPIO_initPeripheralOutputFunction(GPIO_PWMA0_C0_IOMUX,GPIO_PWMA0_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWMA0_C0_PORT, GPIO_PWMA0_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_PWMA0_C1_IOMUX,GPIO_PWMA0_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWMA0_C1_PORT, GPIO_PWMA0_C1_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_PWMA0_C2_IOMUX,GPIO_PWMA0_C2_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWMA0_C2_PORT, GPIO_PWMA0_C2_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_PWMA0_C3_IOMUX,GPIO_PWMA0_C3_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWMA0_C3_PORT, GPIO_PWMA0_C3_PIN);

    DL_GPIO_initPeripheralInputFunction(GPIO_QEI0_PHA_IOMUX,GPIO_QEI0_PHA_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_QEI0_PHB_IOMUX,GPIO_QEI0_PHB_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_QEI1_PHA_IOMUX,GPIO_QEI1_PHA_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_QEI1_PHB_IOMUX,GPIO_QEI1_PHB_IOMUX_FUNC);

    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C1_IOMUX_SDA,
        GPIO_I2C1_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C1_IOMUX_SCL,
        GPIO_I2C1_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C1_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_I2C1_IOMUX_SCL);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C2_IOMUX_SDA,
        GPIO_I2C2_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C2_IOMUX_SCL,
        GPIO_I2C2_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C2_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_I2C2_IOMUX_SCL);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART0_IOMUX_TX, GPIO_UART0_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART0_IOMUX_RX, GPIO_UART0_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART1_IOMUX_TX, GPIO_UART1_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART1_IOMUX_RX, GPIO_UART1_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART3_IOMUX_TX, GPIO_UART3_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART3_IOMUX_RX, GPIO_UART3_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART4_IOMUX_TX, GPIO_UART4_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART4_IOMUX_RX, GPIO_UART4_IOMUX_RX_FUNC);

    DL_GPIO_initDigitalOutput(LED_PIN_LED_IOMUX);

    DL_GPIO_initDigitalOutput(BUZZER_PIN_BUZZER_IOMUX);

    DL_GPIO_initDigitalInputFeatures(KEY_PIN_KEY3_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(KEY_PIN_KEY2_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(KEY_PIN_KEY1_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(MEN_PIN_MEN1_IOMUX);

    DL_GPIO_initDigitalOutput(MEN_PIN_MEN2_IOMUX);

    DL_GPIO_clearPins(GPIOA, LED_PIN_LED_PIN |
		BUZZER_PIN_BUZZER_PIN |
		MEN_PIN_MEN1_PIN |
		MEN_PIN_MEN2_PIN);
    DL_GPIO_enableOutput(GPIOA, LED_PIN_LED_PIN |
		BUZZER_PIN_BUZZER_PIN |
		MEN_PIN_MEN1_PIN |
		MEN_PIN_MEN2_PIN);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_MCAN0_IOMUX_CAN_TX, GPIO_MCAN0_IOMUX_CAN_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_MCAN0_IOMUX_CAN_RX, GPIO_MCAN0_IOMUX_CAN_RX_FUNC);

}


static const DL_SYSCTL_SYSPLLConfig gSYSPLLConfig = {
    .inputFreq              = DL_SYSCTL_SYSPLL_INPUT_FREQ_16_32_MHZ,
	.rDivClk2x              = 3,
	.rDivClk1               = 0,
	.rDivClk0               = 0,
	.enableCLK2x            = DL_SYSCTL_SYSPLL_CLK2X_ENABLE,
	.enableCLK1             = DL_SYSCTL_SYSPLL_CLK1_ENABLE,
	.enableCLK0             = DL_SYSCTL_SYSPLL_CLK0_DISABLE,
	.sysPLLMCLK             = DL_SYSCTL_SYSPLL_MCLK_CLK2X,
	.sysPLLRef              = DL_SYSCTL_SYSPLL_REF_HFCLK,
	.qDiv                   = 9,
	.pDiv                   = DL_SYSCTL_SYSPLL_PDIV_1
};

SYSCONFIG_WEAK bool SYSCFG_DL_SYSCTL_SYSPLL_init(void)
{
    bool fFCCRatioStatus = false;
    uint32_t fFCCSysoscCount;
    uint32_t fFCCPllCount;
    uint32_t fFCCRatio;
    uint32_t fccTimeOutCounter;

    DL_SYSCTL_setFCCPeriods( DL_SYSCTL_FCC_TRIG_CNT_02 );

    /* Measuring PLL. */
    DL_SYSCTL_configFCC(DL_SYSCTL_FCC_TRIG_TYPE_RISE_RISE,
                        DL_SYSCTL_FCC_TRIG_SOURCE_LFCLK,
                        DL_SYSCTL_FCC_CLOCK_SOURCE_SYSPLLCLK1);
    /* Get SYSPLL frequency using FCC */
    fccTimeOutCounter = 0;
    DL_SYSCTL_startFCC();
    while (DL_SYSCTL_isFCCDone() == 0) {
        delay_cycles(977);  /* 1x LFCLK cycle = 32MHz/32.768kHz = 977, 30.5us */
        fccTimeOutCounter++;
        if(fccTimeOutCounter > 65){
            /* Timeout set to approximately 2ms (user-customizable) */
            break;
        }
    }

    /* get measA= SYSPLLCLK1 freq wrt LFOSC*/
    fFCCPllCount = DL_SYSCTL_readFCC();

    /* Measuring SYSPLL Source */
    DL_SYSCTL_configFCC(DL_SYSCTL_FCC_TRIG_TYPE_RISE_RISE,
                        DL_SYSCTL_FCC_TRIG_SOURCE_LFCLK,
                        DL_SYSCTL_FCC_CLOCK_SOURCE_HFCLK);
    /* Get SYSPLL frequency using FCC */
    fccTimeOutCounter = 0;
    DL_SYSCTL_startFCC();
    while (DL_SYSCTL_isFCCDone() == 0) {
        delay_cycles(977);  /* 1x LFCLK cycle = 32MHz/32.768kHz = 977, 30.5us */
        fccTimeOutCounter++;
        if(fccTimeOutCounter > 65){
            /* Timeout set to approximately 2ms (user-customizable) */
            break;
        }
    }

    /* get measB= SYSOSC freq wrt LFOSC*/
    fFCCSysoscCount = DL_SYSCTL_readFCC();

    /* Get ratio of both measurements*/
    fFCCRatio = (fFCCPllCount * FLOAT_TO_INT_SCALE) / fFCCSysoscCount;
    /* Check ratio is within bounds*/
    if ((FCC_LOWER_BOUND <  fFCCRatio) && (fFCCRatio < FCC_UPPER_BOUND))
    {
        /* ratio is good for proceeding into application code. */
        fFCCRatioStatus = true;
    }

    return fFCCRatioStatus;
}
SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);
    DL_SYSCTL_setFlashWaitState(DL_SYSCTL_FLASH_WAIT_STATE_2);

    
	DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
	/* Set default configuration */
	DL_SYSCTL_disableHFXT();
	DL_SYSCTL_disableSYSPLL();
    DL_SYSCTL_setHFCLKSourceHFXTParams(DL_SYSCTL_HFXT_RANGE_8_16_MHZ,255, true);
    DL_SYSCTL_configSYSPLL((DL_SYSCTL_SYSPLLConfig *) &gSYSPLLConfig);

    /*
     * [SYSPLL_ERR_01]
     * PLL Incorrect locking WA start.
     * Insert after every PLL enable.
     * This can lead an infinite loop if the condition persists
     * and can block entry to the application code.
     */

    while (SYSCFG_DL_SYSCTL_SYSPLL_init() == false)
    {
        /* Toggle SYSPLL enable to re-enable SYSPLL and re-check incorrect locking */
        DL_SYSCTL_disableSYSPLL();
        DL_SYSCTL_enableSYSPLL();

        /* Wait until SYSPLL startup is stabilized*/
        while ((DL_SYSCTL_getClockStatus() & SYSCTL_CLKSTATUS_SYSPLLGOOD_MASK) != DL_SYSCTL_CLK_STATUS_SYSPLL_GOOD){}
    }
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_2);
    DL_SYSCTL_setHFCLKDividerForMFPCLK(DL_SYSCTL_HFCLK_MFPCLK_DIVIDER_10);
    DL_SYSCTL_enableMFCLK();
    DL_SYSCTL_enableMFPCLK();
	DL_SYSCTL_setMFPCLKSource(DL_SYSCTL_MFPCLK_SOURCE_HFCLK);
    DL_SYSCTL_setMCLKSource(SYSOSC, HSCLK, DL_SYSCTL_HSCLK_SOURCE_SYSPLL);
    /* DMA Group Priority */
    NVIC_SetPriority(DMA_INT_IRQn, 3);

}


/*
 * Timer clock configuration to be sourced by  / 1 (80000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   80000000 Hz = 80000000 Hz / (1 * (0 + 1))
 */
static const DL_TimerA_ClockConfig gPWMA0ClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};

static const DL_TimerA_PWMConfig gPWMA0Config = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN,
    .period = 5000,
    .isTimerWithFourCC = true,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_PWMA0_init(void) {

    DL_TimerA_setClockConfig(
        PWMA0_INST, (DL_TimerA_ClockConfig *) &gPWMA0ClockConfig);

    DL_TimerA_initPWMMode(
        PWMA0_INST, (DL_TimerA_PWMConfig *) &gPWMA0Config);

    // Set Counter control to the smallest CC index being used
    DL_TimerA_setCounterControl(PWMA0_INST,DL_TIMER_CZC_CCCTL0_ZCOND,DL_TIMER_CAC_CCCTL0_ACOND,DL_TIMER_CLC_CCCTL0_LCOND);

    DL_TimerA_setCaptureCompareOutCtl(PWMA0_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_0_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(PWMA0_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    DL_TimerA_setCaptureCompareValue(PWMA0_INST, 5000, DL_TIMER_CC_0_INDEX);

    DL_TimerA_setCaptureCompareOutCtl(PWMA0_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_1_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(PWMA0_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_1_INDEX);
    DL_TimerA_setCaptureCompareValue(PWMA0_INST, 5000, DL_TIMER_CC_1_INDEX);

    DL_TimerA_setCaptureCompareOutCtl(PWMA0_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_2_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(PWMA0_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_2_INDEX);
    DL_TimerA_setCaptureCompareValue(PWMA0_INST, 5000, DL_TIMER_CC_2_INDEX);

    DL_TimerA_setCaptureCompareOutCtl(PWMA0_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_3_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(PWMA0_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_3_INDEX);
    DL_TimerA_setCaptureCompareValue(PWMA0_INST, 5000, DL_TIMER_CC_3_INDEX);

    DL_TimerA_enableClock(PWMA0_INST);


    
    DL_TimerA_setCCPDirection(PWMA0_INST , DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT | DL_TIMER_CC2_OUTPUT | DL_TIMER_CC3_OUTPUT );


}


static const DL_TimerG_ClockConfig gQEI0ClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};


SYSCONFIG_WEAK void SYSCFG_DL_QEI0_init(void) {

    DL_TimerG_setClockConfig(
        QEI0_INST, (DL_TimerG_ClockConfig *) &gQEI0ClockConfig);

    DL_TimerG_configQEI(QEI0_INST, DL_TIMER_QEI_MODE_2_INPUT,
        DL_TIMER_CC_INPUT_INV_NOINVERT, DL_TIMER_CC_0_INDEX);
    DL_TimerG_configQEI(QEI0_INST, DL_TIMER_QEI_MODE_2_INPUT,
        DL_TIMER_CC_INPUT_INV_NOINVERT, DL_TIMER_CC_1_INDEX);
    DL_TimerG_setLoadValue(QEI0_INST, 65535);
    DL_TimerG_enableClock(QEI0_INST);
    DL_TimerG_startCounter(QEI0_INST);
}
static const DL_TimerG_ClockConfig gQEI1ClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};


SYSCONFIG_WEAK void SYSCFG_DL_QEI1_init(void) {

    DL_TimerG_setClockConfig(
        QEI1_INST, (DL_TimerG_ClockConfig *) &gQEI1ClockConfig);

    DL_TimerG_configQEI(QEI1_INST, DL_TIMER_QEI_MODE_2_INPUT,
        DL_TIMER_CC_INPUT_INV_NOINVERT, DL_TIMER_CC_0_INDEX);
    DL_TimerG_configQEI(QEI1_INST, DL_TIMER_QEI_MODE_2_INPUT,
        DL_TIMER_CC_INPUT_INV_NOINVERT, DL_TIMER_CC_1_INDEX);
    DL_TimerG_setLoadValue(QEI1_INST, 65535);
    DL_TimerG_enableClock(QEI1_INST);
    DL_TimerG_startCounter(QEI1_INST);
}


static const DL_I2C_ClockConfig gI2C1ClockConfig = {
    .clockSel = DL_I2C_CLOCK_BUSCLK,
    .divideRatio = DL_I2C_CLOCK_DIVIDE_1,
};

SYSCONFIG_WEAK void SYSCFG_DL_I2C1_init(void) {

    DL_I2C_setClockConfig(I2C1_INST,
        (DL_I2C_ClockConfig *) &gI2C1ClockConfig);
    DL_I2C_setAnalogGlitchFilterPulseWidth(I2C1_INST,
        DL_I2C_ANALOG_GLITCH_FILTER_WIDTH_50NS);
    DL_I2C_enableAnalogGlitchFilter(I2C1_INST);

    /* Configure Controller Mode */
    DL_I2C_resetControllerTransfer(I2C1_INST);
    /* Set frequency to 400000 Hz*/
    DL_I2C_setTimerPeriod(I2C1_INST, 9);
    DL_I2C_setControllerTXFIFOThreshold(I2C1_INST, DL_I2C_TX_FIFO_LEVEL_EMPTY);
    DL_I2C_setControllerRXFIFOThreshold(I2C1_INST, DL_I2C_RX_FIFO_LEVEL_BYTES_1);
    DL_I2C_enableControllerClockStretching(I2C1_INST);


    /* Enable module */
    DL_I2C_enableController(I2C1_INST);


}
static const DL_I2C_ClockConfig gI2C2ClockConfig = {
    .clockSel = DL_I2C_CLOCK_BUSCLK,
    .divideRatio = DL_I2C_CLOCK_DIVIDE_1,
};

SYSCONFIG_WEAK void SYSCFG_DL_I2C2_init(void) {

    DL_I2C_setClockConfig(I2C2_INST,
        (DL_I2C_ClockConfig *) &gI2C2ClockConfig);
    DL_I2C_setAnalogGlitchFilterPulseWidth(I2C2_INST,
        DL_I2C_ANALOG_GLITCH_FILTER_WIDTH_50NS);
    DL_I2C_enableAnalogGlitchFilter(I2C2_INST);

    /* Configure Controller Mode */
    DL_I2C_resetControllerTransfer(I2C2_INST);
    /* Set frequency to 400000 Hz*/
    DL_I2C_setTimerPeriod(I2C2_INST, 9);
    DL_I2C_setControllerTXFIFOThreshold(I2C2_INST, DL_I2C_TX_FIFO_LEVEL_EMPTY);
    DL_I2C_setControllerRXFIFOThreshold(I2C2_INST, DL_I2C_RX_FIFO_LEVEL_BYTES_1);
    DL_I2C_enableControllerClockStretching(I2C2_INST);


    /* Enable module */
    DL_I2C_enableController(I2C2_INST);


}

static const DL_UART_Main_ClockConfig gUART0ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART0Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART0_init(void)
{
    DL_UART_Main_setClockConfig(UART0_INST, (DL_UART_Main_ClockConfig *) &gUART0ClockConfig);

    DL_UART_Main_init(UART0_INST, (DL_UART_Main_Config *) &gUART0Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART0_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART0_INST, UART0_IBRD_40_MHZ_115200_BAUD, UART0_FBRD_40_MHZ_115200_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART0_INST,
                                 DL_UART_MAIN_INTERRUPT_DMA_DONE_TX |
                                 DL_UART_MAIN_INTERRUPT_RX);

    /* Configure DMA Transmit Event */
    DL_UART_Main_enableDMATransmitEvent(UART0_INST);
    /* Configure FIFOs */
    DL_UART_Main_enableFIFOs(UART0_INST);
    DL_UART_Main_setRXFIFOThreshold(UART0_INST, DL_UART_RX_FIFO_LEVEL_FULL);
    DL_UART_Main_setTXFIFOThreshold(UART0_INST, DL_UART_TX_FIFO_LEVEL_EMPTY);

    DL_UART_Main_enable(UART0_INST);
}
static const DL_UART_Main_ClockConfig gUART1ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART1Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART1_init(void)
{
    DL_UART_Main_setClockConfig(UART1_INST, (DL_UART_Main_ClockConfig *) &gUART1ClockConfig);

    DL_UART_Main_init(UART1_INST, (DL_UART_Main_Config *) &gUART1Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART1_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART1_INST, UART1_IBRD_40_MHZ_115200_BAUD, UART1_FBRD_40_MHZ_115200_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART1_INST,
                                 DL_UART_MAIN_INTERRUPT_DMA_DONE_TX |
                                 DL_UART_MAIN_INTERRUPT_RX);

    /* Configure DMA Transmit Event */
    DL_UART_Main_enableDMATransmitEvent(UART1_INST);
    /* Configure FIFOs */
    DL_UART_Main_enableFIFOs(UART1_INST);
    DL_UART_Main_setRXFIFOThreshold(UART1_INST, DL_UART_RX_FIFO_LEVEL_FULL);
    DL_UART_Main_setTXFIFOThreshold(UART1_INST, DL_UART_TX_FIFO_LEVEL_EMPTY);

    DL_UART_Main_enable(UART1_INST);
}
static const DL_UART_Main_ClockConfig gUART3ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART3Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART3_init(void)
{
    DL_UART_Main_setClockConfig(UART3_INST, (DL_UART_Main_ClockConfig *) &gUART3ClockConfig);

    DL_UART_Main_init(UART3_INST, (DL_UART_Main_Config *) &gUART3Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART3_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART3_INST, UART3_IBRD_80_MHZ_115200_BAUD, UART3_FBRD_80_MHZ_115200_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART3_INST,
                                 DL_UART_MAIN_INTERRUPT_DMA_DONE_TX |
                                 DL_UART_MAIN_INTERRUPT_RX);

    /* Configure DMA Transmit Event */
    DL_UART_Main_enableDMATransmitEvent(UART3_INST);
    /* Configure FIFOs */
    DL_UART_Main_enableFIFOs(UART3_INST);
    DL_UART_Main_setRXFIFOThreshold(UART3_INST, DL_UART_RX_FIFO_LEVEL_FULL);
    DL_UART_Main_setTXFIFOThreshold(UART3_INST, DL_UART_TX_FIFO_LEVEL_EMPTY);

    DL_UART_Main_enable(UART3_INST);
}
static const DL_UART_Main_ClockConfig gUART4ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART4Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART4_init(void)
{
    DL_UART_Main_setClockConfig(UART4_INST, (DL_UART_Main_ClockConfig *) &gUART4ClockConfig);

    DL_UART_Main_init(UART4_INST, (DL_UART_Main_Config *) &gUART4Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART4_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART4_INST, UART4_IBRD_80_MHZ_115200_BAUD, UART4_FBRD_80_MHZ_115200_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART4_INST,
                                 DL_UART_MAIN_INTERRUPT_DMA_DONE_TX |
                                 DL_UART_MAIN_INTERRUPT_RX);

    /* Configure DMA Transmit Event */
    DL_UART_Main_enableDMATransmitEvent(UART4_INST);
    /* Configure FIFOs */
    DL_UART_Main_enableFIFOs(UART4_INST);
    DL_UART_Main_setRXFIFOThreshold(UART4_INST, DL_UART_RX_FIFO_LEVEL_FULL);
    DL_UART_Main_setTXFIFOThreshold(UART4_INST, DL_UART_TX_FIFO_LEVEL_EMPTY);

    DL_UART_Main_enable(UART4_INST);
}

static const DL_DMA_Config gDMA_CH0Config = {
    .transferMode   = DL_DMA_SINGLE_TRANSFER_MODE,
    .extendedMode   = DL_DMA_NORMAL_MODE,
    .destIncrement  = DL_DMA_ADDR_UNCHANGED,
    .srcIncrement   = DL_DMA_ADDR_INCREMENT,
    .destWidth      = DL_DMA_WIDTH_BYTE,
    .srcWidth       = DL_DMA_WIDTH_BYTE,
    .trigger        = UART0_INST_DMA_TRIGGER,
    .triggerType    = DL_DMA_TRIGGER_TYPE_EXTERNAL,
};

SYSCONFIG_WEAK void SYSCFG_DL_DMA_CH0_init(void)
{
    DL_DMA_initChannel(DMA, DMA_CH0_CHAN_ID , (DL_DMA_Config *) &gDMA_CH0Config);
}
static const DL_DMA_Config gDMA_CH1Config = {
    .transferMode   = DL_DMA_SINGLE_TRANSFER_MODE,
    .extendedMode   = DL_DMA_NORMAL_MODE,
    .destIncrement  = DL_DMA_ADDR_UNCHANGED,
    .srcIncrement   = DL_DMA_ADDR_INCREMENT,
    .destWidth      = DL_DMA_WIDTH_BYTE,
    .srcWidth       = DL_DMA_WIDTH_BYTE,
    .trigger        = UART1_INST_DMA_TRIGGER,
    .triggerType    = DL_DMA_TRIGGER_TYPE_EXTERNAL,
};

SYSCONFIG_WEAK void SYSCFG_DL_DMA_CH1_init(void)
{
    DL_DMA_initChannel(DMA, DMA_CH1_CHAN_ID , (DL_DMA_Config *) &gDMA_CH1Config);
}
static const DL_DMA_Config gDMA_CH2Config = {
    .transferMode   = DL_DMA_SINGLE_TRANSFER_MODE,
    .extendedMode   = DL_DMA_NORMAL_MODE,
    .destIncrement  = DL_DMA_ADDR_UNCHANGED,
    .srcIncrement   = DL_DMA_ADDR_INCREMENT,
    .destWidth      = DL_DMA_WIDTH_BYTE,
    .srcWidth       = DL_DMA_WIDTH_BYTE,
    .trigger        = UART3_INST_DMA_TRIGGER,
    .triggerType    = DL_DMA_TRIGGER_TYPE_EXTERNAL,
};

SYSCONFIG_WEAK void SYSCFG_DL_DMA_CH2_init(void)
{
    DL_DMA_initChannel(DMA, DMA_CH2_CHAN_ID , (DL_DMA_Config *) &gDMA_CH2Config);
}
static const DL_DMA_Config gDMA_CH3Config = {
    .transferMode   = DL_DMA_SINGLE_TRANSFER_MODE,
    .extendedMode   = DL_DMA_NORMAL_MODE,
    .destIncrement  = DL_DMA_ADDR_UNCHANGED,
    .srcIncrement   = DL_DMA_ADDR_INCREMENT,
    .destWidth      = DL_DMA_WIDTH_BYTE,
    .srcWidth       = DL_DMA_WIDTH_BYTE,
    .trigger        = UART4_INST_DMA_TRIGGER,
    .triggerType    = DL_DMA_TRIGGER_TYPE_EXTERNAL,
};

SYSCONFIG_WEAK void SYSCFG_DL_DMA_CH3_init(void)
{
    DL_DMA_initChannel(DMA, DMA_CH3_CHAN_ID , (DL_DMA_Config *) &gDMA_CH3Config);
}
SYSCONFIG_WEAK void SYSCFG_DL_DMA_init(void){
    SYSCFG_DL_DMA_CH0_init();
    SYSCFG_DL_DMA_CH1_init();
    SYSCFG_DL_DMA_CH2_init();
    SYSCFG_DL_DMA_CH3_init();
}


static const DL_MCAN_ClockConfig gMCAN0ClockConf = {
    .clockSel = DL_MCAN_FCLK_SYSPLLCLK1,
    .divider  = DL_MCAN_FCLK_DIV_1,
};

static const DL_MCAN_InitParams gMCAN0InitParams= {

/* Initialize MCAN Init parameters.    */
    .fdMode            = false,
    .brsEnable         = false,
    .txpEnable         = false,
    .efbi              = false,
    .pxhddisable       = false,
    .darEnable         = false,
    .wkupReqEnable     = false,
    .autoWkupEnable    = false,
    .emulationEnable   = false,
    .tdcEnable         = false,
    .wdcPreload        = 255,

/* Transmitter Delay Compensation parameters. */
    .tdcConfig.tdcf    = 10,
    .tdcConfig.tdco    = 6,
};


static const DL_MCAN_MsgRAMConfigParams gMCAN0MsgRAMConfigParams ={

    /* Standard ID Filter List Start Address. */
    .flssa                = MCAN0_INST_MCAN_STD_ID_FILT_START_ADDR,
    /* List Size: Standard ID. */
    .lss                  = MCAN0_INST_MCAN_STD_ID_FILTER_NUM,
    /* Extended ID Filter List Start Address. */
    .flesa                = MCAN0_INST_MCAN_EXT_ID_FILT_START_ADDR,
    /* List Size: Extended ID. */
    .lse                  = MCAN0_INST_MCAN_EXT_ID_FILTER_NUM,
    /* Tx Buffers Start Address. */
    .txStartAddr          = MCAN0_INST_MCAN_TX_BUFF_START_ADDR,
    /* Number of Dedicated Transmit Buffers. */
    .txBufNum             = MCAN0_INST_MCAN_TX_BUFF_SIZE,
    .txFIFOSize           = 0,
    /* Tx Buffer Element Size. */
    .txBufMode            = 0,
    .txBufElemSize        = DL_MCAN_ELEM_SIZE_8BYTES,
    /* Tx Event FIFO Start Address. */
    .txEventFIFOStartAddr = MCAN0_INST_MCAN_TX_EVENT_START_ADDR,
    /* Event FIFO Size. */
    .txEventFIFOSize      = MCAN0_INST_MCAN_TX_EVENT_SIZE,
    /* Level for Tx Event FIFO watermark interrupt. */
    .txEventFIFOWaterMark = 3,
    /* Rx FIFO0 Start Address. */
    .rxFIFO0startAddr     = MCAN0_INST_MCAN_FIFO_0_START_ADDR,
    /* Number of Rx FIFO elements. */
    .rxFIFO0size          = MCAN0_INST_MCAN_FIFO_0_NUM,
    /* Rx FIFO0 Watermark. */
    .rxFIFO0waterMark     = 1,
    .rxFIFO0OpMode        = 0,
    /* Rx FIFO1 Start Address. */
    .rxFIFO1startAddr     = MCAN0_INST_MCAN_FIFO_1_START_ADDR,
    /* Number of Rx FIFO elements. */
    .rxFIFO1size          = MCAN0_INST_MCAN_FIFO_1_NUM,
    /* Level for Rx FIFO 1 watermark interrupt. */
    .rxFIFO1waterMark     = 3,
    /* FIFO blocking mode. */
    .rxFIFO1OpMode        = 0,
    /* Rx Buffer Start Address. */
    .rxBufStartAddr       = MCAN0_INST_MCAN_RX_BUFF_START_ADDR,
    /* Rx Buffer Element Size. */
    .rxBufElemSize        = DL_MCAN_ELEM_SIZE_8BYTES,
    /* Rx FIFO0 Element Size. */
    .rxFIFO0ElemSize      = DL_MCAN_ELEM_SIZE_8BYTES,
    /* Rx FIFO1 Element Size. */
    .rxFIFO1ElemSize      = DL_MCAN_ELEM_SIZE_8BYTES,
};

static const DL_MCAN_StdMsgIDFilterElement gMCAN0StdFiltelem = {
    .sfec = 0x1,
    .sft  = 0x0,
    .sfid1 = 0,
    .sfid2 = 2047,
};

static const DL_MCAN_ExtMsgIDFilterElement gMCAN0ExtFiltelem = {
    .efec = 0x1,
    .eft  = 0x3,
    .efid1 = 0,
    .efid2 = 0,
};

static const DL_MCAN_BitTimingParams   gMCAN0BitTimes = {
    /* Arbitration Baud Rate Pre-scaler. */
    .nomRatePrescalar   = 0,
    /* Arbitration Time segment before sample point. */
    .nomTimeSeg1        = 58,
    /* Arbitration Time segment after sample point. */
    .nomTimeSeg2        = 19,
    /* Arbitration (Re)Synchronization Jump Width Range. */
    .nomSynchJumpWidth  = 19,
    /* Data Baud Rate Pre-scaler. */
    .dataRatePrescalar  = 0,
    /* Data Time segment before sample point. */
    .dataTimeSeg1       = 0,
    /* Data Time segment after sample point. */
    .dataTimeSeg2       = 0,
    /* Data (Re)Synchronization Jump Width.   */
    .dataSynchJumpWidth = 0,
};


SYSCONFIG_WEAK void SYSCFG_DL_MCAN0_init(void) {
    DL_MCAN_RevisionId revid_MCAN0;

    DL_MCAN_enableModuleClock(MCAN0_INST);

    DL_MCAN_setClockConfig(MCAN0_INST, (DL_MCAN_ClockConfig *) &gMCAN0ClockConf);

    /* Get MCANSS Revision ID. */
    DL_MCAN_getRevisionId(MCAN0_INST, &revid_MCAN0);

    /* Wait for Memory initialization to be completed. */
    while(false == DL_MCAN_isMemInitDone(MCAN0_INST));

    /* Put MCAN in SW initialization mode. */

    DL_MCAN_setOpMode(MCAN0_INST, DL_MCAN_OPERATION_MODE_SW_INIT);

    /* Wait till MCAN is not initialized. */
    while (DL_MCAN_OPERATION_MODE_SW_INIT != DL_MCAN_getOpMode(MCAN0_INST));

    /* Initialize MCAN module. */
    DL_MCAN_init(MCAN0_INST, (DL_MCAN_InitParams *) &gMCAN0InitParams);


    /* Configure Bit timings. */
    DL_MCAN_setBitTime(MCAN0_INST, (DL_MCAN_BitTimingParams*) &gMCAN0BitTimes);

    /* Configure Message RAM Sections */
    DL_MCAN_msgRAMConfig(MCAN0_INST, (DL_MCAN_MsgRAMConfigParams*) &gMCAN0MsgRAMConfigParams);

    /* Configure Standard ID filter element */
    DL_MCAN_addStdMsgIDFilter(MCAN0_INST, 0U, (DL_MCAN_StdMsgIDFilterElement *) &gMCAN0StdFiltelem);

    /* Configure Extended ID filter element*/
    DL_MCAN_addExtMsgIDFilter(MCAN0_INST, 0U, (DL_MCAN_ExtMsgIDFilterElement *) &gMCAN0ExtFiltelem);

    /* Set Extended ID Mask. */
    DL_MCAN_setExtIDAndMask(MCAN0_INST, MCAN0_INST_MCAN_EXT_ID_AND_MASK );

    /* Loopback mode */

    /* Take MCAN out of the SW initialization mode */
    DL_MCAN_setOpMode(MCAN0_INST, DL_MCAN_OPERATION_MODE_NORMAL);

    while (DL_MCAN_OPERATION_MODE_NORMAL != DL_MCAN_getOpMode(MCAN0_INST));

    /* Enable MCAN mopdule Interrupts */
    DL_MCAN_enableIntr(MCAN0_INST, MCAN0_INST_MCAN_INTERRUPTS, 1U);

    DL_MCAN_selectIntrLine(MCAN0_INST, DL_MCAN_INTERRUPT_RF0N|DL_MCAN_INTERRUPT_TEFN, DL_MCAN_INTR_LINE_NUM_0);
    DL_MCAN_enableIntrLine(MCAN0_INST, DL_MCAN_INTR_LINE_NUM_0, 1U);

    /* Enable MSPM0 MCAN interrupt */
    DL_MCAN_clearInterruptStatus(MCAN0_INST,(DL_MCAN_MSP_INTERRUPT_LINE0));
    DL_MCAN_enableInterrupt(MCAN0_INST,(DL_MCAN_MSP_INTERRUPT_LINE0));

}

