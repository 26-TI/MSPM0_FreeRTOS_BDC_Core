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
 *   - gpio_test.cpp    GPIO/按键（LED 闪烁 + 按键打印）
 *   - can_send_test.cpp CAN 非阻塞发送
 *   - can_recv_test.cpp CAN 接收（轮询 + ISR）
 *   - relay_test.cpp   继电器控制
 *   - pwm_test.cpp     PWM 占空比测试
 *   - gyro_test.cpp    陀螺仪姿态测试
 *   - track_test.cpp   巡线传感器测试
 *   - oled_test.cpp    OLED 显示测试
 *   - motor_proto_test.cpp  电机can测试
 *
 * @note 开关测试任务: 在 main() 中注释/取消注释对应的 xTaskCreate() 即可。
 */

#include "main.hpp"

/* ===================================================================
 * main() — 系统入口
 * =================================================================== */

int main(void) 
{
  /* ---- 硬件初始化 ---- */
  SYSCFG_DL_init(); /* SysConfig 自动生成: 时钟 / GPIO / 外设 */
  bsp_init();       /* BSP 驱动对象绑定到硬件实例 */
  // device_init();     /* 设备层初始化（按需启用） */

  /* ---- 创建 FreeRTOS 任务 ---- */

  // /* GPIO / 按键 — 必开 */
  xTaskCreate(gpio_test_task, "gpio", 0x100, NULL, configMAX_PRIORITIES - 3, NULL);
  // xTaskCreate(relay_test_task, "relay", 0x100, NULL, configMAX_PRIORITIES -
  // 3, NULL);

  /* CAN 电机协议测试 — 回环模式，无需外部硬件 */
  // xTaskCreate(motor_proto_test_task, "motor_proto", 0x200, NULL,
  // configMAX_PRIORITIES - 2, NULL);

  /* CAN 总线 — 按需开关 */
  // xTaskCreate(can_send_test_task, "can_send", 0x200, NULL,
  // configMAX_PRIORITIES - 2, NULL);
  // xTaskCreate(can_recv_isr_test_task, "can_recv", 0x200, NULL, configMAX_PRIORITIES - 2, NULL);

  // /* 串口协议 — 独占 UART2 */
  // xTaskCreate(proto_send_test_task, "proto_send", 0x100, NULL,
  // configMAX_PRIORITIES - 2, NULL);

  /* PWM 测试 */
  // xTaskCreate(pwm_test_task,   "pwm",   0x200, NULL, configMAX_PRIORITIES -
  // 2, NULL);

  /* 陀螺仪 — 需外接 IMU 硬件 */
  // xTaskCreate(gyro_test_task,  "gyro",  0x200, NULL, configMAX_PRIORITIES -
  // 3, NULL);

  /* 巡线 + OLED — 需外接 PCA9555 + OLED 硬件 */
  // xTaskCreate(track_test_task, "track", 0x100, NULL, configMAX_PRIORITIES -
  // 1, NULL); xTaskCreate(oled_test_task,  "oled",  0x200, NULL,
  // configMAX_PRIORITIES - 2, NULL);

  /* ---- 启动调度器（永不返回） ---- */
  vTaskStartScheduler();

  /* 理论上不会执行到这里 */
  while (1) {
  }
}
