/**
 * @file bsp_pwm.hpp
 * @brief MSPM0 BspPwm — 多通道 PWM 输出封装（边沿对齐，兼容 TIMG / TIMA）
 *
 * @note 硬件初始化由 SysConfig 生成的 SYSCFG_DL_xxx_init() 完成，
 *       BspPwm 仅封装运行时占空比控制与状态查询。
 *
 * @note 底层使用统一的 DL_Timer_* API，TIMG 和 TIMA 均可使用。
 *
 * 使用示例:
 * @code
 *   BspPwm pwm;
 *   pwm.init({TIMG7, 80000000, 10000, 2, {DL_TIMER_CC_0_INDEX, DL_TIMER_CC_1_INDEX}});
 *   pwm.set_duty(0, 2500);  // CH0 占空比 25%
 * @endcode
 */

#ifndef __BSP_PWM_MSPM0_HPP__
#define __BSP_PWM_MSPM0_HPP__

#include "ti_msp_dl_config.h" // IWYU pragma: keep

class BspPwm
{
public:
  /** @brief 最大支持的 PWM 通道数 */
  static constexpr uint8_t MAX_CH = 4;

  /**
   * @brief PWM 初始化配置结构体
   */
  struct Config
  {
    GPTIMER_Regs     *timer;          ///< 定时器外设实例指针（TIMGx 或 TIMAx，如 TIMG7）
    uint32_t          clk_freq;       ///< 定时器输入时钟频率 (Hz)，如 80000000
    uint32_t          period;         ///< PWM 周期（Load 寄存器值），决定 PWM 分辨率
    uint8_t           num_ch;         ///< 实际使能的通道数（≤ MAX_CH）
    DL_TIMER_CC_INDEX ch_idx[MAX_CH]; ///< 各通道对应的 CC 索引，仅前 num_ch 个有效
                                      ///< （如 DL_TIMER_CC_0_INDEX）
  };

  BspPwm();

  /**
   * @brief 绑定 PWM 硬件实例
   * @param cfg 定时器/时钟/周期/通道配置
   * @return true  参数有效，绑定成功
   * @return false timer 为空、num_ch 为 0 或超过 MAX_CH
   */
  bool init(const Config &cfg);

  /* ==================== 运行时控制 ==================== */

  /**
   * @brief 设置指定通道的占空比
   * @param ch   通道号（0 ~ num_ch-1）
   * @param duty 占空比值，范围 [0, period]，超出自动钳位到 period
   * @note 占空比 = duty / period × 100%
   */
  void set_duty(uint8_t ch, uint32_t duty);

  /**
   * @brief 读取指定通道的当前占空比设定值
   * @param ch 通道号（0 ~ num_ch-1）
   * @return 占空比值，通道号无效时返回 0
   */
  uint32_t get_duty(uint8_t ch) const;

  /**
   * @brief 获取 PWM 周期（Load 寄存器值）
   * @return 当前周期值
   */
  uint32_t get_period() const;

  /**
   * @brief 获取 PWM 输出频率
   * @return 频率 (Hz)，计算公式: freq = clk_freq / period
   */
  uint32_t get_freq() const;

  /** @brief 获取绑定的定时器外设指针 */
  GPTIMER_Regs *get_timer() const { return _cfg.timer; }

private:
  Config _cfg{}; ///< 运行时配置副本
};

#endif
