/**
 * @file bsp_key.hpp
 * @brief MSPM0 BspKey — 软件消抖按键（任务轮询模式）
 *
 * @note 引脚由 SysConfig 初始化为 Digital Input（外部上拉/下拉）。
 *       poll() 需在 FreeRTOS 任务中周期性调用（如每 50ms）。
 *
 * @note 消抖公式:
 *       消抖时间 = debounceCnt × 轮询周期
 *       长按时间 = longPressCnt × 轮询周期
 *
 * @note 支持事件:
 *       - PRESSED:  按下确认（消抖后）
 *       - RELEASED: 释放确认（消抖后）
 *       - LONG_PRESS: 持续按住超过阈值（只触发一次，释放后重置）
 *
 * @note 纯裸机操作，无系统阻塞、无中断，不依赖 FreeRTOS。
 *
 * 使用示例:
 * @code
 *   // 配置: 低有效, 50ms 轮询 → 100ms 消抖, 1s 长按
 *   BspKey key;
 *   key.init({GPIOB, DL_GPIO_PIN_13, true, 2, 20});
 *
 *   // 在 50ms 周期任务中:
 *   BspKey::Event e = key.poll();
 *   switch (e) {
 *     case BspKey::PRESSED:    // 短按    break;
 *     case BspKey::LONG_PRESS: // 长按 1s break;
 *     default: break;
 *   }
 * @endcode
 */

#ifndef __BSP_KEY_MSPM0_HPP__
#define __BSP_KEY_MSPM0_HPP__

#include "ti_msp_dl_config.h" // IWYU pragma: keep

class BspKey
{
public:
  /** @brief 按键事件类型 */
  enum Event
  {
    NONE       = 0, ///< 无事件（常态）
    PRESSED    = 1, ///< 按下确认（消抖完成后触发一次）
    RELEASED   = 2, ///< 释放确认（消抖完成后触发一次）
    LONG_PRESS = 3, ///< 长按（持续按住超过 longPressCnt 次轮询，仅触发一次）
  };

  /**
   * @brief 按键配置结构体
   *
   * @note 消抖/长按时间取决于轮询周期:
   * @code
   *   实际消抖时间 = debounceCnt × 轮询周期
   *   实际长按时间 = longPressCnt × 轮询周期
   * @endcode
   * 例: 50ms 轮询 + debounceCnt=2 → 100ms 消抖
   */
  struct Config
  {
    GPIO_Regs *port;                  ///< GPIO 端口 (GPIOA / GPIOB)
    uint32_t   pin;                   ///< 引脚掩码 (DL_GPIO_PIN_x)
    bool       active_low     = true; ///< 有效电平: true=低有效(上拉), false=高有效(下拉)
    uint8_t    debounce_cnt   = 3U;   ///< 消抖计数 (连续相同电平次数)
    uint16_t   long_press_cnt = 200U; ///< 长按计数阈值 (按住持续次数)
  };

  BspKey() = default;

  /**
   * @brief 绑定引脚并初始化消抖状态机
   * @param cfg 引脚 + 消抖/长按参数
   *
   * @note 硬件引脚方向/上下拉由 SysConfig 的 SYSCFG_DL_GPIO_init() 配置，
   *       本函数仅保存参数并读取初始电平。
   */
  void init(const Config &cfg);

  /**
   * @brief 轮询消抖（每周期调用一次）
   *
   * 消抖算法:
   *   1. 读取当前 GPIO 电平
   *   2. 若与上次稳定状态相同 → 清零波动计数
   *   3. 若不同 → 累加计数，达到 debounceCnt 则确认状态变化
   *   4. 按下后累加 holdCnt，达到 longPressCnt 触发 LONG_PRESS（仅一次）
   *
   * @return 本次触发的事件:
   *         - NONE:       无变化
   *         - PRESSED:    刚按下
   *         - RELEASED:   刚释放
   *         - LONG_PRESS: 长按触发
   */
  Event poll();

private:
  GPIO_Regs *_port           = nullptr; ///< GPIO 端口指针
  uint32_t   _pin            = 0U;      ///< 引脚掩码
  bool       _active_low     = true;    ///< 有效电平极性
  uint8_t    _debounce_cnt   = 3U;      ///< 消抖确认次数
  uint16_t   _long_press_cnt = 3U;      ///< 长按计数阈值

  bool     _last_stable = false; ///< 上一次确认的稳定状态 (true=按下)
  uint8_t  _cnt         = 0U;    ///< 当前连续不一致计数
  uint16_t _hold_cnt    = 0U;    ///< 按下保持计数
  bool     _long_fired  = false; ///< 本次按下周期内长按是否已触发
};

#endif /* __BSP_KEY_MSPM0_HPP__ */
