/**
 * @file cpu_monitor.cpp
 * @brief CPU idle % — tick hook counter + uxTaskGetSystemState
 *
 * @note Requires FreeRTOSConfig.h:
 *       - configUSE_TICK_HOOK=1
 *       - configGENERATE_RUN_TIME_STATS=1
 *       - configUSE_TRACE_FACILITY=1
 */

#include "task.hpp"
#include "bsp_cfg.hpp"

#include "FreeRTOS.h"
#include "task.h"

/* ===================================================================
 * Run-time counter (tick-hook driven, ~1 MHz effective)
 * =================================================================== */

volatile unsigned long CPU_RunTime = 0UL;

extern "C" void vApplicationTickHook(void)
{
  CPU_RunTime += 1000;
}

/* ===================================================================
 * CPU idle monitor
 * =================================================================== */

#define MONITOR_PERIOD_MS 500
#define MAX_TASKS 16

unsigned long idle_pct = 0;

void cpu_monitor_task(void *arg)
{
  (void)arg;
  vTaskDelay(pdMS_TO_TICKS(3000));

  static TaskStatus_t xTaskArray[MAX_TASKS];
  unsigned long       ulPrevIdle  = 0;
  unsigned long       ulPrevTotal = 0;

  while (1)
  {
    UBaseType_t   uxCount;
    unsigned long ulTotalTime;

    uxCount = uxTaskGetSystemState(xTaskArray, MAX_TASKS, &ulTotalTime);

    /* find the idle task */
    unsigned long ulIdleTime = 0;
    TaskHandle_t  xIdle      = xTaskGetIdleTaskHandle();
    for (UBaseType_t i = 0; i < uxCount; i++)
    {
      if (xTaskArray[i].xHandle == xIdle)
      {
        ulIdleTime = xTaskArray[i].ulRunTimeCounter;
        break;
      }
    }

    /* deltas */
    unsigned long idle_delta  = ulIdleTime - ulPrevIdle;
    unsigned long total_delta = ulTotalTime - ulPrevTotal;
    ulPrevIdle                = ulIdleTime;
    ulPrevTotal               = ulTotalTime;

    idle_pct = (total_delta > 0)
                 ? (idle_delta * 100 / total_delta)
                 : 100;


    vTaskDelay(pdMS_TO_TICKS(MONITOR_PERIOD_MS));
  }
}
