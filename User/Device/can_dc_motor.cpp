/**
 * @file can_dc_motor.cpp
 * @brief DC 减速电机设备 — 实现
 */

#include "can_dc_motor.hpp"

/* ---- 初始化 ---- */

bool DcMotor::init(const Config &cfg)
{
  if (!cfg.send_fn || cfg.node_id == 0) return false;
  _node_id = cfg.node_id;
  _id_base = cfg.id_base;
  _send_fn = cfg.send_fn;
  _ready   = true;
  return true;
}

/* ---- CAN 发送辅助 ---- */

void DcMotor::_send_cmd(uint8_t cmd, int16_t p1, int16_t p2, int16_t p3)
{
  if (!_send_fn) return;
  uint8_t d[8];
  build_motor_frame(d, cmd, p1, p2, p3);
  _send_fn(can_id(), d, 8);
}

/* ---- 通用命令 ---- */

void DcMotor::enable(bool on)
  { _send_cmd(CMD_ENABLE, on ? 1 : 0, 0, 0); }

void DcMotor::stop()
  { _send_cmd(CMD_STOP, 0, 0, 0); }

/* ---- DC 专用命令 ---- */

void DcMotor::set_duty(uint16_t duty)
  { _send_cmd(CMD_DUTY, (int16_t)duty, 0, 0); }   // duty 0~1000

void DcMotor::set_rpm(float rpm)
  { _send_cmd(CMD_RPM, (int16_t)(rpm * 10.0f), 0, 0); } // rpm×10

/* ---- 状态 ---- */

void DcMotor::handle_status(const uint8_t d[8])
  { parse_dc_motor_status(d, &_status); }

uint8_t DcMotor::get_state()     const { return _status.state; }
uint8_t DcMotor::get_direction() const { return _status.dir; }
bool    DcMotor::has_status()    const { return _status._ck_ok; }
bool    DcMotor::is_running()    const { return _status.is_running(); }
