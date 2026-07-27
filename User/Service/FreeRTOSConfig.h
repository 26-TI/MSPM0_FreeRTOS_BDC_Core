/*
 * FreeRTOS Kernel V11.1.0
 * SPDX-License-Identifier: MIT
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 */

/*******************************************************************************
 * FreeRTOS 配置文件 —— 每个配置项均附中文说明
 * 在线文档: https://www.freertos.org/a00110.html
 ******************************************************************************/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/******************************************************************************/
/* 硬件时钟配置 */
/******************************************************************************/

/* CPU 时钟频率 (Hz)，需根据实际硬件设置，通常等于主系统时钟 */
#define configCPU_CLOCK_HZ ((unsigned long)80000000)

/* SysTick 时钟频率 (仅 ARM Cortex-M)，若 SysTick 与 CPU 同频则无需定义 */
/*
 #define configSYSTICK_CLOCK_HZ                  [Platform specific]
 */

/******************************************************************************/
/* 调度行为配置 */
/******************************************************************************/

/* 系统节拍频率 (Hz)，通常由 configCPU_CLOCK_HZ 计算得来 */
#define configTICK_RATE_HZ 1000

/* 1=抢占式调度，0=协作式调度 */
#define configUSE_PREEMPTION 1

/* 1=同优先级任务按时间片轮转，0=不自动切换 */
#define configUSE_TIME_SLICING 0

/* 1=使用硬件指令优化任务选择 (CLZ)，0=通用C算法 */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0

/* 1=启用低功耗无节拍模式，0=始终运行节拍中断 */
#define configUSE_TICKLESS_IDLE 0

/* 任务优先级数量 (0 ~ configMAX_PRIORITIES-1)，0为最低 */
#define configMAX_PRIORITIES 8

/* 空闲任务栈大小 (单位：字，非字节) */
#define configMINIMAL_STACK_SIZE 128

/* 任务名称最大长度 (含 '\0') */
#define configMAX_TASK_NAME_LEN 16

/* TickType_t 位宽: TICK_TYPE_WIDTH_16_BITS / 32_BITS / 64_BITS */
#define configTICK_TYPE_WIDTH_IN_BITS TICK_TYPE_WIDTH_32_BITS

/* 1=有空闲优先级任务就绪时，空闲任务主动让出 CPU */
#define configIDLE_SHOULD_YIELD 1

/* 每个任务的通知数组长度 */
#define configTASK_NOTIFICATION_ARRAY_ENTRIES 1

/* 队列注册表最大条目数 (仅内核调试器需要) */
#define configQUEUE_REGISTRY_SIZE 0

/* 1=兼容旧版 FreeRTOS 函数名/类型名 */
#define configENABLE_BACKWARD_COMPATIBILITY 0

/* 每个任务的线程本地存储指针数量 */
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 0

/* 1=使用精简列表项 (省 RAM，但可能违反严格别名规则) */
#define configUSE_MINI_LIST_ITEM 1

/* xTaskCreate() 中栈深度参数的类型 */
#define configSTACK_DEPTH_TYPE size_t

/* 消息缓冲区中消息长度的类型 */
#define configMESSAGE_BUFFER_LENGTH_TYPE size_t

/* 1=释放内存时清零，防止野指针 */
#define configHEAP_CLEAR_MEMORY_ON_FREE 1

/* vTaskList / vTaskGetRunTimeStats 缓冲区默认长度 */
#define configSTATS_BUFFER_MAX_LENGTH 0xFFFF

/* 1=为每个任务分配 newlib 重入结构体 */
#define configUSE_NEWLIB_REENTRANT 0

/******************************************************************************/
/* 软件定时器配置 */
/******************************************************************************/

/* 1=启用软件定时器功能 (需链接 timers.c) */
#define configUSE_TIMERS 1

/* 定时器服务任务的优先级 */
#define configTIMER_TASK_PRIORITY (configMAX_PRIORITIES - 1)

/* 定时器服务任务栈大小 (单位：字) */
#define configTIMER_TASK_STACK_DEPTH configMINIMAL_STACK_SIZE

/* 定时器命令队列长度 */
#define configTIMER_QUEUE_LENGTH 10

/******************************************************************************/
/* 事件组配置 */
/******************************************************************************/

/* 1=启用事件组功能 (需链接 event_groups.c) */
#define configUSE_EVENT_GROUPS 1

/******************************************************************************/
/* 流缓冲区配置 */
/******************************************************************************/

/* 1=启用流缓冲区功能 (需链接 stream_buffer.c) */
#define configUSE_STREAM_BUFFERS 1

/******************************************************************************/
/* 内存分配配置 */
/******************************************************************************/

/* 1=支持静态内存分配 (xTaskCreateStatic 等) */
#define configSUPPORT_STATIC_ALLOCATION 1

/* 1=支持动态内存分配 (xTaskCreate 等) */
#define configSUPPORT_DYNAMIC_ALLOCATION 1

/* FreeRTOS 堆总大小 (字节)，仅 heap_1/2/4.c 有效 */
#define configTOTAL_HEAP_SIZE (1024 * 16)

/* 1=由应用程序分配堆数组，0=由链接器分配 */
#define configAPPLICATION_ALLOCATED_HEAP 0

/* 1=任务栈从独立堆分配 (如快速内存)，需实现 pvPortMallocStack / vPortFreeStack */
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP 0

/* 1=启用堆保护 (heap_4/5.c 的指针越界检测与混淆) */
#define configENABLE_HEAP_PROTECTOR 0

/******************************************************************************/
/* 中断嵌套配置 */
/******************************************************************************/

/* 内核中断优先级 (SysTick / PendSV)，仅部分移植支持 */
#define configKERNEL_INTERRUPT_PRIORITY 0

/* 可安全调用 FreeRTOS API 的最高中断优先级 (高于此优先级的中断不会被内核屏蔽) */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 0

/* 同上，另一命名 (视移植而定) */
#define configMAX_API_CALL_INTERRUPT_PRIORITY 0

/******************************************************************************/
/* 钩子与回调函数配置 */
/******************************************************************************/

/* 空闲任务钩子 */
#define configUSE_IDLE_HOOK 0
/* 节拍中断钩子 */
#define configUSE_TICK_HOOK 1
/* 内存分配失败钩子 */
#define configUSE_MALLOC_FAILED_HOOK 0
/* 定时器服务任务启动钩子 */
#define configUSE_DAEMON_TASK_STARTUP_HOOK 0

/* 1=流/消息缓冲区支持收发完成回调 */
#define configUSE_SB_COMPLETED_CALLBACK 0

/* 栈溢出检测: 0=关闭, 1=快速检测, 2=模式检测 (较慢但更可靠) */
#define configCHECK_FOR_STACK_OVERFLOW 0

/******************************************************************************/
/* 运行统计与追踪配置 */
/******************************************************************************/

/* 1=收集任务运行时间统计 (需提供时钟源和 portCONFIGURE_TIMER / portGET_RUN_TIME_COUNTER_VALUE 宏) */
#define configGENERATE_RUN_TIME_STATS 1

/* 1=在 TCB 中包含额外的追踪/可视化信息 (vTaskGetRunTimeStats 依赖) */
#define configUSE_TRACE_FACILITY 1

/* 1=启用 vTaskList / vTaskGetRunTimeStats (依赖字符串格式化函数) */
#define configUSE_STATS_FORMATTING_FUNCTIONS 1

/* ── 运行时间计数器（基于 tick hook，tick=1000Hz 时等效 ~1MHz 分辨率）── */
extern volatile unsigned long CPU_RunTime;

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()  (CPU_RunTime = 0ul)
#define portGET_RUN_TIME_COUNTER_VALUE()           CPU_RunTime

/******************************************************************************/
/* 协程配置 */
/******************************************************************************/

/* 1=启用协程 (需链接 croutine.c) */
#define configUSE_CO_ROUTINES 0

/* 协程可用优先级数量 */
#define configMAX_CO_ROUTINE_PRIORITIES 1

/******************************************************************************/
/* 调试断言 */
/******************************************************************************/

/* 断言宏 —— 断言失败时关中断并死循环，便于调试器定位 */
#define configASSERT(x)       \
  if ((x) == 0)               \
  {                           \
    taskDISABLE_INTERRUPTS(); \
    for (;;);                 \
  }

/******************************************************************************/
/* MPU (内存保护单元) 配置 (仅 Cortex-M MPU 移植) */
/******************************************************************************/

/* 1=允许应用程序定义特权函数 */
#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS 0

/* MPU 区域数量 (通常 8 或 16) */
#define configTOTAL_MPU_REGIONS 8

/* Flash MPU 区域的 TEX/S/C/B 属性位 (默认 0x07: TEX=000, S=1, C=1, B=1) */
#define configTEX_S_C_B_FLASH 0x07UL

/* SRAM MPU 区域的 TEX/S/C/B 属性位 */
#define configTEX_S_C_B_SRAM 0x07UL

/* 1=仅允许内核代码发起系统调用，0=禁止非内核代码提权 */
#define configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY 1

/* 1=允许非特权任务进入临界区 (屏蔽中断) */
#define configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS 0

/* 0=使用 v2 MPU 封装 (mpu_wrappers_v2.c)，1=使用 v1 */
#define configUSE_MPU_WRAPPERS_V1 0

/* v2 MPU 封装: 内核对象池大小 (任务/队列/信号量/互斥量/事件组/定时器/流缓冲等总数) */
#define configPROTECTED_KERNEL_OBJECT_POOL_SIZE 10

/* v2 MPU 封装: 系统调用栈大小 (字)，每个任务一个 */
#define configSYSTEM_CALL_STACK_SIZE 128

/* v2 MPU 封装: 1=启用访问控制列表 (ACL)，非特权任务需显式授权才能访问内核对象 */
#define configENABLE_ACCESS_CONTROL_LIST 1

/******************************************************************************/
/* SMP (对称多处理) 配置 */
/******************************************************************************/

/* CPU 核心数量 (默认 1) */
/*
 #define configNUMBER_OF_CORES                     [Num of available cores]
 */

/* SMP 下: 0=仅同优先级任务可并行，1=不同优先级也可并行 */
#define configRUN_MULTIPLE_PRIORITIES 0

/* SMP 下: 1=启用核心亲和性 (vTaskCoreAffinitySet/Get) */
#define configUSE_CORE_AFFINITY 0

/* SMP + 核心亲和性: 默认核心亲和掩码 (tskNO_AFFINITY 表示不限) */
#define configTASK_DEFAULT_CORE_AFFINITY tskNO_AFFINITY

/* SMP 下: 1=允许单独设置任务为抢占/协作模式 */
#define configUSE_TASK_PREEMPTION_DISABLE 0

/* SMP 下: 1=启用被动空闲钩子 (无额外任务开销的后台功能) */
#define configUSE_PASSIVE_IDLE_HOOK 0

/* SMP 下: 定时器服务任务的核心亲和性 */
#define configTIMER_SERVICE_TASK_CORE_AFFINITY tskNO_AFFINITY


/******************************************************************************/
/* ARMv8-M 安全侧配置 */
/******************************************************************************/

/* 可调用安全侧的最大任务数 */
#define secureconfigMAX_SECURE_CONTEXTS 5

/* 1=由内核提供 Idle/Timer 任务内存 (vApplicationGetIdleTaskMemory 等) */
#define configKERNEL_PROVIDED_STATIC_MEMORY 1

/******************************************************************************/
/* ARMv8-M 移植配置 */
/******************************************************************************/

/* 1=启用 TrustZone (非安全侧运行 FreeRTOS 时) */
#define configENABLE_TRUSTZONE 1

/* 1=仅安全侧运行 (不使用 TrustZone，安全侧运行整个应用) */
#define configRUN_FREERTOS_SECURE_ONLY 1

/* 1=启用 MPU (内存保护单元) */
#define configENABLE_MPU 0

/* 1=启用 FPU (浮点单元) */
#define configENABLE_FPU 0

/* 1=启用 MVE (M-Profile 向量扩展，仅 Cortex-M55/M85) */
#define configENABLE_MVE 0

/******************************************************************************/
/* ARMv7-M / ARMv8-M 通用配置 */
/******************************************************************************/

/* 1=启用中断处理程序安装校验 (直接路由方式)；间接路由需设为 0 */
#define configCHECK_HANDLER_INSTALLATION 1

/******************************************************************************/
/* 功能开关 */
/******************************************************************************/

/* 任务通知 */
#define configUSE_TASK_NOTIFICATIONS 1
/* 互斥量 */
#define configUSE_MUTEXES 1
/* 递归互斥量 */
#define configUSE_RECURSIVE_MUTEXES 1
/* 计数信号量 */
#define configUSE_COUNTING_SEMAPHORES 1
/* 队列集 */
#define configUSE_QUEUE_SETS 0
/* 任务标签 (vTaskSetApplicationTaskTag) */
#define configUSE_APPLICATION_TASK_TAG 0

/* vTaskPrioritySet - 设置任务优先级 */
#define INCLUDE_vTaskPrioritySet 1
/* uxTaskPriorityGet - 获取任务优先级 */
#define INCLUDE_uxTaskPriorityGet 1
/* vTaskDelete - 删除任务 */
#define INCLUDE_vTaskDelete 1
/* vTaskSuspend - 挂起任务 */
#define INCLUDE_vTaskSuspend 1
/* xResumeFromISR - 从中断恢复任务 */
#define INCLUDE_xResumeFromISR 1
/* vTaskDelayUntil - 绝对延时 */
#define INCLUDE_vTaskDelayUntil 1
/* vTaskDelay - 相对延时 */
#define INCLUDE_vTaskDelay 1
/* xTaskGetSchedulerState - 获取调度器状态 */
#define INCLUDE_xTaskGetSchedulerState 1
/* xTaskGetCurrentTaskHandle - 获取当前任务句柄 */
#define INCLUDE_xTaskGetCurrentTaskHandle 1
/* uxTaskGetStackHighWaterMark - 获取栈高水位 */
#define INCLUDE_uxTaskGetStackHighWaterMark 0
/* xTaskGetIdleTaskHandle - 获取空闲任务句柄 */
#define INCLUDE_xTaskGetIdleTaskHandle 1
/* eTaskGetState - 获取任务状态 */
#define INCLUDE_eTaskGetState 0
/* xEventGroupSetBitFromISR - 从中断设置事件位 */
#define INCLUDE_xEventGroupSetBitFromISR 1
/* xTimerPendFunctionCall - 定时器回调调度 */
#define INCLUDE_xTimerPendFunctionCall 0
/* xTaskAbortDelay - 中止任务延时 */
#define INCLUDE_xTaskAbortDelay 0
/* xTaskGetHandle - 根据名称获取任务句柄 */
#define INCLUDE_xTaskGetHandle 0
/* xTaskResumeFromISR - 从中断恢复任务 */
#define INCLUDE_xTaskResumeFromISR 1

#endif /* FREERTOS_CONFIG_H */
