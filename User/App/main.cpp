/**
 * @file main.cpp
 * @brief 主程序入口 — 硬件初始化 + 启动 init_task
 *
 * 初始化流程:
 *   1. SYSCFG_DL_init()  硬件初始化（SysConfig 生成：PLL 80MHz 时钟/GPIO/外设）
 *   2. bsp_init()        BSP 驱动对象绑定 + NVIC 使能（★必须在调度器启动前★）
 *   3. xTaskCreate(init_task, ...)  创建启动任务（优先级 7，最高）
 *   4. vTaskStartScheduler()       启动调度器（永不返回）
 *
 * init_task 运行时:
 *   → protocol_init() / device_init()  协议层+设备层初始化
 *   → 创建信号量 + 所有正式任务
 *   → vTaskDelete(NULL) 自删除
 *
 * @note bsp_init 必须在调度器启动前调用:
 *       其内部 NVIC_EnableIRQ 使能中断后，ISR 会调用 FreeRTOS API
 *       (xStreamBufferSendFromISR 等)，若调度器已在运行且 init_task
 *       正在半初始化状态，Cortex-M0+ 无 BASEPRI 保护会导致内核状态不一致 → HardFault
 */

#include "main.hpp"

/** @brief 脱线检测信号量（在 init_task 中创建，line_task 给出，line_off_task 接收） */
SemaphoreHandle_t g_line_off_sem = NULL;

/* ===================================================================
 * init_task() — 启动初始化任务
 * =================================================================== */

void init_task(void *arg)
{
  (void)arg;

  /* -------- 上层外设初始化（BSP 已在 main 中完成） -------- */
  protocol_init(); /* 协议层初始化 */
  vTaskDelay(2000);
  device_init(); /* 设备层初始化 */
  vTaskDelay(2000);

  /* -------- 创建任务间同步对象 -------- */
  g_line_off_sem = xSemaphoreCreateBinary();

  /* 菜单事件信号量初始化 (必须在所有任务之前) */
  menu_event_init();
  vTaskDelay(3000);

  /* -------- 创建 FreeRTOS 任务 -------- */

  /* 高频传感器 / 实时性高 */
  xTaskCreate(track_task, "track", 0x100, NULL, 7, NULL);
  xTaskCreate(motor_task, "motor", 0x200, NULL, 7, NULL);

  /* 控制任务 */
  // xTaskCreate(angle_task, "angle", 0x200, NULL, 6, NULL);

  // xTaskCreate(line_task, "line", 0x200, NULL, 6, NULL);
  // xTaskCreate(line_off_task, "line_off", 0x200, NULL, 6, NULL);

  /* 不重要但需要定期运行 */
  // xTaskCreate(motor_print_task, "motor_prt", 0x100, NULL, 5, NULL);
  xTaskCreate(gpio_task, "gpio", 0x100, NULL, 4, NULL);
  xTaskCreate(gyro_task, "gyro", 0x100, NULL, 4, NULL);
  xTaskCreate(menu_task, "menu", 0x200, NULL, 3, NULL);
  xTaskCreate(cpu_monitor_task, "cpu_mon", 0x100, NULL, 2, NULL);

  /* 消费菜单事件, 打印管理 */
  xTaskCreate(debug_print_task, "dbg_prt", 0x100, NULL, 3, NULL);

  /* -------- 自删除 -------- */
  vTaskDelete(NULL);
}

/* ===================================================================
 * main() — 系统入口
 * =================================================================== */

int main(void)
{
  /* -------- 硬件初始化（★必须在调度器启动前完成） -------- */
  SYSCFG_DL_init(); /* SysConfig 自动生成: 时钟 / GPIO / 外设 */
  bsp_init();       /* 驱动层绑定 + NVIC 使能（内部调用 xStreamBufferCreate 等 FreeRTOS API） */

  /* -------- 创建启动初始化任务（优先级 7，最高） -------- */
  xTaskCreate(init_task, "init", 0x400, NULL, 7, NULL);

  /* ---- 启动调度器（永不返回） ---- */
  vTaskStartScheduler();

  /* 理论上不会执行到这里 */
  while (1)
  {
  }
}
