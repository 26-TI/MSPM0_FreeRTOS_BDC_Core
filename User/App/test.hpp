/**
 * @file test.hpp
 * @brief FreeRTOS 测试任务统一入口声明
 *
 * 包含 7 个测试任务:
 *   - can_send_test_task      CAN 简单发送（每 500ms 一帧，递增计数）
 *   - can_recv_isr_test_task  CAN 接收打印（ISR 系统阻塞模式）
 *   - pwm_test_task           PWM 占空比测试（打印频率/周期/各通道占空比）
 *   - gyro_test_task          陀螺仪姿态测试（200ms 打印 yaw/pitch/roll）
 *   - gpio_test_task          GPIO/按键测试（LED 闪烁 + 按键消抖打印）
 *   - proto_send_test_task    串口协议发送测试
 *   - motor_test_task         双电机开环控制测试
 *
 * 使用方式:
 * @code
 *   #include "test.hpp"
 *   xTaskCreate(gpio_test_task, "gpio", 0x100, NULL, tskIDLE_PRIORITY + 1, NULL);
 * @endcode
 */

#ifndef __TEST_HPP__
#define __TEST_HPP__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /* ===================================================================
 * 测试任务入口声明
 * =================================================================== */

  /**
 * @brief CAN 发送测试任务（非阻塞）
 *
 * 每 500ms 发一帧标准 CAN 帧（ID=0x123，8 字节递增数据）。
 * TX Buffer 忙时跳过该周期（非阻塞）。
 *
 * @param arg FreeRTOS 任务参数（未使用）
 */
  void can_send_test_task(void *arg);

  /**
 * @brief CAN 接收测试任务（ISR 模式）
 *
 * 调用 enableIrq() 使能 RF0N 中断，ISR 自动搬 FIFO→StreamBuffer。
 * recvRead(portMAX_DELAY) 系统阻塞等待，任务挂起不占 CPU。
 *
 * @param arg FreeRTOS 任务参数（未使用）
 */
  void can_recv_isr_test_task(void *arg);

  /**
 * @brief PWM 输出测试任务
 *
 * 初始化 PWM_G0 双通道占空比后，每 500ms 打印一次频率、周期和各通道占空比。
 *
 * @param arg FreeRTOS 任务参数（未使用）
 */
  void pwm_test_task(void *arg);

  /**
 * @brief 陀螺仪姿态测试任务
 *
 * 设置姿态报告模式，每 200ms 读取并打印 yaw/pitch/roll。
 *
 * @note 依赖 ti_gyro 全局实例 (TiGyro 设备驱动)，通过 UART1 (PB7/PB6) 通信。
 *       需要外接陀螺仪硬件。
 * @param arg FreeRTOS 任务参数（未使用）
 */
  void gyro_test_task(void *arg);

  /**
 * @brief GPIO / 按键测试任务
 *
 * 每 5ms 轮询 4 个按键（消抖），打印按下/释放/长按事件。
 * LED 每 500ms 翻转，蜂鸣器随按键短鸣。
 * 无硬件依赖（使用板载 GPIO）。
 *
 * @param arg FreeRTOS 任务参数（未使用）
 */
  void gpio_test_task(void *arg);

  /**
 * @brief 串口协议发送测试任务
 *
 * 每 500ms 通过协议发送一帧 (CMD=0x10, 2B 递增数据)。
 * 使用 UART2，可与上位机或回环测试搭配。
 *
 * @param arg FreeRTOS 任务参数（未使用）
 */
  void proto_send_test_task(void *arg);

  void motor_test_task(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __TEST_HPP__ */
