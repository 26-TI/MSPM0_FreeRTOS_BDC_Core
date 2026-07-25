/**
 * @file bsp_qei.hpp
 * @brief MSPM0 BspQei — 正交编码器（QEI）接口封装
 *
 * @note 硬件初始化由 SysConfig 生成的 SYSCFG_DL_QEI_xxx_init() 完成，
 *       BspQei 仅封装运行时计数读取、方向查询和计数复位。
 *
 * @note QEI 基于 TIMG 定时器（不支持 TIMA），使用 DL_TimerG_* API。
 *
 * 使用示例:
 * @code
 *   BspQei qei;
 *   qei.init({TIMG8});
 *   uint16_t count = qei.get_count();
 *   DL_TIMER_QEI_DIRECTION dir = qei.get_direction();
 * @endcode
 */

#ifndef __BSP_QEI_MSPM0_HPP__
#define __BSP_QEI_MSPM0_HPP__

#include "ti_msp_dl_config.h" // IWYU pragma: keep

class BspQei
{
public:
  /**
   * @brief QEI 初始化配置结构体
   */
  struct Config
  {
    GPTIMER_Regs *timer; ///< 定时器外设实例指针（如 TIMG8），QEI 仅支持 TIMG
  };

  BspQei();

  /**
   * @brief 绑定 QEI 硬件实例并复位计数到中点（32767）
   * @param cfg 包含定时器指针的配置
   * @return true  绑定成功
   * @return false timer 为空
   * @note 初始化时自动将计数器设为 32767（16 位中点），
   *       为正反转各预留约一半的计数范围。
   */
  bool init(const Config &cfg);

  /* ==================== 运行时控制 ==================== */

  /**
   * @brief 读取当前编码器计数值
   * @return 16 位计数值（0 ~ 65535），正转递增、反转递减
   * @note 返回低 16 位，调用者负责处理 16 位溢出
   */
  uint16_t get_count() const;

  /**
   * @brief 读取当前转动方向
   * @return DL_TIMER_QEI_DIR_UP   正转（递增）
   * @return DL_TIMER_QEI_DIR_DOWN 反转（递减）
   */
  DL_TIMER_QEI_DIRECTION get_direction() const;

  /**
   * @brief 设置编码器计数值（归零或预设）
   * @param count 要写入的 16 位计数值
   */
  void set_count(uint16_t count);

  /** @brief 获取绑定的定时器外设指针 */
  GPTIMER_Regs *get_timer() const { return _cfg.timer; }

private:
  Config _cfg{}; ///< 运行时配置副本
};

#endif
