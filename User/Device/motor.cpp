/**
 * @file motor.cpp
 * @brief 直流电机驱动实现 — PWM + QEI 编码器 + PID 速度闭环
 *
 * 控制链路:
 *   定时中断 → update() → 读编码器→算转速 → PID 计算 → 更新 PWM 占空比
 *
 * 电机使能状态:
 *   enable()             使能电机输出 (开环, 占空比 50%)
 *   speed_control_enable() 使能速度闭环 (PID 生效)
 *   disable()            关闭电机输出
 */

#include "motor.hpp"

// ==========================================================================
// Motor_Init() — 初始化电机配置
// ==========================================================================

/**
 * @brief 绑定电机硬件资源并初始化状态
 *
 * 保存配置引用，复位 enable 引脚，清零 target/speed。
 *
 * @param cfg  电机配置（PWM/QEI/编码器线数/减速比/控制周期）
 */
void Motor::Motor_Init(const Config &cfg)
{
  _cfg = cfg;

  _cfg.enable_pin->reset();

  _state.target = 0;
  _state.speed  = 0;
}

// ==========================================================================
// pid_init() — 初始化速度 PID
// ==========================================================================

/**
 * @brief 初始化电机速度环 PID 参数
 * @param p  比例系数
 * @param i  积分系数
 * @param d  微分系数
 * @param t  采样周期 (秒)
 * @param out_max  PID 输出上限
 * @param i_max    积分累加上限
 * @param i_epsilon_enable  积分分离使能 (1=开启)
 */
void Motor::pid_init(float p, float i, float d, float t, float out_max, float i_max, uint8_t i_epsilon_enable)
{
  _pid.init(p, i, d, t, out_max, i_max, i_epsilon_enable);
}

// ==========================================================================
// update() — 速度闭环更新 (由定时任务调用)
// ==========================================================================

/**
 * @brief 读取编码器 → 计算转速 → PID 输出 → 更新 PWM
 *
 * 编码器计数器以 32767 为零点，每次读取后重置。
 * 转速 = Δcount / (编码器线数×4×减速比) × (1000/控制周期ms)
 *
 * PID 仅在 _pid_enable==1 时生效。
 */
void Motor::update()
{
  float pid_out = 0;

  _state.speed = (_cfg.qei->get_count() - 32767.0f) * (1000.0f / _cfg.control_period_ms) / (_cfg.encoder_lines * 4 * _cfg.gear_ratio);
  _cfg.qei->set_count(32767);

  if (_pid_enable == 1)
  {
    pid_out = _pid.calc(_state.target, _state.speed);

    _cfg.pwm->set_duty(_cfg.pwm_ch_b, 2500 + pid_out);
  }

  _pid_out = pid_out;
}

// ==========================================================================
// enable() — 使能电机输出 (开环)
// ==========================================================================

/**
 * @brief 使能电机输出 (开环模式, 占空比 50%)
 *
 * 开启 enable 引脚，两路 PWM 输出 50% 占空比 (2500/5000)。
 * 不启动 PID 控制。
 */
void Motor::enable()
{
  if (_pid_enable == 0)
  {
    _cfg.pwm->set_duty(_cfg.pwm_ch_b, 2500);
    _cfg.pwm->set_duty(_cfg.pwm_ch_a, 2500);
    _cfg.enable_pin->set();
  }
}

// ==========================================================================
// disable() — 关闭电机
// ==========================================================================

/**
 * @brief 关闭电机输出
 *
 * 清零 PID 使能标志，复位 enable 引脚，PWM 输出 50% 占空比（刹车）。
 */
void Motor::disable()
{
  _pid_enable = 0;
  _cfg.enable_pin->reset();
  _cfg.pwm->set_duty(_cfg.pwm_ch_b, 2500);
  _cfg.pwm->set_duty(_cfg.pwm_ch_a, 2500);
}

// ==========================================================================
// speed_control_enable() — 使能速度闭环
// ==========================================================================

/**
 * @brief 使能速度闭环控制
 *
 * 与 enable() 区别: 额外设置 _pid_enable=1，使 update() 中 PID 生效。
 */
void Motor::speed_control_enable()
{
  if (_pid_enable == 0)
  {
    _cfg.pwm->set_duty(_cfg.pwm_ch_b, 2500);
    _cfg.pwm->set_duty(_cfg.pwm_ch_a, 2500);
    _cfg.enable_pin->set();
    _pid_enable = 1;
  }
}

// ==========================================================================
// set_speed() — 设定目标速度
// ==========================================================================

/**
 * @brief 设定目标速度 (转/秒)
 *
 * 仅保存目标值，实际控制由 update() 在下个控制周期执行。
 *
 * @param target  目标速度 (转/秒)
 */
void Motor::set_speed(float target)
{
  _state.target = target;
}
