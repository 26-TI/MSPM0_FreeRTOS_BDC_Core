/**
 * @file test.hpp
 * @brief FreeRTOS 测试任务统一入口声明
 *
 * 包含 7 个测试任务:
 *   - can_send_test_task   CAN 简单发送（每 500ms 一帧，递增计数）
 *   - can_recv_test_task   CAN 接收打印（2ms 轮询模式）
 *   - can_recv_isr_test_task  CAN 接收打印（ISR 系统阻塞模式）
 *   - pwm_test_task        PWM 占空比测试（打印频率/周期/各通道占空比）
 *   - gyro_test_task       陀螺仪姿态测试（200ms 打印 yaw/pitch/roll）
 *   - track_test_task      巡线传感器测试（5ms 读取 PCA9555）
 *   - oled_test_task       OLED 显示测试（100ms 刷新巡线数据）
 *   - gpio_test_task       GPIO/按键测试（LED 闪烁 + 按键消抖打印）
 *   - relay_test_task      继电器测试（每 2s 翻转）
 *
 * 使用方式:
 * @code
 *   #include "test.hpp"
 *   xTaskCreate(blink_test_task, "blink", 0x80, NULL, tskIDLE_PRIORITY + 1, NULL);
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
 * @brief CAN 接收测试任务（轮询模式）
 *
 * 每 2ms 调用 recvPoll() 从 FIFO 搬数据到 StreamBuffer，
 * recvRead(timeout=0) 非阻塞取出并打印。
 * 裸机读取 FIFO，无中断依赖。
 *
 * @param arg FreeRTOS 任务参数（未使用）
 */
  void can_recv_test_task(void *arg);

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
 * @brief 巡线传感器测试任务
 *
 * 每 5ms 通过 I2C1 (PA29/PA30) 读取 PCA9555 巡线数据，
 * 写入全局变量 g_track_data 供 OLED 任务展示。
 *
 * @note 依赖 iic_track 全局实例 (IicTrack 设备驱动)。
 *       需要外接 PCA9555 巡线模块。
 * @param arg FreeRTOS 任务参数（未使用）
 */
  void track_test_task(void *arg);

  /**
 * @brief OLED 显示测试任务
 *
 * 每 100ms 刷新 OLED 屏幕 (128x64):
 * - 第 0 行: 巡线原始数据 HEX 值
 * - 第 2 行: 低 12 位 bit 图 (字符 '1'/'0')
 *
 * @note 依赖 iic_oled 全局实例 (IicOled 设备驱动)，通过 I2C0 (PA0/PA1) 通信。
 *       读取 g_track_data (由 track_test_task 更新)。
 *       需要外接 OLED 显示屏。
 * @param arg FreeRTOS 任务参数（未使用）
 */
  void oled_test_task(void *arg);

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

  /**
 * @brief CAN 电机协议测试任务
 *
 * 使用 CAN 回环模式，测试 can_motor_proto 协议栈:
 *   - Phase 1: 软件层帧编解码验证（buildMotorFrame / checksum / parseStatus）
 *   - Phase 2: DcMotor 回环 (enable / setDuty / setRpm / stop)
 *   - Phase 3: StepperMotor 回环 (enable / setSpeed / moveTo / moveAbs / timedMove / stop)
 *
 * @note 使用 CAN 回环模式，无需外部电机硬件。
 *       测试结果通过 UART2 输出，最后打印 PASS/FAIL 汇总。
 * @param arg FreeRTOS 任务参数（未使用）
 */
  void motor_proto_test_task(void *arg);

  /**
 * @brief 双电机开环控制测试任务
 *
 * 测试流程（PID 未调试，使用开环控制）:
 *   - Phase 1: 两个电机同时正转，占空比从 10% → 50% 逐步递增
 *   - Phase 2: 两个电机同时反转，占空比从 10% → 50%
 *   - Phase 3: 左轮正转 + 右轮反转（原地旋转）
 *   - Phase 4: 全部停止
 *   - 每 500ms 通过 UART 打印左右轮速度/脉冲/方向
 *
 * @note 依赖 motor_left / motor_right 全局实例
 * @param arg FreeRTOS 任务参数（未使用）
 */
  void motor_test_task(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __TEST_HPP__ */
