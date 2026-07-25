/**
 * @file bsp_i2c.cpp
 * @brief MSPM0 BspI2c 实现 — I2C 控制器模式（裸机阻塞: while 等标志位）
 *
 * @note 硬件由 SysConfig 生成的 SYSCFG_DL_I2Cx_init() 统一初始化。
 *       本类封装裸机阻塞式 I2C 读写（while 等 BUSY/IDLE/RXFIFO），
 *       适用于 OLED、PCA9555 等低速外设。不依赖中断或 DMA。
 */

#include "bsp_i2c.hpp"

/* ==================== 构造 ==================== */
BspI2c::BspI2c() {}

/* ==================== 初始化 ==================== */

/**
 * @brief 绑定 I2C 硬件实例
 * @param cfg I2C 外设配置
 * @return true  绑定成功
 * @return false i2c 指针为空
 */
bool BspI2c::init(const Config &cfg)
{
  if (!cfg.i2c)
  {
    return false;
  }
  _cfg = cfg;
  return true;
}

/* ==================== 状态查询 ==================== */

void BspI2c::wait_idle()
{
  while (!(DL_I2C_getControllerStatus(_cfg.i2c) & DL_I2C_CONTROLLER_STATUS_IDLE))
  {
    /* 等待 IDLE 标志置位 */
  }
}

void BspI2c::wait_not_busy()
{
  while (DL_I2C_getControllerStatus(_cfg.i2c) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS)
  {
    /* 等待 BUSY 标志清零 */
  }
}

/* ==================== 裸机阻塞: while 等标志位 ==================== */

/**
 * @brief 向 I2C 设备写入数据（裸机阻塞: while 等 BUSY/IDLE）
 * @param dev_addr 7 位设备地址
 * @param data     数据缓冲区
 * @param len      数据长度
 */
void BspI2c::write(uint8_t dev_addr, const uint8_t *data, size_t len)
{
  wait_idle();

  DL_I2C_fillControllerTXFIFO(_cfg.i2c, data, len);
  DL_I2C_startControllerTransfer(_cfg.i2c, dev_addr,
                                 DL_I2C_CONTROLLER_DIRECTION_TX, len);

  wait_not_busy();
  wait_idle();
}

/**
 * @brief 读取设备 8 位寄存器（写地址 → 重启 → 读 1 字节）
 * @param dev_addr 7 位设备地址
 * @param reg      寄存器地址
 * @return 寄存器值
 */
uint8_t BspI2c::read_reg_8(uint8_t dev_addr, uint8_t reg)
{
  uint8_t rx = 0x00U;

  wait_idle();

  /* 写寄存器地址 */
  DL_I2C_fillControllerTXFIFO(_cfg.i2c, &reg, 1U);
  DL_I2C_startControllerTransfer(_cfg.i2c, dev_addr,
                                 DL_I2C_CONTROLLER_DIRECTION_TX, 1U);

  wait_not_busy();
  wait_idle();

  /* 读数据 */
  DL_I2C_startControllerTransfer(_cfg.i2c, dev_addr,
                                 DL_I2C_CONTROLLER_DIRECTION_RX, 1U);

  while (DL_I2C_isControllerRXFIFOEmpty(_cfg.i2c))
  {
    /* 等待 RX FIFO 有数据 */
  }
  rx = DL_I2C_receiveControllerData(_cfg.i2c);

  return rx;
}

/**
 * @brief 读取设备 16 位寄存器（连续读 reg_low 和 reg_low+1）
 * @param dev_addr 7 位设备地址
 * @param reg_low  低地址寄存器
 * @return 16 位值，reg_low 在低字节，reg_low+1 在高字节
 */
uint16_t BspI2c::read_reg_16(uint8_t dev_addr, uint8_t reg_low)
{
  uint8_t  low  = read_reg_8(dev_addr, reg_low);
  uint8_t  high = read_reg_8(dev_addr, (uint8_t)(reg_low + 1U));
  uint16_t val  = ((uint16_t)high << 8U) | (uint16_t)low;
  return val;
}

/* ==================== 流水线式传输（OLED 批量刷新） ==================== */

/**
 * @brief 启动 TX 传输（不等待完成）
 * @param dev_addr 7 位设备地址
 * @param count    本次传输总字节数
 */
void BspI2c::start_tx(uint8_t dev_addr, size_t count)
{
  wait_idle();
  DL_I2C_startControllerTransfer(_cfg.i2c, dev_addr,
                                 DL_I2C_CONTROLLER_DIRECTION_TX, count);
}

bool BspI2c::is_tx_fifo_full()
{
  return DL_I2C_isControllerTXFIFOFull(_cfg.i2c);
}

/**
 * @brief 发送 1 字节（自动等待 FIFO 有空位）
 * @param data 要发送的字节
 * @note 必须在 startTx() 之后、waitTxDone() 之前调用
 */
void BspI2c::tx_byte(uint8_t data)
{
  while (DL_I2C_isControllerTXFIFOFull(_cfg.i2c))
  {
    /* 等待 FIFO 有空位 */
  }
  DL_I2C_transmitControllerData(_cfg.i2c, data);
}

/**
 * @brief 等待 TX 传输完成并恢复空闲
 */
void BspI2c::wait_tx_done()
{
  wait_not_busy();
  wait_idle();
}
