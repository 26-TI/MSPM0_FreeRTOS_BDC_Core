/**
 * @file ins.cpp
 * @brief 惯性导航角度闭环控制 — 实现
 */

#include "ins.hpp"

/* ===================================================================
 * INS::init()
 * =================================================================== */

void INS::init(TiGyro *gyro, Motor *motor_left, Motor *motor_right)
{
  _gyro        = gyro;
  _motor_left  = motor_left;
  _motor_right = motor_right;
}

/* ===================================================================
 * INS::set_pid()
 * =================================================================== */

void INS::set_pid(float kp, float ki, float kd)
{
  _pid.init(kp, ki, kd, 0.01f, 3.0f, 1.0f, 0);
}

/* ===================================================================
 * INS::set_target()
 * =================================================================== */

void INS::set_target(float yaw_deg)
{
  _target_yaw = yaw_deg;
}

/* ===================================================================
 * INS::enable() / disable()
 * =================================================================== */

void INS::enable()
{
  _enabled    = true;
  _correction = 0.0f;
  _pid.clear();
}

void INS::disable()
{
  _enabled = false;
  if (_motor_left) _motor_left->set_speed(0.0f);
  if (_motor_right) _motor_right->set_speed(0.0f);
  _correction = 0.0f;
}

/* ===================================================================
 * 角度误差计算 (处理 0°/360° 环绕)
 * =================================================================== */

static float angle_error(float target, float current)
{
  float err = target - current;

  /* 归一化到 [-180, 180) */
  while (err > 180.0f) err -= 360.0f;
  while (err < -180.0f) err += 360.0f;

  return err;
}

/* ===================================================================
 * INS::update() — 10ms 周期
 * =================================================================== */

void INS::update()
{
  if (!_enabled || !_gyro || !_motor_left || !_motor_right)
    return;

  /* ── 1. 读取当前 Yaw ── */
  const TiGyro::ReportData &r = _gyro->get_last_report();
  if (!r.valid)
  {
    _motor_left->set_speed(0.0f);
    _motor_right->set_speed(0.0f);
    return;
  }
  _current_yaw = r.yaw_deg;

  /* ── 2. 计算角度误差 ── */
  float error = angle_error(_target_yaw, _current_yaw);

  if (fabs(error) < 0.8f)
  {
    _motor_left->set_speed(0.0f);
    _motor_right->set_speed(0.0f); /* 右轮硬件反装 */
    return;
  }

  /* ── 3. PID: target=error, current=0 → 输出 = Kp*error + ... ── */
  _correction = _pid.calc(0.0f, error);

  /* ── 4. 差速驱动 ── */
  float left_speed  = _base_speed - _correction;
  float right_speed = _base_speed + _correction;

  _motor_left->set_speed(left_speed);
  _motor_right->set_speed(-right_speed); /* 右轮硬件反装 */
}
