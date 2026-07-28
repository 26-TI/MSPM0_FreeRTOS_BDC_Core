/**
 * @file pid.cpp
 * @brief 增量式 PID 控制器 — 实现
 */

#include "pid.hpp"

// ==========================================================================
// init() — 初始化 PID 参数
// ==========================================================================

/**
 * @brief 保存控制参数并清零历史状态
 * @see PID::init()
 */
void PID::init(float p, float i, float d, float t, float out_max, float i_max, uint8_t i_epsilon_enable)
{
  Kp_               = p;
  Ki_               = i;
  Kd_               = d;
  T_                = t;
  OUT_MAX_          = out_max;
  I_MAX_            = i_max;
  I_EPSILON_ENABLE_ = i_epsilon_enable;

  clear();
}

// ==========================================================================
// calc() — PID 计算核心
// ==========================================================================

/**
 * @brief 执行一次增量式 PID 计算
 *
 * 算法步骤:
 *   1. 计算误差 = target - current
 *   2. 积分分离: 误差 < 目标×60% 时累加积分
 *   3. 积分清零: 误差 > 目标时清零 (方向快速切换)
 *   4. 积分限幅: 限制在 ±I_MAX_ 内
 *   5. 输出 = Kp×e + Ki×∫e·dt + Kd×de/dt
 *   6. 输出限幅: 限制在 ±OUT_MAX_ 内
 *
 * @see PID::calc()
 */
float PID::calc(float target, float current)
{
  // 计算误差值
  float error = target - current;

  // 计算积分项并进行积分限幅积分分离（使用绝对值，正负目标对称）
  float abs_target = (target > 0.0f) ? target : -target;
  float abs_error  = (error > 0.0f) ? error : -error;

  // 积分分离
  if (I_EPSILON_ENABLE_ == 1)
  {
    if (abs_error < 0.6f * abs_target)
      sum_error_ += error * T_;
  }
  else
  {
    sum_error_ += error * T_;
  }

  // 积分在剧烈变化时清零，保证迅速切换速度方向时不受积分影响
  if (abs_error > 1.0f * abs_target)
    sum_error_ = 0;

  // 积分限幅
  if (sum_error_ > I_MAX_)
    sum_error_ = I_MAX_;
  else if (sum_error_ < -I_MAX_)
    sum_error_ = -I_MAX_;

  // 计算 PID 输出
  float output = Kp_ * error
                 + Ki_ * sum_error_
                 + Kd_ * (error - last_error_) / T_;

  // 保存误差值
  last_error_ = error;

  // 输出限幅
  if (output > OUT_MAX_)
    output = OUT_MAX_;
  else if (output < -OUT_MAX_)
    output = -OUT_MAX_;

  return output;
}

// ==========================================================================
// clear() — 清零历史误差
// ==========================================================================

/**
 * @brief 清零积分累加值和上次误差，重置控制器状态
 * @see PID::clear()
 */
void PID::clear()
{
  last_error_ = 0;
  sum_error_  = 0;
}
