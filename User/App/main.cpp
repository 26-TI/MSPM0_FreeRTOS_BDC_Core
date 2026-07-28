/**
 * @file main.cpp
 * @brief 主程序入口 — 系统初始化 + FreeRTOS 任务创建
 *
 * 初始化流程:
 *   1. SYSCFG_DL_init()  硬件初始化（SysConfig 生成：时钟/GPIO/外设）
 *   2. bsp_init()        BSP 驱动对象绑定
 *   3. device_init()     设备层初始化（暂未使用）
 *   4. xTaskCreate()     创建 FreeRTOS 任务
 *   5. vTaskStartScheduler()  启动调度器（永不返回）
 *
 * @note 所有测试任务分散在独立文件中:
 *   - gpio_test.cpp      GPIO/按键（LED 闪烁 + 按键打印）
 *   - can_send_test.cpp  CAN 非阻塞发送
 *   - can_recv_test.cpp  CAN 接收（ISR 模式）
 *   - pwm_test.cpp       PWM 占空比测试
 *   - gyro_test.cpp      陀螺仪姿态测试
 *   - gyro_recv_test.cpp 陀螺仪原始数据接收 (UART3→UART4 HEX dump)
 *   - uart4_recv_test.cpp UART4 串口监视回显
 *   - track_test.cpp     巡线传感器读取 (PCA9555, 10ms)
 *   - oled_test.cpp      OLED 显示 (巡线+陀螺仪姿态)
 *   - proto_test.cpp     串口协议测试
 *   - motor_test.cpp     双电机开环控制测试
 *
 * @note 开关测试任务: 在 main() 中注释/取消注释对应的 xTaskCreate() 即可。
 */

#include "main.hpp"

/* ===================================================================
 * main() — 系统入口
 * =================================================================== */

int main(void)
{
  /* -------- 硬件初始化 -------- */
  SYSCFG_DL_init(); /* SysConfig 自动生成: 时钟 / GPIO / 外设 */
  bsp_init();       /* 驱动层初始化 */
  protocol_init();  /* 协议层初始化 */
  device_init();    /* 设备层初始化 */

  // 任务优先级范围为0-7,最高优先级为7,最低优先级为0,空闲任务优先级为1

  /* -------- 创建 FreeRTOS 任务 -------- */

  /* 巡线传感器读取处理 (PCA9555, I2C1) */
  xTaskCreate(track_task, "track", 0x100, NULL, 7, NULL);

  /* 循线控制 (5ms */
  xTaskCreate(line_task, "line", 0x600, NULL, 6, NULL);

  /* 电机控制更新 (5ms */
  xTaskCreate(motor_task, "motor", 0x400, NULL, 5, NULL);

  /* 电机数据打印 (5ms, 优先级 5) */
  xTaskCreate(motor_print_task, "motor_prt", 0x200, NULL, 4, NULL);

  /* GPIO / 按键处理 — 必开 */
  xTaskCreate(gpio_task, "gpio", 0x100, NULL, 4, NULL);

  /* 陀螺仪接收处理 (UART3) */
  xTaskCreate(gyro_task, "gyro", 0x200, NULL, 4, NULL);

  /* OLED 显示 (200ms, 优先级 5) */
  xTaskCreate(oled_task, "oled", 0x100, NULL, 3, NULL);

  /* -------- FreeRTOS 测试任务 -------- */

  /* 双电机测试波形 — 正弦/方波/恒速（与 motor_task 二选一） */
  // xTaskCreate(motor_test_task, "motor_tst", 0x400, NULL, 6, NULL);

  /* CAN 总线 — 按需开关 */
  // xTaskCreate(can_send_test_task, "can_send", 0x200, NULL, configMAX_PRIORITIES - 2, NULL);
  // xTaskCreate(can_recv_isr_test_task, "can_recv", 0x200, NULL, configMAX_PRIORITIES - 2, NULL);

  /* 串口协议 — 独占 UART2 */
  // xTaskCreate(proto_send_test_task, "proto_send", 0x100, NULL, configMAX_PRIORITIES - 2, NULL);

  /* ---- 启动调度器（永不返回） ---- */
  vTaskStartScheduler();

  /* 理论上不会执行到这里 */
  while (1)
  {
  }
}
