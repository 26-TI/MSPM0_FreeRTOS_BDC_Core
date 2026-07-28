/**
 * @file cpu_monitor.cpp
 * @brief CPU 空闲率监控 — 基于 FreeRTOS 运行时间统计
 *
 * 依赖 FreeRTOSConfig.h:
 *   - configUSE_TICK_HOOK = 1
 *   - configGENERATE_RUN_TIME_STATS = 1
 *   - configUSE_TRACE_FACILITY = 1
 *   - configUSE_STATS_FORMATTING_FUNCTIONS = 1
 *
 * 原理:
 *   tick-hook 每 tick 累加 CPU_RunTime（分辨率 = tick 频率），
 *   空闲任务运行时此计数器也在累加 → 空闲率 = 空闲任务时间 / 总时间。
 *
 * 开销:
 *   - tick-hook 仅一条加法，几乎为零
 *   - 监控任务每 500ms 调用一次 uxTaskGetSystemState，遍历任务数组
 *   - 优先级 1（仅高于空闲任务），不影响实时任务
 */

#include "task.hpp"

#include "FreeRTOS.h"
#include "task.h"

/* ===================================================================
 * 运行时间计数器 (tick-hook 驱动)
 * =================================================================== */

volatile unsigned long CPU_RunTime = 0UL;

extern "C" void vApplicationTickHook(void)
{
  CPU_RunTime += 1000; /* 每 tick +1000，与 1000Hz tick 同步 */
}

/* ===================================================================
 * CPU 空闲率监控
 * =================================================================== */

#define MONITOR_PERIOD_MS 1000
#define MAX_TASKS          16

/* 全局变量，供调试器查看 */
unsigned long g_idle_pct = 0;

void cpu_monitor_task(void *arg)
{
  (void)arg;

  /* 启动后等 1 秒让系统稳定 */
  vTaskDelay(pdMS_TO_TICKS(1000));

  static TaskStatus_t xTaskArray[MAX_TASKS];
  unsigned long       ulPrevIdle  = 0;
  unsigned long       ulPrevTotal = 0;

  while (1)
  {
    /* ── 获取所有任务快照 ── */
    uint32_t    ulTotalTime = 0;
    UBaseType_t uxCount     = uxTaskGetSystemState(xTaskArray, MAX_TASKS, &ulTotalTime);

    /* ── 找到空闲任务（优先级 0, 名称 "IDLE"） ── */
    unsigned long ulIdleTime = 0;
    for (UBaseType_t i = 0; i < uxCount; i++)
    {
      if (xTaskArray[i].uxCurrentPriority == 0U)
      {
        ulIdleTime = xTaskArray[i].ulRunTimeCounter;
        break;
      }
    }

    /* ── 计算增量 → 空闲率百分比 ── */
    unsigned long idle_delta  = ulIdleTime - ulPrevIdle;
    unsigned long total_delta = ulTotalTime - ulPrevTotal;
    ulPrevIdle                = ulIdleTime;
    ulPrevTotal               = ulTotalTime;

    g_idle_pct = (total_delta > 0) ? (idle_delta * 100 / total_delta) : 100;

    /* ── 可选: 输出各任务 CPU 占用（取消注释以启用） ── */
    // bsp_uart4.uart_printf("[CPU] idle=%lu%% tasks=%lu\r\n", g_idle_pct, (unsigned long)uxCount);
    // for (UBaseType_t i = 0; i < uxCount; i++)
    // {
    //   unsigned long pct = (total_delta > 0)
    //     ? (xTaskArray[i].ulRunTimeCounter * 100 / total_delta)
    //     : 0;
    //   bsp_uart4.uart_printf("  %-16s pri=%lu cpu=%lu%%\r\n",
    //     xTaskArray[i].pcTaskName,
    //     (unsigned long)xTaskArray[i].uxCurrentPriority,
    //     pct);
    // }

    vTaskDelay(pdMS_TO_TICKS(MONITOR_PERIOD_MS));
  }
}
