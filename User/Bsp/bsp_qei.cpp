/**
 * @file bsp_qei.cpp
 * @brief MSPM0 BspQei 实现 — 正交编码器（QEI）接口封装
 *
 * @note 硬件初始化（时钟/引脚/QEI模式）由 SysConfig 生成的
 *       SYSCFG_DL_QEI_xxx_init() 在 SYSCFG_DL_init() 中统一完成。
 *       BspQei 仅负责运行时读取计数值、方向及复位操作。
 *
 * @note QEI x4 倍频模式下，每个编码器线产生 4 个计数脉冲，
 *       正转时计数器递增，反转时递减。
 */

#include "bsp_qei.hpp"

/* ==================== 构造 ==================== */
BspQei::BspQei() {}

/* ==================== 初始化 ==================== */

/**
 * @brief 绑定 QEI 硬件实例并初始化计数器
 *
 * @param cfg 包含定时器外设指针的配置
 * @return true  绑定成功
 * @return false timer 为空指针
 *
 * @note 初始化时自动将计数器设为 32767（16 位中点 0x7FFF），
 *       使正反转方向各有约一半的计数范围，减少溢出频率。
 *       硬件初始化和引脚配置需在此之前由 SYSCFG_DL_init() 完成。
 */
bool BspQei::init(const Config &cfg)
{
  if (!cfg.timer)
    return false;

  _cfg = cfg;
  set_count(32767); /* 复位到 16 位中点 */
  return true;
}

/* ==================== 运行时控制 ==================== */

/**
 * @brief 读取当前编码器计数值（低 16 位）
 *
 * @return 16 位无符号计数值，范围 [0, 65535]
 *
 * @note 在 x4 倍频模式下，正转递增、反转递减。
 *       计数器从 65535 溢出到 0（正转）或从 0 下溢到 65535（反转）。
 *       调用者需自行处理 16 位溢出（Motor 类已内置溢出安全处理）。
 */
uint16_t BspQei::get_count() const
{
  return (uint16_t)(DL_TimerG_getTimerCount(_cfg.timer) & 0xFFFFU);
}

/**
 * @brief 读取当前转动方向
 *
 * @return DL_TIMER_QEI_DIR_UP   编码器正转（计数器递增）
 * @return DL_TIMER_QEI_DIR_DOWN 编码器反转（计数器递减）
 *
 * @note 方向由 QEI 硬件根据 A/B 相信号相位自动判定
 */
DL_TIMER_QEI_DIRECTION BspQei::get_direction() const
{
  return DL_TimerG_getQEIDirection(_cfg.timer);
}

/**
 * @brief 设置编码器计数值
 *
 * @param count 要写入的 16 位计数值
 *
 * @note 常用于编码器归零或预设初始位置。
 *       注意：在电机运行中调用可能导致速度计算跳变。
 */
void BspQei::set_count(uint16_t count)
{
  DL_TimerG_setTimerCount(_cfg.timer, count);
}
