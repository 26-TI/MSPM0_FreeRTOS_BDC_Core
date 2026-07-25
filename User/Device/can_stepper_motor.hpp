/**
 * @file can_stepper_motor.hpp
 * @brief 步进电机设备 — CAN 远程步进电机节点的本地代理 (无虚函数)
 *
 * ## 用法
 * @code
 *   StepperMotor step;
 *   step.init({1, 0x200, canSend});   // nodeId=1, CAN ID=0x201
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
    uint8_t  node_id;   ///< CAN 节点号 (1~8)
    uint16_t id_base;   ///< CAN ID 基数: 0x200
    SendFn   send_fn;   ///< → bsp_can.send
  };

  bool init(const Config &cfg);
  uint32_t can_id() const { return (uint32_t)_id_base + _node_id; }

  /* ---- 通用命令 ---- */
  void enable(bool on = true);   ///< CMD_ENABLE
  void stop();                   ///< CMD_STOP

  /* ---- 步进专用命令 ---- */
  void set_speed(float rpm);
  void move_to(float angle_deg, float max_rpm = 45.0f);
  void move_abs(float angle_deg, int16_t turns, float max_rpm = 45.0f);
  void timed_move(float angle_deg, float duration_s);

  /* ---- 状态帧 ---- */
  void handle_status(const uint8_t d[8]); ///< 解析并缓存状态

  float   get_angle() const;   ///< 当前角度 °
  float   get_speed() const;   ///< 当前转速 RPM
  int16_t get_turns() const;   ///< 累计圈数
  uint8_t get_state() const;   ///< 0=空闲 1=运动中 2=到位 3=故障
  bool    has_status() const;  ///< 校验和是否通过
  bool    is_done() const;
  bool    is_moving() const;

private:
  void _send_cmd(uint8_t cmd, int16_t p1, int16_t p2, int16_t p3);

  uint8_t         _node_id = 0;
  uint16_t        _id_base = 0x200;
  SendFn          _send_fn = nullptr;
  bool            _ready   = false;   ///< init 是否成功
  StepperStatus   _status;
};

#endif
