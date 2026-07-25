/**
 * @file ti_gyro.cpp
 * @brief TI 陀螺仪协议封装实现。
 */

#include "ti_gyro.hpp"

#include <string.h>

TiGyro::TiGyro()  = default;
TiGyro::~TiGyro() = default;

/**
 * @brief 初始化底层串口句柄。
 *
 * @param uart 指向 UART 驱动对象的指针。
 * @return true 表示初始化成功，false 表示传入为空。
 */
bool TiGyro::init(BspUart<64> *uart)
{
  _uart = uart;
  return _uart != nullptr;
}

/**
 * @brief 发送启动/停止数据上报命令。
 *
 * @param enable true 为启动，false 为停止。
 * @return true 表示命令发送成功。
 */
bool TiGyro::start_report(bool enable)
{
  /* 0x0A: Report Ctrl
   * DATA[0] = 0x01 -> 启动数据上报
   * DATA[0] = 0x00 -> 停止数据上报
   */
  uint8_t data = enable ? 0x01U : 0x00U;
  return send_command(CMD_REPORT_CTRL, &data, 1);
}

/**
 * @brief 设置输出模式。
 *
 * @param mode 输出模式，0x00 为全数据模式，0x01 为仅姿态模式。
 * @return true 表示命令发送成功。
 */
bool TiGyro::set_mode(uint8_t mode)
{
  /* 0x0B: Set Mode
   * 0x00 -> 全数据模式（Acc/Gyro/Pitch/Roll/Yaw）
   * 0x01 -> 仅姿态模式（Yaw/Pitch/Roll）
   */
  return send_command(CMD_SET_MODE, &mode, 1);
}

/**
 * @brief 发送重新校准命令。
 *
 * @return true 表示命令发送成功。
 */
bool TiGyro::trigger_calibration()
{
  /* 0x10: Trigger Calib
   * 无数据区，触发重新校准。
   */
  return send_command(CMD_TRIGGER_CALIB);
}

/**
 * @brief 发送 MCU 软复位命令。
 *
 * @return true 表示命令发送成功。
 */
bool TiGyro::soft_reset()
{
  /* 0x12: Soft Reset
   * 触发 MCU 软复位，串口通信可能短暂中断。
   */
  return send_command(CMD_SOFT_RESET);
}

/**
 * @brief 发送 Yaw 单圈比例校准命令。
 *
 * @param actual_angle 实际测得角度。
 * @return true 表示命令发送成功。
 */
bool TiGyro::calibrate_yaw_scale(float actual_angle)
{
  /* 0x14: Cal Yaw Scale
   * DATA 为 float32 小端编码的实际测得角度。
   * 例如写入 360.0 度时，实际角度应该为 360.0f。
   */
  uint8_t data[4]{};
  encode_float32_le(actual_angle, data);
  return send_command(CMD_YAW_SCALE, data, 4);
}

/**
 * @brief 发送一条完整协议命令。
 *
 * @param cmd 命令码。
 * @param data 数据区内容。
 * @param len 数据区长度。
 * @return true 表示命令发送成功。
 */
bool TiGyro::send_command(uint8_t cmd, const uint8_t *data, size_t len)
{
  if (!_uart)
    return false;

  uint8_t frame[1 + 1 + 1 + 1 + 32 + 1]{};
  size_t  pos  = 0;
  frame[pos++] = FRAME_HEADER_1;
  frame[pos++] = FRAME_HEADER_2;
  frame[pos++] = _cfg.device_id;
  frame[pos++] = cmd;
  frame[pos++] = static_cast<uint8_t>(len);

  if (data && len)
    memcpy(frame + pos, data, len);

  pos += len;
  frame[pos++] = checksum(frame + 2, 1 + 1 + 1 + len);

  _last_sent_cmd = cmd;
  _last_ack_ok   = false;

  return _uart->send(frame, pos, portMAX_DELAY) == pos;
}

/**
 * @brief 解析一帧协议数据。
 *
 * @param buf 原始字节流缓冲区。
 * @param size 缓冲区长度。
 * @param frame 输出解析结果。
 * @return true 表示帧合法且已解析。
 */
bool TiGyro::parse_frame(const uint8_t *buf, size_t size, Frame &frame) const
{
  if (!buf || size < 6)
    return false;

  if (buf[0] != FRAME_HEADER_1 || buf[1] != FRAME_HEADER_2)
    return false;

  size_t data_len  = buf[4];
  size_t frame_len = 6 + data_len;
  if (size < frame_len)
    return false;

  if (checksum(buf + 2, 1 + 1 + 1 + data_len) != buf[frame_len - 1])
    return false;

  frame.dev_id = buf[2];
  frame.cmd    = buf[3];
  frame.len    = static_cast<uint8_t>(data_len);
  memset(frame.data, 0, sizeof(frame.data));
  memcpy(frame.data, buf + 5, data_len);
  return true;
}

/**
 * @brief 计算校验和。
 *
 * @param data 参与计算的数据。
 * @param len 数据长度。
 * @return 8 位校验和。
 */
uint8_t TiGyro::checksum(const uint8_t *data, size_t len) const
{
  uint32_t sum = 0;
  for (size_t i = 0; i < len; ++i)
    sum += data[i];
  return static_cast<uint8_t>(sum & 0xFFU);
}

/**
 * @brief 将 float32 值编码为小端字节序。
 *
 * @param value 输入浮点值。
 * @param buf 输出缓冲区，至少 4 字节。
 */
void TiGyro::encode_float32_le(float value, uint8_t *buf) const
{
  union
  {
    float    f;
    uint32_t u;
  } converter;

  converter.f = value;
  buf[0]      = static_cast<uint8_t>(converter.u & 0xFFU);
  buf[1]      = static_cast<uint8_t>((converter.u >> 8) & 0xFFU);
  buf[2]      = static_cast<uint8_t>((converter.u >> 16) & 0xFFU);
  buf[3]      = static_cast<uint8_t>((converter.u >> 24) & 0xFFU);
}

bool TiGyro::update(uint32_t timeout)
{
  if (!_uart)
    return false;

  uint8_t buf[64]{};
  size_t  n = _uart->receive(buf, sizeof(buf), timeout);
  if (!n)
    return false;

  bool processed = false;
  for (size_t i = 0; i < n; ++i)
  {
    if (_rx_count == 0 && buf[i] != FRAME_HEADER_1)
      continue;

    if (_rx_count == 1 && buf[i] != FRAME_HEADER_2)
    {
      _rx_count = 0;
      continue;
    }

    _rx_buffer[_rx_count++] = buf[i];

    if (_rx_count < 6)
      continue;

    size_t data_len  = _rx_buffer[4];
    size_t frame_len = 6 + data_len;
    if (_rx_count < frame_len)
      continue;

    Frame frame;
    if (parse_frame(_rx_buffer, frame_len, frame))
    {
      _last_frame        = frame;
      _last_report.valid = false;
      if (frame.cmd == CMD_ACK)
      {
        _last_ack_ok        = (frame.len >= 1 && frame.data[0] == _last_sent_cmd);
        _last_report.ack_ok = _last_ack_ok;
      }
      else if (frame.cmd == 0x01U)
      {
        parse_report_frame(frame, _last_report);
        _last_report.ack_ok = false;
      }
      processed = true;
    }

    _rx_count = 0;
  }

  return processed;
}

bool TiGyro::parse_report_frame(const Frame &frame, ReportData &report) const
{
  report.valid  = false;
  report.frame  = frame;
  report.ack_ok = false;

  if (frame.cmd != 0x01U)
    return false;

  if (frame.len == 18)
  {
    report.valid            = true;
    report.is_attitude_only = false;
    report.acc_x            = read_i16_le(frame.data + 0);
    report.acc_y            = read_i16_le(frame.data + 2);
    report.acc_z            = read_i16_le(frame.data + 4);
    report.gyro_x           = read_i16_le(frame.data + 6);
    report.gyro_y           = read_i16_le(frame.data + 8);
    report.gyro_z           = read_i16_le(frame.data + 10);
    report.pitch_raw        = read_i16_le(frame.data + 12);
    report.roll_raw         = read_i16_le(frame.data + 14);
    report.yaw_raw          = read_u16_le(frame.data + 16);
    report.pitch_deg        = static_cast<float>(report.pitch_raw) / 100.0f;
    report.roll_deg         = static_cast<float>(report.roll_raw) / 100.0f;
    report.yaw_deg          = static_cast<float>(report.yaw_raw) / 100.0f;
    return true;
  }

  if (frame.len == 6)
  {
    report.valid            = true;
    report.is_attitude_only = true;
    report.yaw_raw          = read_u16_le(frame.data + 0);
    report.pitch_raw        = read_i16_le(frame.data + 2);
    report.roll_raw         = read_i16_le(frame.data + 4);
    report.pitch_deg        = static_cast<float>(report.pitch_raw) / 100.0f;
    report.roll_deg         = static_cast<float>(report.roll_raw) / 100.0f;
    report.yaw_deg          = static_cast<float>(report.yaw_raw) / 100.0f;
    return true;
  }

  return false;
}

int16_t TiGyro::read_i16_le(const uint8_t *buf) const
{
  return static_cast<int16_t>(static_cast<uint16_t>(buf[0]) | (static_cast<uint16_t>(buf[1]) << 8));
}

uint16_t TiGyro::read_u16_le(const uint8_t *buf) const
{
  return static_cast<uint16_t>(buf[0] | (static_cast<uint16_t>(buf[1]) << 8));
}
