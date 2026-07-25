/**
 * @file motor.hpp
 * @brief 直流电机设备驱动 — H 桥 PWM + QEI 编码器 + 速度环 PID
 *
 * @note 设计原则:
 *       - Motor 不管理定时器，update() 由用户定时中断/任务中调用
 *       - 编码器→速度换算由 Motor 内部完成（x4 倍频 / 减速比 / 线数 / 轮周长）
 *       - PID 输出经方向判断后，映射到 H 桥正/反转两个 PWM 通道
 *       - 所有参数在 init() 时一次性传入，运行期间不可修改
 *       - 运行状态通过 get_state() 获取只读引用
 *
 * @note 控制流程:
 *       1. 用户定时中断中调用 update()
 *       2. Motor 内部: 读编码器 → 算增量（16 位溢出安全） → 换算速度
 *          → PID 计算 → H 桥 PWM 输出 → 更新 State
 *
 * 使用示例:
 * @code
 *   Motor motor;
 *   motor.init({&bsp_pwm_g0, &bsp_qei, 0, 1, 2000, 19.48f, 30.0f, 20, 0.5f, 0.1f, 0.0f, 10000});
 *   motor.set_target_speed(50.0f);  // 50 cm/s 前进
 *   // 在定时中断中:
 *   motor.update();
 *   const Motor::State &s = motor.get_state();
 *   // s.current_speed_cm_s, s.current_pwm, s.is_forward ...
 * @endcode
 */

#ifndef __MOTOR_HPP__
#define __MOTOR_HPP__

#include "bsp_pwm.hpp"
#include "bsp_qei.hpp"
#include "pid.hpp"
#include <stdint.h>

class Motor
{
public:
  /* ==================== 配置结构体 ==================== */
  /**
   * @brief 电机初始化配置（全部参数 init 时一次性传入，运行期不可改）
   */
  struct Config
  {
    /* ---- 硬件绑定 ---- */
    BspPwm *pwm;        ///< PWM 外设实例指针（需已 init）
    BspQei *qei;        ///< QEI 编码器实例指针（需已 init）
    uint8_t pwm_ch_fwd; ///< 正转 PWM 通道号（0~3），对应 H 桥正转输入端
    uint8_t pwm_ch_rev; ///< 反转 PWM 通道号（0~3），对应 H 桥反转输入端

    /* ---- 编码器参数 ---- */
    uint16_t encoder_lines;      ///< 编码器线数（脉冲/圈），不含倍频，如 2000
    float    wheel_perimeter_cm; ///< 轮子周长 (cm)
    float    gear_ratio;         ///< 减速比（电机圈数 : 轮子圈数），如 30.0 表示电机30圈轮子1圈

    /* ---- 控制参数 ---- */
    uint16_t control_period_ms; ///< 控制周期 (ms)，需与 update() 调用周期一致
    float    kp;                ///< PID 比例系数
    float    ki;                ///< PID 积分系数
    float    kd;                ///< PID 微分系数
    uint32_t pwm_max;           ///< PWM 占空比上限（≤ BspPwm period），也作为 PID 输出限幅
  };

  /* ==================== 运行状态（只读） ==================== */
  /**
   * @brief 电机实时运行状态
   * @note 由 update() 内部更新，外部通过 get_state() 只读访问
   */
  struct State
  {
    float    target_speed_cm_s;  ///< 目标速度 (cm/s)，正=前进，负=后退
    float    current_speed_cm_s; ///< 当前实际速度 (cm/s)，由编码器增量换算
    uint32_t current_pwm;        ///< 当前 PWM 占空比（绝对值，0 ~ pwm_max）
    int32_t  total_pulses;       ///< 累计编码器脉冲数（带符号，正=正转累积）
    bool     is_forward;         ///< 当前转动方向: true=正转，false=反转/停止
  };

  /* ==================== 接口 ==================== */
  Motor();

  /**
   * @brief 初始化电机驱动（绑定硬件、预计算参数、初始化 PID）
   *
   * @param cfg 完整硬件/编码器/控制参数
   * @return true  参数有效，初始化成功
   * @return false 参数无效（空指针、除零、通道号越界）
   *
   * @note 调用前需确保 BspPwm 和 BspQei 已完成各自的 init()
   * @note 初始化时自动读取编码器当前值作为增量计算的起点
   */
  bool init(const Config &cfg);

  /**
   * @brief 设置目标速度
   *
   * @param speed_cm_s 目标速度 (cm/s)
   *                   正值 = 前进（PID 输出 > 0 → CH_fwd 输出 PWM）
   *                   负值 = 后退（PID 输出 < 0 → CH_rev 输出 PWM）
   *                   零   = 停止
   *
   * @note 此函数仅写入目标值，不立即生效。
   *       实际控制在下次 update() 时由 PID 闭环执行。
   */
  void set_target_speed(float speed_cm_s);

  /**
   * @brief 控制循环主函数（由用户定时中断/任务调用）
   *
   * @note 内部执行顺序:
   *       1. 读取 QEI 编码器当前计数值
   *       2. 计算与上次读数的差值（16 位溢出安全，自动判别方向）
   *       3. 累积总脉冲数
   *       4. 将脉冲增量换算为线速度 (cm/s)
   *       5. 调用速度环 PID 计算 PWM 输出
   *       6. 根据 PID 输出正负分配到 H 桥正/反转通道
   *       7. 更新 State 结构体
   *
   * @note 调用频率必须与 control_period_ms 一致，否则速度换算错误
   * @note 在定时中断中调用时，应保持此函数执行时间尽量短
   */
  void update();

  /**
   * @brief 紧急停止
   *
   * @note 执行操作:
   *       - 两路 PWM 通道均设为 0（H 桥全部关闭）
   *       - PID 控制器复位（积分清零、微分历史清零）
   *       - State 全部清零（包括 target_speed，需重新设置）
   */
  void stop();

  /**
   * @brief 获取电机只读状态
   * @return State 结构体常量引用，包含目标速度/当前速度/PWM/脉冲/方向
   */
  const State &get_state() const { return _state; }

private:
  /**
   * @brief 编码器脉冲增量 → 线速度换算
   *
   * @param delta_pulses 本周期编码器脉冲增量（带符号）
   * @return 线速度 (cm/s)，正值 = 正转方向
   *
   * @note 换算公式:
   *       speed = delta / (encoder_lines × 4 × gear_ratio)
   *             × wheel_perimeter_cm / (control_period_ms / 1000)
   */
  float pulses_to_speed_cm_s(int16_t delta_pulses) const;

  /**
   * @brief H 桥 PWM 输出控制
   *
   * @param pid_output PID 计算结果（正值 = 正转 PWM，负值 = 反转 PWM）
   *
   * @note 输出逻辑:
   *       - pid_output > 0: CH_fwd = duty, CH_rev = 0    （正转）
   *       - pid_output < 0: CH_fwd = 0, CH_rev = |duty|  （反转）
   *       - pid_output = 0: CH_fwd = 0, CH_rev = 0       （停止/刹车）
   *       - duty 自动钳位到 [0, pwm_max]
   */
  void apply_pwm(float pid_output);

  /* ---- 成员变量 ---- */
  Config   _cfg{};         ///< 配置副本（init 后不可变）
  State    _state{};       ///< 运行状态（由 update() 更新）
  Pid      _pid;           ///< 速度环 PID 控制器
  float    _pulses_per_cm; ///< 预计算常数: 每 cm 线位移对应的编码器脉冲数
  uint16_t _last_count;    ///< 上次编码器读数，用于计算本周期增量
};

#endif // __MOTOR_HPP__
