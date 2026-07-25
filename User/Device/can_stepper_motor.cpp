/**
 * @file can_stepper_motor.cpp
 * @brief 步进电机设备 — 实现
 */

#include "can_stepper_motor.hpp"

/* ---- 初始化 ---- */

bool StepperMotor::init(const Config &cfg)
{
  if (!cfg.send_fn || cfg.node_id == 0) return false;
  _node_id = cfg.node_id;
  _id_base = cfg.id_base;
  _send_fn = cfg.send_fn;
  _ready   = true;
  return true;
}

/* ---- CAN 发送辅助 ---- */

void StepperMotor::_send_cmd(uint8_t cmd, int16_t p1, int16_t p2, int16_t p3)
{
  if (!_send_fn) return;
  uint8_t d[8];
  build_motor_frame(d, cmd, p1, p2, p3);
  _send_fn(can_id(), d, 8);
}

/* ---- 通用命令 ---- */

void StepperMotor::enable(bool on)
  { _send_cmd(CMD_ENABLE, on ? 1 : 0, 0, 0); }

void StepperMotor::stop()
  { _send_cmd(CMD_STOP, 0, 0, 0); }

/* ---- 步进专用命令 ---- */

void StepperMotor::set_speed(float rpm)
  { _send_cmd(CMD_SPEED, (int16_t)(rpm * 10.0f), 0, 0); }

void StepperMotor::move_to(float angle_deg, float max_rpm)
  { _send_cmd(CMD_POSITION, (int16_t)(angle_deg * 100.0f), 0, (int16_t)max_rpm); }

void StepperMotor::move_abs(float angle_deg, int16_t turns, float max_rpm)
  { _send_cmd(CMD_POSITION, (int16_t)(angle_deg * 100.0f), turns, (int16_t)max_rpm); }

void StepperMotor::timed_move(float angle_deg, float duration_s)
  { _send_cmd(CMD_TIMED, (int16_t)(angle_deg * 100.0f), (int16_t)(duration_s * 1000.0f), 0); }

/* ---- 状态 ---- */

void StepperMotor::handle_status(const uint8_t d[8])
  { parse_stepper_status(d, &_status); }

float   StepperMotor::get_angle() const  { return _status.angle; }
float   StepperMotor::get_speed() const  { return _status.rpm; }
int16_t StepperMotor::get_turns() const  { return _status.turns; }
uint8_t StepperMotor::get_state() const  { return _status.state; }
bool    StepperMotor::has_status() const { return _status.ck_ok; }
bool    StepperMotor::is_done()   const  { return _status.is_done(); }
bool    StepperMotor::is_moving() const  { return _status.is_moving(); }
