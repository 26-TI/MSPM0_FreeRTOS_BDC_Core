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
#include "ins.hpp"
#include "can_stepper_motor.hpp"

/* ---- 函数声明 ---- */

/**
 * @brief 设备层统一初始化 — 绑定 BSP 驱动到设备对象
 *
 * 初始化顺序: 左轮电机 → 右轮电机 → 陀螺仪 → OLED → 巡线 → INS
 *
 * @note 必须在 bsp_init() 之后、任务创建之前调用
 */
void device_init();

/* ---- 全局声明 ---- */
extern TiGyro   ti_gyro;
extern Motor    motor_left;  ///< 左轮电机 (PWM-G0 CH0/CH1, QEI0)
extern Motor    motor_right; ///< 右轮电机 (PWM-G0 CH2/CH3, QEI1)
extern IicOled  iic_oled;    ///< OLED SSD1306 (I2C2, 0x3C)
extern IicTrack iic_track;   ///< 巡线 PCA9555 (I2C1, 0x20)
extern INS      ins;         ///< 惯性导航角度闭环
extern StepperMotor g_step1;    ///< CAN 步进电机 #1 (CAN ID=0x301)
extern StepperMotor g_step2;    ///< CAN 步进电机 #2 (CAN ID=0x302)

/** @brief CAN 电机发送回调 (→ bsp_can.send) */
bool can_motor_send(uint32_t id, const uint8_t *data, uint8_t len);

#endif // __DEVICE_CFG_HPP__
