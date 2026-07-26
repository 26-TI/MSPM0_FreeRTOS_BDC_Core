/**
 * @file motor.hpp
 * @brief 直流电机设备驱动 — H 桥 PWM + QEI 编码器 + 转速 (RPM)
 *
 * @note 设计原则:
 *       - Motor 不管理定时器，update() 由用户定时中断/任务中调用
 *       - 编码器→转速 (RPM) 换算由 Motor 内部完成（x4 倍频 / 减速比 / 线数）
 *       - 占空比通过 set_duty() 直接设置 H 桥 PWM 通道
 *       - 所有参数在 init() 时一次性传入，运行期间不可修改
 *       - 运行状态通过 get_state() 获取只读引用
 *
 * @note 控制流程:
 *       1. 用户定时中断中调用 update()
 *       2. Motor 内部: 读编码器 → 算增量（16 位溢出安全） → 换算 RPM → 更新 State
 *
 * 使用示例:
 * @code
 *   Motor motor;
 *   motor.init({&bsp_pwm_g0, &bsp_qei, nullptr, 0, 1, 2000, 30.0f, 5, 10000});
 *   motor.enable();
 *   motor.set_duty(2500, 2500);
 *   // 在定时中断中:
 *   motor.update();
 *   const Motor::State &s = motor.get_state();
 *   // s.current_speed_rpm, s.pwm_a, s.pwm_b, s.is_forward ...
 * @endcode
 */

#ifndef __MOTOR_HPP__
#define __MOTOR_HPP__

#include "bsp_pwm.hpp"
#include "bsp_qei.hpp"
#include "bsp_gpio.hpp"
#include "pid.hpp"
#include <stdint.h>

class Motor
{
public:
  struct Config
  {
    BspPwm  *pwm;
    BspQei  *qei;
    BspGpio *enable_pin;
    uint8_t  pwm_ch_a, pwm_ch_b;

    uint16_t encoder_lines;     ///< 编码器线数
    float    gear_ratio;        ///< 减速比
    uint16_t control_period_ms; ///< 控制周期 (ms)
    uint32_t pwm_max;           ///< PWM 最大占空比 (period)
  };

  struct State
  {
    float target;
    float speed;
  };

  void Motor_Init(const Config &cfg);

  void PIDL_Init(float p, float i, float d, float t, float out_max, float i_max, uint8_t i_epsilon_enable);
  void PIDH_Init(float p, float i, float d, float t, float out_max, float i_max, uint8_t i_epsilon_enable);

  void update();
  void enable();
  void disable();
  void speed_control_enable();

  void set_speed(float target);

private:
  Config _cfg{};
  State  _state{};

  PID _pidl;
  PID _pidh;

  float _speed_l = 0.8;
  float _speed_h = 1.2;

  uint8_t _speed_statue = 0;
  uint8_t _pid_enable   = 0;
};

#endif
