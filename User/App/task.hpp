/**
 * @file task.hpp
 * @brief FreeRTOS 正式任务统一入口声明
 *
 * 包含正式功能任务:
 *   - gpio_task   GPIO/按键任务（三按键短按/长按检测 + 蜂鸣器 + LED）
 *   - gyro_task   陀螺仪接收任务（阻塞接收，解析姿态帧）
 *   - track_task  巡线传感器读取任务（10ms 读取 PCA9555）
 *
 * 使用方式:
 * @code
 *   #include "task.hpp"
 *   xTaskCreate(gpio_task, "gpio", 0x100, NULL, configMAX_PRIORITIES - 3, NULL);
 * @endcode
 */

#ifndef __TASK_HPP__
#define __TASK_HPP__

#include <stdint.h>

#include "FreeRTOS.h" // IWYU pragma: keep
#include "semphr.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /* ===================================================================
   * 正式任务入口声明
   * =================================================================== */

  /**
   * @brief GPIO / 按键任务
   *
   * 每 50ms 轮询 3 个按键（消抖 100ms），区分短按/长按（1s）。
   * 短按/长按均触发蜂鸣器短鸣，LED 每 500ms 翻转。
   *
   * @param arg FreeRTOS 任务参数（未使用）
   */
  void gpio_task(void *arg);

  /**
   * @brief 陀螺仪接收任务
   *
   * 阻塞等待 UART3 陀螺仪数据帧，解析后更新 ti_gyro 内部缓存。
   * 不打印，供 OLED 等任务读取最新姿态。
   *
   * @param arg FreeRTOS 任务参数（未使用）
   */
  void gyro_task(void *arg);

  /**
   * @brief 巡线传感器读取任务
   *
   * 每 10ms 读取 PCA9555 (I2C1) 12路巡线数据，存入 g_track_data。
   *
   * @param arg FreeRTOS 任务参数（未使用）
   */
  void track_task(void *arg);

  /**
   * @brief 电机控制更新任务
   *
   * 每 5ms 调用左右双电机 Motor::update()，完成编码器读取和 PID 闭环。
   * 优先级 7（最高），确保控制周期精确。
   *
   * @param arg FreeRTOS 任务参数（未使用）
   */
  void motor_task(void *arg);

  /**
   * @brief 电机测试波形任务
   *
   * 正弦/方波/恒速测试，由 TEST_SINE_WAVE / TEST_SQUARE_WAVE 宏选择。
   * 优先级 6，周期 2ms。
   *
   * @param arg FreeRTOS 任务参数（未使用）
   */
  void motor_test_task(void *arg);

  /**
   * @brief 电机数据打印任务
   *
   * 每 50ms 打印左右双轮的目标速度、实际速度、PID 输出。
   * 优先级 5，串口输出。
   *
   * @param arg FreeRTOS 任务参数（未使用）
   */
  void motor_print_task(void *arg);

  /**
   * @brief 循线控制任务
   *
   * 12路传感器权值法（中间=0），PID 差速控制双轮。
   * 左轮正速度=正转，右轮反速度=正转。速度单位: 转/秒。
   * 优先级 6，周期 10ms。
   *
   * @param arg FreeRTOS 任务参数（未使用）
   */
  void line_task(void *arg);

  /**
   * @brief OLED 显示任务
   *
   * 每 100ms 刷新 OLED：行0=12路巡线传感器，行1~3=陀螺仪 Yaw/Roll/Pitch。
   * 优先级 3，依赖 iic_oled / ti_gyro / g_track_data。
   *
   * @param arg FreeRTOS 任务参数（未使用）
   */
  void oled_task(void *arg);

  /**
   * @brief CPU 占用率监控任务
   *
   * 每秒采样空闲 tick 计数器，计算并打印 CPU 占用率。
   * 依赖 configUSE_IDLE_HOOK=1 和 vApplicationIdleHook()。
   * 优先级 1（仅高于空闲任务）。
   *
   * @param arg FreeRTOS 任务参数（未使用）
   */
  void cpu_monitor_task(void *arg);

  /**
   * @brief 脱线处理任务
   *
   * 永久阻塞等待 g_line_off_sem 信号量。
   * 收到信号量后执行脱线保护动作（停车等）。
   * 优先级 6。
   *
   * @param arg FreeRTOS 任务参数（未使用）
   */
  void line_off_task(void *arg);

  /**
   * @brief 角度闭环控制任务
   *
   * 每 10ms 调用 ins.update() 执行角度闭环差速控制。
   * 依赖 ins / ti_gyro / motor_left / motor_right。
   * 优先级 6。
   *
   * @param arg FreeRTOS 任务参数（未使用）
   */
  void angle_task(void *arg);

  /**
   * @brief 角度闭环测试任务
   *
   * 每 5s 轮转目标角度 (0°→90°→180°→270°→0°)，串口打印状态。
   * 优先级 4。
   *
   * @param arg FreeRTOS 任务参数（未使用）
   */
  void angle_test_task(void *arg);

  /**
   * @brief OLED 菜单任务
   *
   * 每 50ms 消费按键信号量 → 状态机 → OLED 渲染。
   * 3键导航: key1=UP, key2=DOWN, key3短=ENTER, key3长=HOME。
   * 优先级 3，独占 OLED。
   *
   * @param arg FreeRTOS 任务参数（未使用）
   */
  void menu_task(void *arg);

  /**
   * @brief 启动初始化任务
   *
   * 系统启动时以最高优先级运行一次:
   *   1. 调用 bsp_init / protocol_init / device_init 初始化所有外设
   *   2. 创建任务间信号量
   *   3. 创建所有正式任务
   *   4. vTaskDelete(NULL) 自删除
   *
   * @param arg FreeRTOS 任务参数（未使用）
   */
  void init_task(void *arg);

  /**
   * @brief 菜单事件信号量测试任务
   *
   * 每 50ms 轮询所有 g_menu_sems[] 信号量，收到后串口打印事件名称。
   * 用于验证 菜单 → 信号量 give → 任务 take 的完整链路。
   * 优先级 3。
   *
   * @param arg FreeRTOS 任务参数（未使用）
   */
  void debug_print_task(void *arg);

  /* ===================================================================
   * 任务间共享变量
   * =================================================================== */

  /** @brief 脱线检测信号量（line_task 给出, line_off_task 接收） */
  extern SemaphoreHandle_t g_line_off_sem;

#ifdef __cplusplus
}
#endif

#endif /* __TASK_HPP__ */
