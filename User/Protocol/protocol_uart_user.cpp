/**
 * @file protocol_uart_user.cpp
 * @brief 用户自定义串口帧协议 — 帧同步 + 累加校验 + 解析任务
 *
 * @note 解析任务流程:
 * @code
 *   while(1):
 *     1. receive(&hdr1, 1, MAX)       系统阻塞等帧头 1
 *     2. hdr1 不匹配 → continue
 *     3. receive(&hdr2, 1, 100ms)     等帧头 2
 *     4. hdr2 不匹配 → goto 1
 *     5. receive(&cmd, 1, 100ms)      等 CMD
 *     6. receive(&len, 1, 100ms)      等 LEN
 *     7. len > 64 → goto 1
 *     8. receive(buf, len+2, 100ms)   等 DATA+SUM+TAIL
 *     9. SUM==calc && TAIL==cfg.tail → _onCmd(cmd, buf, len)
 *    10. 失败 → goto 1
 * @endcode
 *
 * @note 纯系统阻塞操作，无裸机等待，无中断依赖。
 */

#include "protocol_uart_user.hpp"
#include "bsp_uart.hpp"

#include "task.h"
#include <string.h>
#include <stdio.h>

/* ================================================================
 *  初始化
 * ================================================================ */

/** @brief 自增实例 ID，用于生成唯一任务名 */
static uint8_t g_proto_uart_inst_id = 0U;

/**
 * @brief 保存配置，启动 FreeRTOS 解析任务
 * @param cfg 串口 + 回调 + 帧格式
 */
void ProtocolUartUser::init(const Config &cfg)
{
  _uart    = cfg.uart;
  _on_cmd   = cfg.on_cmd;
  _header1 = cfg.header1;
  _header2 = cfg.header2;
  _tail    = cfg.tail;

  /* 任务名自动编号: proto_uart0, proto_uart1, ... */
  char name[16];
  snprintf(name, sizeof(name), "proto_uart%u", g_proto_uart_inst_id++);

  xTaskCreate(_task_entry, name, 256U, this,
              tskIDLE_PRIORITY + 2U, nullptr);
}

/* ================================================================
 *  发送
 * ================================================================ */

/**
 * @brief 打包 [H1 H2 CMD LEN DATA... SUM TAIL] 并发送
 * @param cmd  命令码
 * @param data 数据
 * @param len  长度 0~64
 * @return true  发送成功（6+len 字节全部写入 TX 流缓冲）
 * @return false 参数非法或 TX 流缓冲满（50ms 超时）
 */
bool ProtocolUartUser::send(uint8_t cmd, const uint8_t *data, uint8_t len)
{
  if (!_uart || len > PROTO_UART_DATA_MAX)
  {
    return false;
  }

  uint8_t buf[7U + PROTO_UART_DATA_MAX]; /* 最大 71 字节，放栈上 */
  buf[0] = _header1;
  buf[1] = _header2;
  buf[2] = cmd;
  buf[3] = len;

  if (len > 0U && data)
  {
    memcpy(&buf[4U], data, len);
  }

  buf[4U + len] = _checksum(buf, (uint8_t)(4U + len));
  buf[5U + len] = _tail;

  size_t total = (size_t)(6U + len);
  size_t n     = _uart->send(buf, total, pdMS_TO_TICKS(50));
  return (n == total);
}

/* ================================================================
 *  累加校验
 * ================================================================ */

/** @brief SUM(buf[0..len-1]) & 0xFF */
uint8_t ProtocolUartUser::_checksum(const uint8_t *buf, uint8_t len)
{
  uint8_t sum = 0U;
  for (uint8_t i = 0U; i < len; i++)
  {
    sum += buf[i];
  }
  return sum;
}

/* ================================================================
 *  解析任务（静态函数，this 指针通过 arg 传入）
 * ================================================================ */

/**
 * @brief 帧解析任务入口
 *
 * 帧同步策略:
 *   - 系统阻塞等第 1 字节（portMAX_DELAY，任务挂起）
 *   - 匹配帧头 1 后以 100ms 超时读后续字段
 *   - 超时或格式错误 → 丢弃当前帧，重新同步
 *
 * @param arg ProtocolUartUser 实例的 this 指针
 */
void ProtocolUartUser::_task_entry(void *arg)
{
  ProtocolUartUser *self = static_cast<ProtocolUartUser *>(arg);

  while (1)
  {
    uint8_t hdr[4U];                       /* H1, H2, CMD, LEN */
    uint8_t buf[PROTO_UART_DATA_MAX + 2U]; /* DATA + SUM + TAIL */

    /* ---- 1. 系统阻塞等帧头 1 ---- */
    if (self->_uart->receive(&hdr[0], 1U, portMAX_DELAY) == 0U)
    {
      continue;
    }
    if (hdr[0] != self->_header1)
    {
      continue;
    }

    /* ---- 2. 帧头 2（100ms 超时） ---- */
    if (self->_uart->receive(&hdr[1], 1U, pdMS_TO_TICKS(100)) == 0U)
    {
      continue;
    }
    if (hdr[1] != self->_header2)
    {
      continue;
    }

    /* ---- 3. CMD（100ms 超时） ---- */
    if (self->_uart->receive(&hdr[2], 1U, pdMS_TO_TICKS(100)) == 0U)
    {
      continue;
    }

    /* ---- 4. LEN（100ms 超时） ---- */
    if (self->_uart->receive(&hdr[3], 1U, pdMS_TO_TICKS(100)) == 0U)
    {
      continue;
    }

    uint8_t cmd = hdr[2];
    uint8_t len = hdr[3];

    /* ---- 5. 长度合法性 ---- */
    if (len > PROTO_UART_DATA_MAX)
    {
      continue;
    }

    /* ---- 6. DATA + SUM + TAIL ---- */
    uint8_t remaining = len + 2U;
    if (self->_uart->receive(buf, remaining, pdMS_TO_TICKS(100))
        < (size_t)remaining)
    {
      continue;
    }

    /* ---- 7. 校验 ---- */
    uint8_t calc_sum = hdr[0] + hdr[1] + hdr[2] + hdr[3];
    for (uint8_t i = 0U; i < len; i++)
    {
      calc_sum += buf[i];
    }

    uint8_t rx_sum  = buf[len];
    uint8_t rx_tail = buf[len + 1U];

    if (calc_sum == rx_sum && rx_tail == self->_tail)
    {
      /* 有效帧 → 用户回调 */
      if (self->_on_cmd)
      {
        self->_on_cmd(cmd, buf, len);
      }
    }
    /* 校验失败 → 静默丢弃，继续等下帧 */
  }
}
