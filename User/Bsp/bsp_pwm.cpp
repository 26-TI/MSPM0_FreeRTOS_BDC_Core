/**
 * @file bsp_pwm.cpp
 * @brief MSPM0 BspPwm 实现 — 多通道 PWM 输出封装（边沿对齐，兼容 TIMG / TIMA）
 *
 * @note 硬件初始化（时钟/引脚/模式）由 SysConfig 生成的 SYSCFG_DL_PWM_xxx_init()
 *       在 SYSCFG_DL_init() 中统一完成。BspPwm 仅负责运行时占空比读写，
 *       不重复操作硬件初始化寄存器。
 *
 * @note 底层使用统一的 DL_Timer_* API（非 DL_TimerG_* 或 DL_TimerA_*），
 *       该 API 对 TIMG 和 TIMA 定时器均有效。
 */

#include "bsp_pwm.hpp"

/* ==================== 构造 ==================== */
BspPwm::BspPwm() {}

/* ==================== 初始化 ==================== */

/**
 * @brief 绑定 PWM 硬件实例，记录运行时参数
 *
 * @param cfg 包含定时器指针、时钟频率、周期、通道数及通道索引的完整配置
 * @return true  参数校验通过，绑定成功
 * @return false timer 为空、num_ch 为 0 或超过 MAX_CH
 *
 * @note 此函数不操作硬件寄存器，仅保存配置副本。
 *       硬件初始化需在此之前由 SYSCFG_DL_init() 完成。
 */
bool BspPwm::init(const Config &cfg)
{
  if (!cfg.timer || cfg.num_ch == 0 || cfg.num_ch > MAX_CH)
    return false;

  _cfg = cfg;
  return true;
}

/* ==================== 运行时控制 ==================== */

/**
 * @brief 设置指定通道的 PWM 占空比
 *
 * @param ch   通道号，范围 [0, num_ch-1]
 * @param duty 占空比值，范围 [0, period]
 *             若传入值超过 period，自动钳位到 period（100% 占空比）
 *
 * @note 占空比计算公式:  duty / period × 100%
 * @note 通道号越界时静默忽略，不操作硬件
 */
void BspPwm::set_duty(uint8_t ch, uint32_t duty)
{
  if (ch >= _cfg.num_ch)
    return;
  if (duty > _cfg.period)
    duty = _cfg.period;

  DL_Timer_setCaptureCompareValue(_cfg.timer, duty, _cfg.ch_idx[ch]);
}

/**
 * @brief 读取指定通道的当前占空比设定值
 *
 * @param ch 通道号，范围 [0, num_ch-1]
 * @return 占空比寄存器值（0 ~ period），通道号无效时返回 0
 *
 * @note 读取的是 Capture/Compare 寄存器值，即最近一次 set_duty 写入的值
 */
uint32_t BspPwm::get_duty(uint8_t ch) const
{
  if (ch >= _cfg.num_ch)
    return 0;

  return DL_Timer_getCaptureCompareValue(_cfg.timer, _cfg.ch_idx[ch]);
}

/**
 * @brief 获取 PWM 周期（即 Load 寄存器值）
 *
 * @return 周期值（无符号整数），由 SysConfig 硬件初始化时设定
 */
uint32_t BspPwm::get_period() const
{
  return DL_Timer_getLoadValue(_cfg.timer);
}

/**
 * @brief 计算并返回 PWM 输出频率
 *
 * @return 频率值 (Hz)
 * @retval 0  周期为 0（异常情况）
 * @retval >0 实际 PWM 频率
 *
 * @note 计算公式:  freq = clk_freq / period
 * @note 例: 80MHz / 10000 = 8kHz
 */
uint32_t BspPwm::get_freq() const
{
  uint32_t period = get_period();
  return (period > 0) ? (_cfg.clk_freq / period) : 0;
}
