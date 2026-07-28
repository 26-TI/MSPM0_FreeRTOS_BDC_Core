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

  /**
   * @brief 绑定电机硬件资源并初始化
   * @param cfg  电机配置 (PWM/QEI/编码器线数/减速比/控制周期)
   */
  void Motor_Init(const Config &cfg);

  /**
   * @brief 初始化速度环 PID 参数
   * @param p  比例系数
   * @param i  积分系数
   * @param d  微分系数
   * @param t  采样周期 (秒)
   * @param out_max  PID 输出上限
   * @param i_max    积分上限
   * @param i_epsilon_enable  积分分离使能
   */
  void pid_init(float p, float i, float d, float t, float out_max, float i_max, uint8_t i_epsilon_enable);

  /** @brief 读取编码器 → 计算转速 → PID 输出 → 更新 PWM (由定时任务调用) */
  void update();

  /** @brief 使能电机输出 (开环, 占空比 50%) */
  void enable();

  /** @brief 关闭电机输出 */
  void disable();

  /** @brief 使能速度闭环控制 (PID 生效) */
  void speed_control_enable();

  /**
   * @brief 设定目标速度 (转/秒)
   * @param target  目标速度
   */
  void set_speed(float target);

  /* ── 调试接口 ── */
  float get_target() const { return _state.target; }
  float get_speed() const { return _state.speed; }
  float get_pid_out() const { return _pid_out; }

private:
  Config _cfg{};
  State  _state{};

  PID _pid;

  uint8_t _pid_enable = 0;
  float   _pid_out    = 0; ///< 最近一次 PID 输出（调试用）
};

#endif
