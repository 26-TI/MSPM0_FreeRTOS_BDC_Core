/**
 * @file oled_test.cpp
 * @brief OLED 显示任务 — 12路巡线 + 陀螺仪姿态
 *
 * @note 屏幕布局 (128×64, 4行 × 16字符):
 *       行0: "000000000000    "  12路传感器 (1=白 0=黑)
 *       行1: "Yaw:  180.0"       陀螺仪偏航
 *       行2: "Rol:  180.0"       陀螺仪翻滚
 *       行3: "Pit:   90.0"       陀螺仪俯仰
 */

#include "test.hpp"
#include "device_cfg.hpp"
#include "user_system.hpp"

#include "FreeRTOS.h" // IWYU pragma: keep
#include "task.h"

/* ===================================================================
 * OLED 显示任务
 * =================================================================== */

void oled_test_task(void *arg)
{
  (void)arg;

  iic_oled.clear();
  char buf[17]; /* 16 字符 + '\0' */
  int  len;

  while (1)
  {
    /* ── 行0: 12路巡线传感器 (0=黑线 / 1=白) ── */
    {
      uint16_t raw = g_track_data & 0x0FFFU;
      for (int i = 0; i < 12; i++)
        buf[i] = (raw & (1U << (11U - i))) ? '1' : '0';
      for (int i = 12; i < 16; i++)
        buf[i] = ' '; /* 填充空格清除残留 */
      buf[16] = '\0';
      iic_oled.show_string(0, 0, buf);
    }

    /* ── 陀螺仪 ── */
    const TiGyro::ReportData &r = ti_gyro.get_last_report();

    if (r.valid)
    {
      /* 行1: Yaw */
      len = user_system::format(buf, sizeof(buf), "Yaw:%7.1f", (double)r.yaw_deg);
      while (len < 16) buf[len++] = ' ';
      buf[16] = '\0';
      iic_oled.show_string(0, 1, buf);

      /* 行2: Roll */
      len = user_system::format(buf, sizeof(buf), "Rol:%7.1f", (double)r.roll_deg);
      while (len < 16) buf[len++] = ' ';
      buf[16] = '\0';
      iic_oled.show_string(0, 2, buf);

      /* 行3: Pitch */
      len = user_system::format(buf, sizeof(buf), "Pit:%7.1f", (double)r.pitch_deg);
      while (len < 16) buf[len++] = ' ';
      buf[16] = '\0';
      iic_oled.show_string(0, 3, buf);
    }
    else
    {
      iic_oled.show_string(0, 1, "Yaw:  --.--      ");
      iic_oled.show_string(0, 2, "Rol:  --.--      ");
      iic_oled.show_string(0, 3, "Pit:  --.--      ");
    }

    iic_oled.refresh();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
