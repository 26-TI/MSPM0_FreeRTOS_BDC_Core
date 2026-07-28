/**
 * @file ins.hpp
 * @brief 惯性导航角度闭环控制 — 基于陀螺仪 Yaw 的差速纠偏
 *
 * 原理:
 *   1. 从 ti_gyro 读取当前 Yaw 角
 *   2. 与目标 Yaw 比较，处理 0°/360° 环绕
 *   3. PID 计算差速修正量
 *   4. 左轮 = 基础速度 - 修正量, 右轮 = 基础速度 + 修正量
 *
 * 使用示例:
 * @code
 *   INS ins;
 *   ins.init(&ti_gyro, &motor_left, &motor_right);
 *   ins.set_pid(50.0f, 0.5f, 10.0f);
 *   ins.set_target(90.0f);
 *   ins.enable();
 *   // 在 10ms 周期中调用:
 *   ins.update();
 * @endcode
 */

#ifndef __INS_HPP__
#define __INS_HPP__

#include "pid.hpp"
#include "ti_gyro.hpp"
#include "motor.hpp"

class INS
{
public:
  PID _pid;

  /* ── 初始化 ── */
  void init(TiGyro *gyro, Motor *motor_left, Motor *motor_right);

  /* ── PID 参数 ── */
  void set_pid(float kp, float ki, float kd);

  /* ── 目标角度 ── */
  void  set_target(float yaw_deg);
  float target() const { return _target_yaw; }

  /* ── 基础速度 (角度保持时前进速度, rps) ── */
  void  set_base_speed(float rps) { _base_speed = rps; }
  float base_speed() const { return _base_speed; }

  /* ── 使能/失能 ── */
  void enable();
  void disable();
  bool enabled() const { return _enabled; }

  /* ── 周期更新 (10ms 调用一次) ── */
  void update();

  /* ── 状态查询 ── */
  float current_yaw() const { return _current_yaw; }
  float correction() const { return _correction; }

private:
  TiGyro *_gyro        = nullptr;
  Motor  *_motor_left  = nullptr;
  Motor  *_motor_right = nullptr;

  float _target_yaw  = 180.0f; ///< 目标 Yaw 角 (度, 0~360)
  float _current_yaw = 0.0f;   ///< 当前 Yaw 角
  float _base_speed  = 0.0f;   ///< 基础前进速度 (rps)
  float _correction  = 0.0f;   ///< 最近一次 PID 输出 (rps)
  bool  _enabled     = false;
};

#endif // __INS_HPP__
