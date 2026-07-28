/**
 * @file line_off_task.cpp
 * @brief 脱线处理任务 — 永久阻塞等待脱线信号量
 *
 * 当循线任务检测到连续 100ms 丢线时，通过二值信号量通知本任务。
 * 本任务负责执行脱线后的保护动作（停车、报警等）。
 */

#include "task.hpp"
#include "device_cfg.hpp"

#include "FreeRTOS.h" // IWYU pragma: keep
#include "task.h"
#include "semphr.h"

/* ===================================================================
 * 脱线处理任务 (优先级 6, 永久阻塞等待信号量)
 * =================================================================== */

void line_off_task(void *arg)
{
  (void)arg;

  while (1)
  {
    /* 永久阻塞等待脱线信号量（不占 CPU） */
    xSemaphoreTake(g_line_off_sem, portMAX_DELAY);
    /* ── 脱线处理: 紧急停车 ── */
    motor_left.set_speed(0.0f);
    motor_right.set_speed(0.0f);

    vTaskDelay(pdMS_TO_TICKS(10));

    /* TODO: 在此添加更多脱线处理逻辑
       *   - 蜂鸣器报警
       *   - LED 指示
       *   - 串口上报脱线事件
       *   - 进入脱线恢复模式
       */
  }
}
