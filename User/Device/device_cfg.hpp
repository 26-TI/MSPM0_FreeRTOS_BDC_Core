/**
 * @file device_cfg.hpp
 * @brief 设备层总配置 — 全局声明与初始化入口
 */

#ifndef __DEVICE_CFG_HPP__
#define __DEVICE_CFG_HPP__

#include "ti_gyro.hpp"
#include "motor.hpp"
#include "iic_oled.hpp"
#include "iic_track.hpp"

/* ==================== 函数声明 ==================== */
void device_init();

/* ==================== 全局声明 ==================== */
extern TiGyro   ti_gyro;
extern Motor    motor_left;  ///< 左轮电机 (PWM-G0 CH0/CH1, QEI0)
extern Motor    motor_right; ///< 右轮电机 (PWM-G0 CH2/CH3, QEI1)
extern IicOled  iic_oled;    ///< OLED SSD1306 (I2C2, 0x3C)
extern IicTrack iic_track;   ///< 巡线 PCA9555 (I2C1, 0x20)

#endif // __DEVICE_CFG_HPP__
