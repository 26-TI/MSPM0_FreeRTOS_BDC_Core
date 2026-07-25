/**
 * @file device_cfg.cpp
 * @brief 设备层总配置 — 设备实例与初始化
 */

#include "device_cfg.hpp"
#include "bsp_cfg.hpp"

/* ==================== 设备初始化 ==================== */
void device_init()
{
  /* Track (PCA9555 巡线) — I2C1, 地址 0x20 */
  iic_track.init(&bsp_i2c1, 0x20);
}
