#include "set_wifi.h"
#include "ui.h"
#include "User.h"
#include "user_datas.h"

set_wifi setwifi;

const char conect_wifi[][16] = {"连接至网络", "啟動AT力場"};

bool set_wifi::power_on_conect()
{
  int8_t x;
  uint8_t buf = 0;
  uint8_t count = 0;
  WiFi.setAutoReconnect(true);
  for (x = 32; x > 7; x--)
  {
    oled.choose_clr(12, 0, 104, 4);

    oled.str(12, x, conect_wifi[user_datas.ui_style], 16, 1, 0);

    oled.choose_refresh(12, 0, 104, 4);
    yield();
  }
  WiFi.mode(WIFI_STA);
  Serial.println("Conect to WiFi:");
  Serial.println(WiFi.SSID());
  Serial.println(WiFi.psk());
  WiFi.begin();
  x = 92;

  for (;;)
  {
    if (WiFi.SSID() == "")
      break;
    delay(100);
    count++;
    if (WiFi.status() == WL_CONNECTED)
    {
      buf = 1;
      break;
    }
    if (count == 5)
    {
      count = 0;
      buf++;
      if (buf == 20)
      {
        buf = 0;
        break;
      }
      oled.str(x, 7, ".", 16, 1, 0);
      oled.choose_refresh(92, 0, 24, 4);
      x += 8;
      if (x == 116)
      {
        x = 92;
        oled.choose_clr(92, 0, 24, 4);
      }
    }
  }
  for (x = 32; x > 7; x--)
  {
    oled.choose_clr(8, 0, 120, 4);
    oled.str(12, x, conect_wifi[user_datas.ui_style], 16, 1, 0);
    if (buf)
    {

      if (user_datas.ui_style == UI_STYLE_ZHAOYANG)
      {
        oled.str(24, x, "AT", 16, 1, 1);
        oled.chinese(40, x, "力場全开", 16, 1, 0);
      }
      else
        oled.chinese(48, x, "搞定", 16, 1, 0);
    }
    else
    {
      if (user_datas.ui_style == UI_STYLE_ZHAOYANG)
      {
        oled.str(8, x, "AT", 16, 1, 1);
        oled.chinese(24, x, "力場啟動失败", 16, 1, 0);
      }
      else
        oled.chinese(48, x, "失败", 16, 1, 0);
    }

    oled.choose_refresh(8, 0, 120, 4);
    yield();
  }
  delay(500);
  oled.roll(8, 0, 120, 4, 1, UP, 32);
  return buf;
}
