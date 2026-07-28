/**
 * @file can_stepper_motor.hpp
 * @brief 步进电机设备 — CAN 远程步进电机节点的本地代理 (无虚函数)
 *
 * ## 用法
 * @code
 *   StepperMotor step;
 *   step.init({1, 0x300, canSend});   // nodeId=1, CAN ID=0x301
 *   step.set_speed(60);
 *   step.move_to(90);
 * @endcode
 */

#ifndef __STEPPER_MOTOR_HPP__
#define __STEPPER_MOTOR_HPP__

#include <stdint.h>
#include <stdbool.h>
#include "../Protocol/can_motor_proto.hpp"

class StepperMotor
{
public:
  /** CAN 发送回调类型 */
  typedef bool (*SendFn)(uint32_t id, const uint8_t *data, uint8_t len);

  struct Config
  {
    uint8_t  node_id; ///< CAN 节点号 (1~8)
    uint16_t id_base; ///< CAN ID 基数: 0x300 (步进命令)
    SendFn   send_fn; ///< → bsp_can.send
  };

  bool     init(const Config &cfg);
  uint32_t can_id() const { return (uint32_t)_id_base + _node_id; }

  /* ---- 通用命令 ---- */
  void enable(bool on = true); ///< CanMotorProto::STEP_ENABLE
  void stop();                 ///< CanMotorProto::STEP_STOP

  /* ---- 步进专用命令 ---- */
  void set_speed(float rpm);
  void move_to(float angle_deg, float max_rpm = 45.0f);
  void move_abs(float angle_deg, int16_t turns, float max_rpm = 45.0f);
  void timed_move(float angle_deg, float duration_s);

  /* ---- 状态帧 ---- */
  void handle_status(const uint8_t d[8]); ///< 解析并缓存状态

  float   get_angle() const;  ///< 当前角度 °
  float   get_speed() const;  ///< 当前转速 RPM
  int16_t get_turns() const;  ///< 累计圈数
  uint8_t get_state() const;  ///< 0=空闲 1=运动中 2=到位 3=故障
  bool    has_status() const; ///< 校验和是否通过
  bool    is_done() const;
  bool    is_moving() const;

private:
  void _send_cmd(uint8_t cmd, int16_t p1, int16_t p2, int16_t p3);

  uint8_t                      _node_id = 0;
  uint16_t                     _id_base = 0x300;
  SendFn                       _send_fn = nullptr;
  bool                         _ready   = false; ///< init 是否成功
  CanMotorProto::StepperStatus _status;
};

/* ================================================================
 *  MultiStep — 多电机广播发送 (设备层)
 *
 *  依赖协议层 CanMotorProto::build_multi_frame() 拼帧，
 *  本类只负责换算参数 + 发送。0x8000 = 跳过该电机。
 *
 *  用法:
 *    MultiStep::set_send_fn(can_send);
 *    MultiStep::speed(30, SKIP, 50);
 *    MultiStep::move_to(90, SKIP, 270);
 * ================================================================ */

class MultiStep
{
public:
  typedef bool (*SendFn)(uint32_t id, const uint8_t *data, uint8_t len);

  static void set_send_fn(SendFn fn) { _send_fn = fn; }

  static void speed(float rpm1, float rpm2, float rpm3)
  {
    _send(CanMotorProto::MULTI_SUB_SPEED,
          _rpm(rpm1),
          _rpm(rpm2),
          _rpm(rpm3));
  }

  static void move_to(float ang1, float ang2, float ang3)
  {
    _send(CanMotorProto::MULTI_SUB_POSITION,
          _ang(ang1),
          _ang(ang2),
          _ang(ang3));
  }

  static void stop(bool m1, bool m2, bool m3)
  {
    _send(CanMotorProto::MULTI_SUB_STOP,
          m1 ? 0 : MULTI_SKIP,
          m2 ? 0 : MULTI_SKIP,
          m3 ? 0 : MULTI_SKIP);
  }

  static void enable(int en1, int en2, int en3)
  {
    _send(CanMotorProto::MULTI_SUB_ENABLE,
          _en(en1),
          _en(en2),
          _en(en3));
  }

private:
  static SendFn _send_fn;

  static void _send(uint8_t sub, int16_t p1, int16_t p2, int16_t p3)
  {
    if (!_send_fn) return;
    uint8_t d[8];
    CanMotorProto::build_multi_frame(d, sub, p1, p2, p3);
    _send_fn(0x300, d, 8);
  }

  static int16_t _rpm(float v) { return (v < -32000.0f) ? MULTI_SKIP : (int16_t)(v * 10.0f); }
  static int16_t _ang(float v) { return (v < -32000.0f) ? MULTI_SKIP : (int16_t)(v * 100.0f); }
  static int16_t _en(int v)    { return v < 0 ? MULTI_SKIP : (int16_t)(v ? 1 : 0); }
};

#endif
