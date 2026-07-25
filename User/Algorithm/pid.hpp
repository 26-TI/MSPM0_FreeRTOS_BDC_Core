/**
 * @file pid.hpp
 * @brief 位置式PID控制器 — 类声明
 *
 * @note 位置式 PID 算法:
 *   error    = target - current
 *   sum_error += error    （积分分离: 仅误差 < 60% 目标时累加）
 *   output   = Kp×error + Ki×T×sum_error + Kd×(error-last_error)/T
 *   output 钳位到 [-OUT_MAX, +OUT_MAX]
 *
 * @note 积分分离策略: 当 |error| < 0.6×|target| 时累加积分，否则暂停积分
 * @note 启动补偿: 当 |current| ≤ 0.2 时，输出额外 +1000 以克服静摩擦
 */

#ifndef __PID_HPP__
#define __PID_HPP__

class Pid
{
public:
  Pid();

  /**
   * @brief PID 参数初始化
   *
   * @param p        比例系数 Kp
   * @param i        积分系数 Ki
   * @param d        微分系数 Kd
   * @param t        采样周期 T (s)，如 0.005=5ms
   * @param out_max  输出限幅（绝对值）
   * @param i_max    积分限幅（绝对值）
   */
  void  init(float p, float i, float d, float t, float out_max, float i_max);

  /**
   * @brief PID 计算
   * @param target  目标值
   * @param current 当前反馈值
   * @return 限幅后的 PID 输出
   */
  float cala(float target, float current);

  /**
   * @brief 重置积分累计和微分历史
   */
  void  reset();

  /// @brief 获取上次输出值
  float get_output() const { return _output; }

private:
  float _kp, _ki, _kd;     ///< PID 系数
  float _t;               ///< 采样周期 (s)
  float _out_max;         ///< 输出限幅
  float _i_max;           ///< 积分限幅
  float _sum_error;       ///< 积分累计
  float _last_error;      ///< 上次误差
  float _output;         ///< 上次输出
};

#endif // __PID_HPP__
