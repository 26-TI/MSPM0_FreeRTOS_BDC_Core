/**
 * @file motor.cpp
 * @brief 直流电机设备驱动 — 实现
 *
 * @note 速度换算公式（x4 倍频 + 减速比）:
 *       pulses_per_wheel_rev = encoder_lines × 4 × gear_ratio
 *       revolutions          = delta_pulses / pulses_per_wheel_rev
 *       distance_cm          = revolutions × wheel_perimeter_cm
 *       speed_cm_s           = distance_cm / (control_period_ms / 1000)
 *
 *       简化:
 *       speed_cm_s = delta_pulses × wheel_perimeter_cm
 *                  / (encoder_lines × 4 × gear_ratio × control_period_ms / 1000)
 *
 * @note H 桥 PWM 输出策略:
 *       - pid_output > 0 → 正转: CH_fwd = duty, CH_rev = 0
 *       - pid_output < 0 → 反转: CH_fwd = 0, CH_rev = |duty|
 *       - pid_output = 0 → 停止: CH_fwd = 0, CH_rev = 0
 *       - duty 自动钳位到 [0, pwm_max]
 *
 * @note 16 位编码器溢出处理:
 *       通过 uint16_t 减法自动模 65536 运算，再根据差值是否超过半量程
 *       （32767）判断真实方向。只要相邻两次 update() 之间的脉冲变化
 *       不超过 ±32767，算法即正确。
 */

#include "motor.hpp"

/* ==================== 构造 ==================== */
Motor::Motor() {}

/* ==================== 初始化 ==================== */

/**
 * @brief 初始化电机驱动
 *
 * 执行以下操作:
 * -# 校验参数有效性（空指针 / 除零 / 通道号越界）
 * -# 保存配置副本到 _cfg
 * -# 初始化速度环 PID 控制器（输出限幅 = pwm_max，积分限幅 = pwm_max × 35%）
 * -# 预计算每厘米位移对应的编码器脉冲数 _pulses_per_cm
 * -# 同步编码器初值，清零运行状态
 * -# 注意，如果用24v控制12v电机，pwm最大输出需要调整到50%
 *
 * @param cfg 硬件绑定 / 编码器参数 / PID 系数 / 控制周期的完整配置
 * @return true  参数有效，初始化成功
 * @return false pwm 或 qei 为空、encoder_lines 或 control_period_ms 为零、
 *               通道号 ≥ BspPwm::MAX_CH
 */
bool Motor::init(const Config &cfg)
{
  if (!cfg.pwm || !cfg.qei)
    return false;
  if (cfg.encoder_lines == 0 || cfg.control_period_ms == 0)
    return false;
  if (cfg.pwm_ch_fwd >= BspPwm::MAX_CH || cfg.pwm_ch_rev >= BspPwm::MAX_CH)
    return false;

  _cfg = cfg;

  /*
   * PID 初始化
   * 输出限幅 = pwm_max，积分限幅 = pwm_max
   * T = control_period_ms / 1000（采样周期 s）
   */
  _pid.init(cfg.kp, cfg.ki, cfg.kd,
            cfg.control_period_ms / 1000.0f,
            (float)cfg.pwm_max, (float)cfg.pwm_max);

  /*
   * 预计算: 每厘米轮子位移对应的编码器脉冲数
   * pulses_per_rev = encoder_lines × 4（x4 倍频） × gear_ratio（减速比）
   * _pulses_per_cm = pulses_per_rev / wheel_perimeter_cm
   */
  float pulses_per_rev = (float)cfg.encoder_lines * 4.0f * cfg.gear_ratio;
  _pulses_per_cm       = pulses_per_rev / cfg.wheel_perimeter_cm;

  /* 同步编码器初值（以此为增量计算的起点），清零运行状态 */
  _last_count = cfg.qei->get_count();
  _state      = {};

  return true;
}

/* ==================== 目标速度设置 ==================== */

/**
 * @brief 设置目标速度
 *
 * @param speed_cm_s 目标线速度 (cm/s)
 *                   正值 = 前进方向（PID 将使 CH_fwd 输出 PWM）
 *                   负值 = 后退方向（PID 将使 CH_rev 输出 PWM）
 *                   零   = 停止
 *
 * @note 此函数仅更新内部目标值，不立即产生 PWM 输出。
 *       实际控制动作在下次 update() 时由 PID 闭环完成。
 */
void Motor::set_target_speed(float speed_cm_s)
{
  _state.target_speed_cm_s = speed_cm_s;
}

/* ==================== 控制更新 ==================== */

/**
 * @brief 控制循环主函数 — 由用户定时中断调用
 *
 * 完整流程（共 6 步）:
 *
 * @step 1 读取 QEI 编码器当前计数值，计算与上次读数的差值。
 *         使用 uint16_t 减法自动处理模 65536 溢出，再通过半量程判定
 *         （差值 > 32767 则为负向溢出）转回有符号增量。
 * @step 2 将增量累加到 total_pulses（带符号累积，用于里程/位置计算）。
 * @step 3 使用预计算的 _pulses_per_cm 将脉冲增量换算为线速度 (cm/s)。
 * @step 4 调用速度环 PID: 输入 target_speed 和 current_speed，
 *         输出 PWM 占空比（正值=正转力矩，负值=反转力矩）。
 * @step 5 根据 PID 输出正负，分配到 H 桥的两个 PWM 通道。
 * @step 6 更新 State 中的 current_pwm 和 is_forward。
 *
 * @note 调用频率必须与 init 时指定的 control_period_ms 严格一致，
 *       否则速度换算结果将出现比例误差。
 * @note 本函数应在定时中断或高优先级任务中调用，
 *       内部无裸机阻塞操作，执行时间可控。
 */
void Motor::update()
{
  /* ---- 步骤 1: 读取编码器，计算有符号增量（16 位溢出安全） ---- */
  uint16_t current = _cfg.qei->get_count();
  uint16_t diff    = current - _last_count; /* uint16_t 减法，自动模 65536 */
  int16_t  delta   = (diff > 32767) ? (int16_t)(diff - 65536) : (int16_t)diff;
  _last_count      = current;

  /* ---- 步骤 2: 累计编码器脉冲（带符号，用于里程统计） ---- */
  _state.total_pulses += (int32_t)delta;

  /* ---- 步骤 3: 脉冲增量 → 线速度 (cm/s) ---- */
  _state.current_speed_cm_s = pulses_to_speed_cm_s(delta);

  /* ---- 步骤 4: 速度环 PID 计算 ---- */
  float pid_out = _pid.cala(_state.target_speed_cm_s,
                            _state.current_speed_cm_s);

  /* ---- 步骤 5: H 桥 PWM 输出 ---- */
  apply_pwm(pid_out);

  /* ---- 步骤 6: 更新运行状态 ---- */
  _state.current_pwm = (uint32_t)(pid_out > 0 ? pid_out : -pid_out);
  _state.is_forward  = (pid_out >= 0);
}

/* ==================== 紧急停止 ==================== */

/**
 * @brief 紧急停止
 *
 * 执行以下操作:
 * - 正/反转两路 PWM 通道均设为 0（H 桥全部关断，电机惯性滑行）
 * - PID 控制器复位（积分清零、微分历史清零，保留 PID 系数）
 * - State 全部字段清零（包括 target_speed_cm_s，需重新 set_target_speed）
 */
void Motor::stop()
{
  /* 两路 PWM 归零 */
  _cfg.pwm->set_duty(_cfg.pwm_ch_fwd, 0);
  _cfg.pwm->set_duty(_cfg.pwm_ch_rev, 0);

  /* PID 积分/微分历史清零 */
  _pid.reset();

  /* 状态全部复位 */
  _state = {};
}

/* ==================== 速度换算 ==================== */

/**
 * @brief 编码器脉冲增量 → 线速度换算
 *
 * @param delta_pulses 当前控制周期内的编码器脉冲增量（带符号，int16_t）
 * @return 线速度 (cm/s)，正值 = 正转（前进）方向
 *
 * @note 换算步骤:
 *       1. distance_cm = delta_pulses / _pulses_per_cm
 *          （脉冲数 ÷ 每厘米脉冲数 = 位移 cm）
 *       2. speed_cm_s = distance_cm / (control_period_ms / 1000)
 *          （位移 ÷ 时间 = 速度）
 */
float Motor::pulses_to_speed_cm_s(int16_t delta_pulses) const
{
  float distance_cm = (float)delta_pulses / _pulses_per_cm;
  float period_s    = _cfg.control_period_ms / 1000.0f;
  return distance_cm / period_s;
}

/* ==================== H 桥 PWM 输出 ==================== */

/**
 * @brief 根据 PID 输出正负，分配 H 桥两路 PWM 通道的占空比
 *
 * @param pid_output PID 计算结果:
 *                   > 0 → 正转力矩，占空比 = +pid_output
 *                   < 0 → 反转力矩，占空比 = |pid_output|
 *                   = 0 → 停止，两路 PWM 均关闭
 *
 * @note PWM 占空比由 BspPwm::set_duty() 内部自动钳位到 [0, period]，
 *       即使 pid_output 绝对值超过 pwm_max 也不会写入越界值。
 * @note 两路 PWM 不会同时输出非零值，避免 H 桥上下管直通。
 */
void Motor::apply_pwm(float pid_output)
{
  if (pid_output > 0)
  {
    /* 正转: 正转通道输出 PWM，反转通道关闭 */
    _cfg.pwm->set_duty(_cfg.pwm_ch_fwd, (uint32_t)pid_output);
    _cfg.pwm->set_duty(_cfg.pwm_ch_rev, 0);
  }
  else if (pid_output < 0)
  {
    /* 反转: 反转通道输出 PWM，正转通道关闭 */
    _cfg.pwm->set_duty(_cfg.pwm_ch_fwd, 0);
    _cfg.pwm->set_duty(_cfg.pwm_ch_rev, (uint32_t)(-pid_output));
  }
  else
  {
    /* 停止: 两通道均关闭 */
    _cfg.pwm->set_duty(_cfg.pwm_ch_fwd, 0);
    _cfg.pwm->set_duty(_cfg.pwm_ch_rev, 0);
  }
}
