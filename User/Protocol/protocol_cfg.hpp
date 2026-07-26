/**
 * @file protocol_cfg.hpp
 * @brief 协议层总配置 — 全局实例声明与初始化入口
 */

#ifndef __PROTOCOL_CFG_HPP__
#define __PROTOCOL_CFG_HPP__

#include "protocol_uart_user.hpp"

/* ==================== 函数声明 ==================== */

/**
 * @brief 协议层初始化 — 绑定串口与回调
 *
 * @note 必须在 bsp_init() 之后、FreeRTOS 调度器启动之前调用。
 */
void protocol_init();

/* ==================== 全局声明 ==================== */

extern ProtocolUartUser proto_uart; ///< 用户串口协议实例

#endif // __PROTOCOL_CFG_HPP__
