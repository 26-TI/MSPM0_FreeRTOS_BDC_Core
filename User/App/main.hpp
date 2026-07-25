/**
 * @file main.hpp
 * @brief 主程序头文件 — 汇总所有需要的公共头文件
 *
 * @note 本文件被 main.cpp 引用。
 *       test.hpp 集中声明所有测试任务入口。
 */

#ifndef __MAIN_HPP__
#define __MAIN_HPP__

#include "ti_msp_dl_config.h" // IWYU pragma: keep

#include "FreeRTOS.h" // IWYU pragma: keep
#include "task.h"

#include "bsp_cfg.hpp"    // IWYU pragma: keep
#include "device_cfg.hpp" // IWYU pragma: keep
#include "test.hpp"       // IWYU pragma: keep

#endif /* __MAIN_HPP__ */