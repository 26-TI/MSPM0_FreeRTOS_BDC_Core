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
extern Motor    motor;
extern IicOled  iic_oled;
extern IicTrack iic_track;


#endif // __DEVICE_CFG_HPP__
