#ifndef PID_HPP_
#define PID_HPP_

#include <stdint.h>

class PID
{
public:
  /**
     * @brief 构造并初始化 PID 控制器
     * @param p  比例系数 Kp
     * @param i  积分系数 Ki
     * @param d  微分系数 Kd
     * @param t  采样周期 T
     * @param out_max 输出限幅最大值
     * @param i_max   积分限幅最大值
     * @param i_epsilon_enable 积分分离使能 (1=开启, 0=关闭)
     */
  PID(float p, float i, float d, float t, float out_max, float i_max, uint8_t i_epsilon_enable);

  /// 重新初始化参数（保留已有对象重新配置）
  void init(float p, float i, float d, float t, float out_max, float i_max, uint8_t i_epsilon_enable);

  /// 执行一次 PID 计算，返回控制输出
  float calc(float target, float current);

  /// 清除历史误差（积分累加值+上次误差归零）
  void clear();

private:
  float   Kp_;
  float   Ki_;
  float   Kd_;
  float   T_;
  float   OUT_MAX_;
  float   I_MAX_;
  uint8_t I_EPSILON_ENABLE_;

  float sum_error_;
  float last_error_;
};

#endif // PID_HPP_
