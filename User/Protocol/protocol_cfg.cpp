/**
 * @file protocol_cfg.cpp
 * @brief 协议层总配置 — 全局实例定义 + 统一初始化
 */

#include "protocol_cfg.hpp"
#include "bsp_cfg.hpp"

/* ==================== 全局实例 ==================== */

ProtocolUartUser proto_uart;

/* ==================== 协议初始化 ==================== */

/** @brief 协议回调（由 proto_test.cpp 实现） */
extern void proto_on_command(uint8_t cmd, const uint8_t *data, uint8_t len);

/**
 * @brief 协议层统一初始化 — 绑定 UART 并注册回调
 *
 * 调用时机: bsp_init() 之后，vTaskStartScheduler() 之前。
 */
void protocol_init()
{
  proto_uart.init({&bsp_uart4, proto_on_command});
}
