/**
 * @file protocol_uart_user.hpp
 * @brief 用户自定义串口帧协议 — 独占 UART + 回调函数指针
 *
 * @note 基于 example/protocol_uart 适配 MSPM0。
 *       独占一路 BspUart，init() 启动 FreeRTOS 解析任务，
 *       收到有效帧后调用用户注册的回调函数。
 *
 * @note 帧格式（与上位机兼容）:
 * @code
 *   Byte[0]=H1  Byte[1]=H2  Byte[2]=CMD  Byte[3]=LEN
 *   Byte[4..3+LEN]=DATA
 *   Byte[4+LEN]=SUM  累加校验 (H1+H2+CMD+LEN+DATA) & 0xFF
 *   Byte[5+LEN]=TAIL
 *
 *   最小帧 7B（LEN=0），最大帧 71B（LEN=64）
 * @endcode
 *
 * 使用示例:
 * @code
 *   void my_callback(uint8_t cmd, const uint8_t *d, uint8_t len) {
 *     bsp_uart4.uart_printf("CMD=0x%02X\r\n", cmd);
 *   }
 *   proto_uart.init({&bsp_uart4, my_callback});
 *   proto_uart.send(0x10, data, 2);
 * @endcode
 */

#ifndef __PROTOCOL_UART_USER_HPP__
#define __PROTOCOL_UART_USER_HPP__

#include <stdint.h>
#include <stddef.h>

/* 前向声明 */
template <size_t B>
class BspUart;

/* ===================================================================
 * 常量
 * =================================================================== */

#define PROTO_UART_DATA_MAX 64U ///< 单帧最大数据字节数
#define PROTO_UART_HDR1 0xAAU   ///< 默认帧头 1
#define PROTO_UART_HDR2 0x55U   ///< 默认帧头 2
#define PROTO_UART_TAIL 0x0DU   ///< 默认帧尾

/* ===================================================================
 * 回调类型
 * =================================================================== */

/**
 * @brief 收到有效帧时的回调函数指针
 * @param cmd  命令码
 * @param data 数据指针（仅在回调期间有效）
 * @param len  数据长度 (0~64)
 *
 * @note 在解析任务上下文中调用，不应执行耗时或阻塞操作。
 */
typedef void (*ProtoUartCallback)(uint8_t cmd, const uint8_t *data, uint8_t len);

/* ===================================================================
 * ProtocolUartUser — 用户自定义串口帧协议
 * =================================================================== */

class ProtocolUartUser
{
public:
  /** @brief 初始化配置 */
  struct Config
  {
    BspUart<64>      *uart;                      ///< 独占的串口指针
    ProtoUartCallback on_cmd   = nullptr;         ///< 收到有效帧的回调
    uint8_t           header1 = PROTO_UART_HDR1; ///< 帧头 1
    uint8_t           header2 = PROTO_UART_HDR2; ///< 帧头 2
    uint8_t           tail    = PROTO_UART_TAIL; ///< 帧尾
  };

  /**
   * @brief 初始化协议，启动解析任务
   * @param cfg 串口 + 回调 + 帧格式
   *
   * @note 创建 FreeRTOS 任务独占该 UART，
   *       系统阻塞等待帧头，收到有效帧后调 onCmd。
   *       任务名自动编号 proto_uart0/1/2...，支持多实例。
   */
  void init(const Config &cfg);

  /**
   * @brief 运行时更换回调（线程安全由调用者保证）
   * @param cb 新回调，nullptr 禁用接收
   */
  void set_callback(ProtoUartCallback cb) { _on_cmd = cb; }

  /**
   * @brief 打包并发送一帧（累加校验自动计算）
   * @param cmd  命令码
   * @param data 数据指针
   * @param len  长度 0~64
   * @return true  发送成功
   * @return false TX 缓冲满（50ms 超时）或参数非法
   */
  bool send(uint8_t cmd, const uint8_t *data, uint8_t len);

  /** @brief 获取绑定的串口指针 */
  BspUart<64> *get_uart() const { return _uart; }

private:
  /** @brief 解析任务入口（静态，传 this） */
  static void _task_entry(void *arg);

  /** @brief 累加校验 @return SUM(buf[0..len-1]) & 0xFF */
  static uint8_t _checksum(const uint8_t *buf, uint8_t len);

  BspUart<64>      *_uart    = nullptr; ///< 绑定的串口
  ProtoUartCallback _on_cmd   = nullptr; ///< 用户回调
  uint8_t           _header1 = PROTO_UART_HDR1;
  uint8_t           _header2 = PROTO_UART_HDR2;
  uint8_t           _tail    = PROTO_UART_TAIL;
};

#endif /* __PROTOCOL_UART_USER_HPP__ */
