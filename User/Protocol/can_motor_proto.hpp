/**
 * @file can_motor_proto.hpp
 * @brief 统一 CAN 电机帧协议 — 步进 + DC 减速电机
 *
 * 纯协议层，不含设备依赖。所有编解码为静态方法。
 * 纯协议层，不含设备依赖。所有编解码为静态方法。
 */

#ifndef __CAN_MOTOR_PROTO_HPP__
#define __CAN_MOTOR_PROTO_HPP__

#include <stdint.h>
#include <stdbool.h>

class CanMotorProto
{
public:
  /* ---- 步进指令码 ---- */
  enum StepCmd : uint8_t
  {
    STEP_SPEED    = 0x01, ///< 恒速转动,        p1=rpm×10
    STEP_POSITION = 0x02, ///< 角度定位,        p1=角度°×100, p2=圈数, p3=限速rpm
    STEP_STOP     = 0x03, ///< 停止
    STEP_ENABLE   = 0x04, ///< 使能/失能,       p1=1使能 0失能
    STEP_QUERY    = 0x05, ///< 查询状态
    STEP_TIMED    = 0x06, ///< 定时转动,        p1=角度°×100, p2=时长ms
  };

  /* ---- DC 指令码 ---- */
  enum DcCmd : uint8_t
  {
    DC_COAST      = 0x03, ///< 失能-滑行 (停PWM+停PID)
    DC_DRIVE      = 0x04, ///< 单独使能驱动-阻尼 (启PWM, 阻尼态)
    DC_SPEED_LOOP = 0x05, ///< 使能速度环 (启PID)
    DC_SET_SPEED  = 0x06, ///< 设置速度, p1=rpm×100
    DC_QUERY      = 0x07, ///< 回读电机状态
  };

  /* ---- 步进 QUERY 子码 ---- */
  enum QueryCode : uint8_t
  {
    Q_ALL   = 0x00,
    Q_ANGLE = 0x01,
    Q_SPEED = 0x02,
    Q_TURNS = 0x03,
  };

  /* ---- 状态帧 ---- */
  struct StepperStatus
  {
    uint8_t state = 0; ///< 0=空闲 1=运动中 2=已到位 3=故障
    float   angle = 0; ///< 当前角度 °
    float   rpm   = 0; ///< 当前转速 RPM
    int16_t turns = 0; ///< 累计圈数
    bool    is_done() const { return state == 2; }
    bool    is_moving() const { return state == 1; }
  };

  struct DcMotorStatus
  {
    float rpm       = 0; ///< 当前转速 RPM
    float current_a = 0; ///< 板电流 A
    float voltage_v = 0; ///< 板电压 V
  };

  /* ---- CAN ID ---- */
  static uint32_t dc_cmd_id(uint8_t n) { return 0x100 + n; }
  static uint32_t dc_stat_id(uint8_t n) { return 0x200 + n; }
  static uint32_t step_cmd_id(uint8_t n) { return 0x300 + n; }
  static uint32_t step_stat_id(uint8_t n) { return 0x400 + n; }

  static void pack_i16(uint8_t *buf, int pos, int16_t v)
  {
    buf[pos]     = (uint8_t)(v >> 8);
    buf[pos + 1] = (uint8_t)v;
  }

  static int16_t unpack_i16(const uint8_t *buf, int pos)
  {
    return (int16_t)(((uint16_t)buf[pos] << 8) | buf[pos + 1]);
  }

  /* ---- 帧编解码 (需编译单元) ---- */
  static void build_frame(uint8_t d[8], uint8_t cmd, int16_t p1, int16_t p2, int16_t p3);
  static void parse_stepper(const uint8_t d[8], StepperStatus *st);
  static void parse_dc(const uint8_t d[8], DcMotorStatus *st);

  /* ---- 多电机广播 (CAN ID 0x300, CMD 0x08) ---- */

#define MULTI_SKIP ((int16_t)0x8000)

  enum MultiSubCmd : uint8_t
  {
    MULTI_SUB_SPEED    = 0x01,
    MULTI_SUB_POSITION = 0x02,
    MULTI_SUB_STOP     = 0x03,
    MULTI_SUB_ENABLE   = 0x04,
  };

  /** 拼多电机广播帧 (协议层, 不负责发送)
   *  @param d[8]  输出帧缓冲
   *  @param sub   子命令 (MultiSubCmd)
   *  @param p1~p3 电机1~3参数, MULTI_SKIP=跳过 */
  static void build_multi_frame(uint8_t d[8], uint8_t sub, int16_t p1, int16_t p2, int16_t p3)
  {
    d[0] = 0x08; /* CMD_MULTI */
    d[1] = sub;
    pack_i16(d, 2, p1);
    pack_i16(d, 4, p2);
    pack_i16(d, 6, p3);
  }
};

#endif
