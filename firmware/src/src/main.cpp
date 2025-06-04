#include "User.h"

#include "EEPROM.h"

#include "uart_app.h"

#include "qf_key_chek.h"

extern void get_mac(uint8_t *mac_ret);

/*
  启凡科创QF-HP物联网加热台固件源码
  版本:V2.4
  日期:2025-4-21
*/
static const uint8_t enc_iolist[][3] = {
    {5, 4, 2},
    {5, 2, 4},
    {2, 4, 5},
    {2, 5, 4},
};

void setup()
{
  user_datas_init();

  delay(100);

#ifdef DEBUG
  {
    Serial.begin(115200);
  }

#endif

  Serial.println();

  oled.begin();

  eeprom.read_all_data();

  pinMode(16, OUTPUT);
  digitalWrite(16, 0);

  delay(1);

  pinMode(16, INPUT);
  os_delay_us(35);
  user_datas.hardware_version = digitalRead(16);

  pinMode(16, OUTPUT);
  pinMode(0, OUTPUT);

  Ticker_init();

  miot.begin();

  uint8_t sw = enc_iolist[user_datas.encoder_rotation + user_datas.hardware_version * 2][0];
  uint8_t swa = enc_iolist[user_datas.encoder_rotation + user_datas.hardware_version * 2][1];
  uint8_t swb = enc_iolist[user_datas.encoder_rotation + user_datas.hardware_version * 2][2];

  ec11.begin(sw, swa, swb, ui_key_callb, (ec11_rotation_t)user_datas.encoder_rotation);

  ec11.speed_up(true);
  ec11.speed_up_max(20);

  ui.page_switch_flg = true;
}

void loop()
{
  static uint16_t cnt = 0;
  cnt++;
  if (cnt == 0)
    delay(1);
  if (system_get_cpu_freq() != SYS_CPU_160MHZ)
    system_update_cpu_freq(SYS_CPU_160MHZ);
  ui.run_task();
  eeprom.write_task();
  miot.run_task();
  while (Serial.available())
    uart_cmd_get_byte(Serial.read());
  uart_app_handler();
}
