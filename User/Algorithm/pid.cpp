/**
 * @file pid.cpp
 * @brief 位置式PID控制器 — 实现
 *
 * @note PID 公式:
 *   error    = target - current
 *   sum_error += error        （积分分离: 误差较大时暂停累加以防过冲）
 *   output   = Kp×error + Ki×T×sum_error + Kd×(error-last_error)/T
 *
 * @note 积分分离: |error| ≥ 0.6×|target| 时停止积分累加
 * @note 积分限幅: sum_error 钳位到 [-I_MAX, +I_MAX]
 * @note 启动补偿: |current| ≤ 0.2 时输出额外 +1000 克服静摩擦
 * @note 输出限幅: output 钳位到 [0, OUT_MAX]（单向输出）
 */

#include "pid.hpp"

/* ==================== 构造 / 析构 ==================== */

/**
 * @brief 默认构造函数 — 初始化为零参数
 */
Pid::Pid()
{
  init(0, 0, 0, 0, 0, 0);
}

/* ==================== 初始化 ==================== */

/**
 * @brief PID 全参数初始化
 */
void Pid::init(float p, float i, float d, float t, float out_max, float i_max)
{
  _kp      = p;
  _ki      = i;
  _kd      = d;
  _t       = t;
  _out_max = out_max;
  _i_max   = i_max;

  _sum_error  = 0;
  _last_error = 0;
  _output    = 0;
}

/* ==================== 计算 ==================== */

/**
 * @brief PID 计算（带积分分离 + 积分限幅 + 启动补偿 + 输出限幅）
 *
 * @param target  目标值
 * @param current 当前反馈值
 * @return 限幅后的 PID 输出 [0, OUT_MAX]
 */
float Pid::cala(float target, float current)
{
  /* ---- 计算误差 ---- */
  float error = target - current;

  /* ---- 积分分离: 仅误差较小时累加 ---- */
  if (error < 0.6f * target && error > -0.6f * target)
  {
    _sum_error += error;
  }

  /* ---- 积分限幅 ---- */
  if (_sum_error > _i_max)
    _sum_error = _i_max;
  else if (_sum_error < -_i_max)
    _sum_error = -_i_max;

  /* ---- PID 计算 ---- */
  float output = _kp * error + _ki * _t * _sum_error + _kd * (error - _last_error) / _t;

  /* ---- 保存本次误差 ---- */
  _last_error = error;

  /* ---- 启动补偿（速度接近零时增加输出以克服静摩擦）---- */
  if (current <= 0.2f && current >= -0.2f)
  {
    output += 1000;
  }

  /* ---- 输出限幅（单向输出 ≥ 0）---- */
  _output = output;
  return _output > _out_max ? _out_max : _output;
}

/* ==================== 重置 ==================== */

/**
 * @brief PID 重置 — 清零积分和微分历史
 */
void Pid::reset()
{
  _sum_error  = 0;
  _last_error = 0;
  _output    = 0;
}
