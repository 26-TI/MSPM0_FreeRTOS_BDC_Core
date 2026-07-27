/**
 * @file oled_task.cpp
 * @brief OLED display task — 12ch track sensors + gyro attitude
 *
 * Layout (128×64, 4 rows × 16 chars):
 *   Row 0: "000000000000    "  12ch track (1=white 0=black)
 *   Row 1: "Yaw:  180.0"       Gyro yaw
 *   Row 2: "Rol:  180.0"       Gyro roll
 *   Row 3: "Pit:   90.0"       Gyro pitch
 */

#include "task.hpp"
#include "device_cfg.hpp"
#include "test.hpp"        /* g_track_data */
#include "user_system.hpp" /* user_system::format */

#include "FreeRTOS.h"
#include "task.h"

void oled_task(void *arg)
{
  (void)arg;

  iic_oled.clear();

  char buf[17]; /* 16 chars + '\0' */
  int  len;

  while (1)
  {
    /* ── Row 0: 12ch track sensors ── */
    {
      uint16_t raw = g_track_data & 0x0FFFU;
      for (int i = 0; i < 12; i++)
        buf[i] = (raw & (1U << (11U - i))) ? '1' : '0';
      for (int i = 12; i < 16; i++)
        buf[i] = ' ';
      buf[16] = '\0';
      iic_oled.show_string(0, 0, buf);
    }

    /* ── Gyro ── */
    const TiGyro::ReportData &r = ti_gyro.get_last_report();

    if (r.valid)
    {
      /* Row 1: Yaw */
      len = user_system::format(buf, sizeof(buf), "Yaw:%7.1f", (double)r.yaw_deg);
      while (len < 16) buf[len++] = ' ';
      buf[16] = '\0';
      iic_oled.show_string(0, 1, buf);

      /* Row 2: Roll */
      len = user_system::format(buf, sizeof(buf), "Rol:%7.1f", (double)r.roll_deg);
      while (len < 16) buf[len++] = ' ';
      buf[16] = '\0';
      iic_oled.show_string(0, 2, buf);

      /* Row 3: Pitch */
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
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
