/**
 * @file device_cfg.cpp
 * @brief 设备层总配置 — 设备实例与初始化
 */

#include "device_cfg.hpp"
#include "bsp_cfg.hpp"

/* ==================== 全局实例 ==================== */
TiGyro   ti_gyro;
Motor    motor_left;
Motor    motor_right;
IicOled  iic_oled;
IicTrack iic_track;
INS      ins;

/* ==================== 设备初始化 ==================== */
void device_init()
{
  /* ---- 左轮电机 — PWM-G0 CH0(正转)/CH3(反转), QEI1, EN=PA15 ---- */
  motor_left.Motor_Init({
    .pwm               = &bsp_pwm_a0,
    .qei               = &bsp_qei1,
    .enable_pin        = &gpio_men1, /* PA15: 左轮使能 */
    .pwm_ch_a          = 3,          /* CH0: PA7 */
    .pwm_ch_b          = 0,          /* CH3: PA28 */
    .encoder_lines     = 500,        /* 编码器线数 */
    .gear_ratio        = 28.0f,      /* 减速比 28:1 */
    .control_period_ms = 10,         /* 10ms 控制周期 */
    .pwm_max           = 5000,       /* PWM 周期 */
  });

  motor_left.PID_Init(1000, 25000, 0, 0.01, 2400, 1100, 0);

  /* ---- 右轮电机 — PWM-G0 CH2(正转)/CH1(反转), QEI0, EN=PA16 ---- */
  motor_right.Motor_Init({
    .pwm               = &bsp_pwm_a0,
    .qei               = &bsp_qei0,
    .enable_pin        = &gpio_men2, /* PA16: 右轮使能 */
    .pwm_ch_a          = 2,          /* CH2: PA1 */
    .pwm_ch_b          = 1,          /* CH1: PA0 */
    .encoder_lines     = 500,
    .gear_ratio        = 28.0f,
    .control_period_ms = 10,   /* 10ms 控制周期 */
    .pwm_max           = 5000, /* PWM 周期 */
  });

  motor_right.PID_Init(1000, 25000, 0, 0.01, 2400, 1100, 0);

  /* 陀螺仪 — UART3 */
  ti_gyro.init(&bsp_uart3);

  /* OLED — I2C2, 地址 0x3C */
  iic_oled.init(&bsp_i2c2, 0x3C);

  /* Track (PCA9555 巡线) — I2C1, 地址 0x20 */
  iic_track.init(&bsp_i2c1, 0x20);

  /* INS 角度闭环 — 陀螺仪 + 双电机 */
  ins.init(&ti_gyro, &motor_left, &motor_right);
  ins._pid.init(0.05, 0.0, 0.0, 0.01, 1.0, 1, 1);

}
