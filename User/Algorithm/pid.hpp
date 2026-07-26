#ifndef PID_HPP_
#define PID_HPP_

#include <stdint.h>

class PID
{
public:
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
