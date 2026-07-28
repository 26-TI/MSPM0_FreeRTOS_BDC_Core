/**
 * @file pid.hpp
 * @brief 增量式 PID 控制器 — 积分分离 + 积分限幅 + 输出限幅
 *
 * 适用于电机速度闭环等嵌入式实时控制场景。
 * 纯浮点运算，无动态内存分配。
 */

#ifndef PID_HPP_
#define PID_HPP_

#include <stdint.h>

/**
 * @brief 增量式 PID 控制器
 *
 * 特性:
 *   - 积分分离: 误差小于目标 60% 时启用积分，避免超调
 *   - 积分清零: 误差超过目标时清零积分，快速响应方向切换
 *   - 积分限幅: 防止积分饱和
 *   - 输出限幅: 限制最终输出范围
 *
 * 使用示例:
 * @code
 *   PID pid;
 *   pid.init(0.1f, 0.01f, 0.0f, 0.01f, 1000.0f, 500.0f, 1);
 *   float out = pid.calc(target, current);
 * @endcode
 */
class PID
{
public:
  /**
   * @brief 初始化 PID 参数并清零历史误差
   * @param p  比例系数
   * @param i  积分系数
   * @param d  微分系数
   * @param t  采样周期 (秒)
   * @param out_max  PID 输出上限
   * @param i_max    积分累加上限
   * @param i_epsilon_enable  积分分离使能 (1=开启)
   */
  void init(float p, float i, float d, float t, float out_max, float i_max, uint8_t i_epsilon_enable);

  /**
   * @brief 执行一次 PID 计算
   * @param target   目标值
   * @param current  当前值
   * @return PID 控制输出 (已限幅)
   */
  float calc(float target, float current);

  /**
   * @brief 清零积分累加值和上次误差 (重置控制器状态)
   */
  void clear();

private:
  float   Kp_;               ///< 比例系数
  float   Ki_;               ///< 积分系数
  float   Kd_;               ///< 微分系数
  float   T_;                ///< 采样周期 (秒)
  float   OUT_MAX_;          ///< 输出上限
  float   I_MAX_;            ///< 积分累加上限
  uint8_t I_EPSILON_ENABLE_; ///< 积分分离使能标志

  float sum_error_;  ///< 积分累加值
  float last_error_; ///< 上一次误差值
};

#endif // PID_HPP_
