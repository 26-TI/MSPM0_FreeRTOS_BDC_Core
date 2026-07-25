/**
 * @file bsp_spi.hpp
 * @brief MSPM0 BspSpi — SPI 主模式全双工字节传输封装（裸机阻塞: while 等 FIFO）
 *
 * @note 硬件初始化由 SysConfig 生成的 SYSCFG_DL_SPI0_init() 完成，
 *       BspSpi 仅封装运行时单字节全双工传输。
 *
 * @note 当前工程使用 SPI0（PA9=PICO, PA10=POCI, PA11=SCLK, PA8=CS0 硬件控制），
 *       Mode 0 (CPOL=0, CPHA=0)，8MHz (80MHz BUSCLK / (2×(1+4)) )。
 *
 * 使用示例:
 * @code
 *   BspSpi spi;
 *   spi.init({SPI0_INST});
 *   uint8_t rx = spi.transfer(0xAA);  // 发 0xAA 同时收 1 字节
 * @endcode
 */

#ifndef __BSP_SPI_MSPM0_HPP__
#define __BSP_SPI_MSPM0_HPP__

#include "ti_msp_dl_config.h" // IWYU pragma: keep

class BspSpi
{
public:
  /**
   * @brief SPI 初始化配置结构体
   */
  struct Config
  {
    SPI_Regs *spi; ///< SPI 外设实例指针（如 SPI0_INST）
  };

  BspSpi();

  /**
   * @brief 绑定 SPI 硬件实例
   * @param cfg SPI 外设配置
   * @return true  绑定成功
   * @return false spi 为空指针
   */
  /**
   * @brief 绑定 SPI 硬件实例
   * @param cfg SPI 外设配置（仅需指定 spi 指针）
   * @return true  绑定成功
   * @return false spi 为空指针
   *
   * @note 调用前必须已执行 SYSCFG_DL_init()，
   *       硬件初始化由 SysConfig 生成的 SYSCFG_DL_SPI0_init() 完成。
   */
  bool init(const Config &cfg);

  /**
   * @brief 获取绑定的 SPI 外设指针
   * @return SPI_Regs 指针，未初始化时可能为 nullptr
   */
  SPI_Regs *get_spi() const { return _cfg.spi; }

  /* ==================== 裸机阻塞: while 等 FIFO ==================== */

  /**
   * @brief 发送 1 字节同时接收 1 字节（裸机阻塞: while 等 TX/RX FIFO）
   * @param tx 要发送的字节
   * @return  同时接收到的字节
   *
   * @note 实现逻辑:
   *         1. 等待 TX FIFO 不满（有空位）
   *         2. 写入发送数据
   *         3. 等待 RX FIFO 不空（有数据可读）
   *         4. 读取接收数据
   * @note SPI Mode 0, 8MHz (80MHz BUSCLK / (2 × (1 + 4)))。
   */
  uint8_t transfer(uint8_t tx);

  /**
   * @brief 仅发送 1 字节，丢弃接收（裸机阻塞）
   * @param tx 要发送的字节
   */
  void send(uint8_t tx) { transfer(tx); }

  /**
   * @brief 仅接收 1 字节（发送哑字节 0x00，裸机阻塞）
   * @return 接收到的字节
   */
  uint8_t recv() { return transfer(0x00); }

  /**
   * @brief 连续发送多个字节（裸机阻塞，每个字节丢弃接收）
   * @param data 数据指针
   * @param len  长度
   */
  void write(const uint8_t *data, size_t len);

  /**
   * @brief 连续接收多个字节（裸机阻塞，发送哑字节 0x00）
   * @param[out] buf 接收缓冲区
   * @param      len 长度
   */
  void read(uint8_t *buf, size_t len);

  /* ==================== CS 软件控制（如未用硬件 CS） ==================== */

  /**
   * @brief 软件拉低 CS（开始选通）
   * @note 若使用硬件 CS（SysConfig 中已配置 CS0 为外设功能），此函数为预留接口。
   *       若需软件 CS 控制，请在此函数中实现对应 GPIO 操作。
   */
  void cs_low();

  /**
   * @brief 软件拉高 CS（结束选通）
   * @note 同上，配合 cs_low() 使用。
   */
  void cs_high();

private:
  Config _cfg{};
};

#endif // __BSP_SPI_MSPM0_HPP__
