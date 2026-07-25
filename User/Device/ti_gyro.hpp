/**
 * @file ti_gyro.hpp
 * @brief TI 陀螺仪协议封装。
 *
 * 该类负责将 TI 陀螺仪协议命令封装为 UART 帧，并提供帧校验与解析能力。
 */

#ifndef __TI_GYRO_HPP__
#define __TI_GYRO_HPP__

#include <stddef.h>
#include <stdint.h>

#include "bsp_uart.hpp"

class TiGyro
{
public:
  /**
   * @brief 协议配置参数。
   *
   * 默认设备 ID 为 0x60，广播地址 0xFF 不应使用。
   */
  struct Config
  {
    Config() : device_id(0x60U) {}
    uint8_t device_id;
  };

  /**
   * @brief 接收到的协议帧内容。
   */
  struct Frame
  {
    uint8_t dev_id;
    uint8_t cmd;
    uint8_t len;
    uint8_t data[32];

    Frame() : dev_id(0), cmd(0), len(0)
    {
      for (size_t i = 0; i < sizeof(data); ++i)
        data[i] = 0;
    }
  };

  /**
   * @brief 解析后的传感器数据。
   */
  struct ReportData
  {
    volatile bool     valid;
    volatile bool     ack_ok;
    volatile bool     is_attitude_only;
    volatile int16_t  acc_x;
    volatile int16_t  acc_y;
    volatile int16_t  acc_z;
    volatile int16_t  gyro_x;
    volatile int16_t  gyro_y;
    volatile int16_t  gyro_z;
    volatile int16_t  pitch_raw;
    volatile int16_t  roll_raw;
    volatile uint16_t yaw_raw;
    volatile float    pitch_deg;
    volatile float    roll_deg;
    volatile float    yaw_deg;
    Frame             frame;

    ReportData() : valid(false), ack_ok(false), is_attitude_only(false), acc_x(0), acc_y(0), acc_z(0), gyro_x(0), gyro_y(0), gyro_z(0), pitch_raw(0), roll_raw(0), yaw_raw(0), pitch_deg(0.0f), roll_deg(0.0f), yaw_deg(0.0f) {}
  };

  /**
   * @brief 协议命令码。
   */
  enum Command : uint8_t
  {
    /** @brief 0x0A：启动/停止数据上报。 */
    CMD_REPORT_CTRL = 0x0A,

    /** @brief 0x0B：设置输出模式。 */
    CMD_SET_MODE = 0x0B,

    /** @brief 0x10：触发重新校准。 */
    CMD_TRIGGER_CALIB = 0x10,

    /** @brief 0x12：软复位 MCU。 */
    CMD_SOFT_RESET = 0x12,

    /** @brief 0x14：Yaw 单圈比例校准。 */
    CMD_YAW_SCALE = 0x14,

    /** @brief 0xF0：通用指令应答。 */
    CMD_ACK = 0xF0,
  };

  /**
   * @brief 数据输出模式。
   */
  enum ReportMode : uint8_t
  {
    /** @brief 全数据模式：包含 Acc/Gyro/Pitch/Roll/Yaw。 */
    REPORT_MODE_FULL = 0x00U,

    /** @brief 仅姿态模式：仅输出 Yaw/Pitch/Roll。 */
    REPORT_MODE_ATTITUDE = 0x01U
  };

  TiGyro();
  ~TiGyro();

  /**
   * @brief 初始化串口句柄与默认配置。
   *
   * @param uart 指向底层串口驱动对象的指针。
   * @return true 表示初始化成功，false 表示串口指针为空。
   */
  bool init(BspUart<64> *uart);

  /**
   * @brief 发送启动/停止数据上报命令。
   *
   * @param enable true 为启动，false 为停止。
   * @return true 表示命令已成功发送。
   */
  bool start_report(bool enable);

  /**
   * @brief 设置输出模式。
   *
   * @param mode 输出模式，取值为 REPORT_MODE_FULL 或 REPORT_MODE_ATTITUDE。
   * @return true 表示命令已成功发送。
   */
  bool set_mode(uint8_t mode);

  /**
   * @brief 发送重新校准命令。
   *
   * @return true 表示命令已成功发送。
   */
  bool trigger_calibration();

  /**
   * @brief 发送软复位命令。
   *
   * @return true 表示命令已成功发送。
   */
  bool soft_reset();

  /**
   * @brief 发送 Yaw 单圈比例校准命令。
   *
   * @param actual_angle 实际测得角度。
   * @return true 表示命令已成功发送。
   */
  bool calibrate_yaw_scale(float actual_angle);

  /**
   * @brief 发送一条原始协议命令。
   *
   * @param cmd 命令码。
   * @param data 数据区内容，允许为 nullptr。
   * @param len 数据区长度。
   * @return true 表示命令已成功发送。
   */
  bool send_command(uint8_t cmd, const uint8_t *data = nullptr, size_t len = 0);

  /**
   * @brief 解析一帧协议数据。
   *
   * @param buf 指向原始字节流的指针。
   * @param size 缓冲区长度。
   * @param frame 输出解析后的帧对象。
   * @return true 表示帧合法且已成功解析。
   */
  bool parse_frame(const uint8_t *buf, size_t size, Frame &frame) const;

  /**
   * @brief 从串口接收缓冲区中读取并处理最新帧。
   *
   * @param timeout 等待接收数据的超时时间。
   * @return true 表示至少成功处理了一帧新数据。
   */
  bool update(uint32_t timeout = portMAX_DELAY);

  /**
   * @brief 获取最近一次已解析的原始协议帧。
   */
  const Frame &get_last_frame() const { return _last_frame; }

  /**
   * @brief 获取最近一次解析后的传感器数据。
   */
  const ReportData &get_last_report() const { return _last_report; }

  /**
   * @brief 获取最近一次命令是否收到应答成功。
   */
  bool is_last_ack_success() const { return _last_ack_ok; }

  /**
   * @brief 获取当前设备 ID。
   */
  uint8_t get_device_id() const { return _cfg.device_id; }

  /**
   * @brief 设置设备 ID。
   *
   * @param device_id 新的设备 ID。
   */
  void set_device_id(uint8_t device_id) { _cfg.device_id = device_id; }

private:
  static constexpr uint8_t FRAME_HEADER_1 = 0xAA;
  static constexpr uint8_t FRAME_HEADER_2 = 0x55;
  static constexpr size_t  MAX_FRAME_LEN  = 64;

  /**
   * @brief 计算校验和。
   *
   * @param data 待计算的数据指针。
   * @param len 数据长度。
   * @return 8 位校验和。
   */
  uint8_t checksum(const uint8_t *data, size_t len) const;

  /**
   * @brief 将 float32 数值编码为小端字节序。
   *
   * @param value 需要编码的浮点值。
   * @param buf 输出缓冲区，至少 4 字节。
   */
  void encode_float32_le(float value, uint8_t *buf) const;

  /**
   * @brief 解析上报数据帧。
   *
   * @param frame 输入协议帧。
   * @param report 输出解析后的上报数据。
   * @return true 表示解析成功。
   */
  bool parse_report_frame(const Frame &frame, ReportData &report) const;

  /**
   * @brief 从小端字节序读取 int16。
   */
  int16_t read_i16_le(const uint8_t *buf) const;

  /**
   * @brief 从小端字节序读取 uint16。
   */
  uint16_t read_u16_le(const uint8_t *buf) const;

  BspUart<64> *_uart = nullptr;
  Config       _cfg{};
  Frame        _last_frame{};
  ReportData   _last_report{};
  bool         _last_ack_ok   = false;
  uint8_t      _last_sent_cmd = 0;
  uint8_t      _rx_buffer[64]{};
  size_t       _rx_count = 0;
};

#endif
