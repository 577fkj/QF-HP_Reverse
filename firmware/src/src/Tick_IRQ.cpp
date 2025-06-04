#include "Tick_IRQ.h"
#include "User.h"

Ticker ticker25ms;
Ticker ticker100_ms;
Ticker ticker50_ms;
Ticker ticker1s;

uint8_t oled_flg = 0;

int16_t temp_time_buf = 0;
int16_t min_count = 0;
uint8_t oled_sleep_t = 0;

typedef enum
{
  backflow_idle,
  backflow_preheating_uping,
  backflow_preheating_hold,
  backflow_uping,
  backflow_hold,
} backflow_state_t;

static backflow_state_t backflow_sta = backflow_idle;
static uint8_t backflow_percent = 0;

uint8_t tick_get_backflow_percent()
{
  return backflow_percent;
}

void tick_backflow_start()
{
  backflow_sta = backflow_preheating_uping;
  backflow_percent = 0;
}

void tick_backflow_stop()
{
  backflow_sta = backflow_idle;
}

void s1_tic()
{

  int16_t tmp;

  adc.get_temp_task(); // 更新温度
  pwm.temp_set();      // 输出pwm

  if (adc.now_temp <= 38 && !pwm.power) // 未开启加热温度最低
  {
    if (pwm.get_fan_sta() == 1 && user_datas.fan_auto_flg == 1) // 自动关闭风扇开启
      pwm.fan(0);                                               // 关闭风扇

    oled_sleep_t++;                                                    // 息屏时间累加
    if (oled_sleep_t == oled_display_sleep_time && !ui.oled_sleep_flg) // 到达息屏时间
    {
      ui.wake_sleep_change_flg = 1; // 息屏
    }
    return;
  }
  else // 温度高或加热中
  {
    oled_sleep_t = 0; // 休眠累加
  }

  // 累加1s
  min_count++;
  if (pwm.temp_reached_flg == true) // 倒计时阶段
  {
    if (min_count == 60) // 1分钟
    {
      min_count = 0;
      temp_time_buf--; // 倒计时减一
    }
  }

  ////////回流百分比//////////////////

  if (backflow_sta == backflow_preheating_uping)
  {
    tmp = (user_datas.curve_temp_buf[0] - 39) * 10 / 25;
    if (tmp == 0)
      tmp = 1;
    tmp = (adc.now_temp - 38) * 10 / tmp;
    if (tmp > 25)
      tmp = 25;
    if (backflow_percent < tmp)
      backflow_percent = tmp;
    if (pwm.backflow_temp_tmp < user_datas.curve_temp_buf[0])
    {
      pwm.backflow_temp_tmp += 3;
      if (pwm.backflow_temp_tmp > user_datas.curve_temp_buf[0])
        pwm.backflow_temp_tmp = user_datas.curve_temp_buf[0];
    }
    else if (adc.now_temp >= pwm.backflow_temp_tmp)
    {
      backflow_sta = backflow_preheating_hold;
      min_count = 0;
    }
    return;
  }

  if (backflow_sta == backflow_preheating_hold)
  {
    tmp = user_datas.curve_temp_buf[1] * 10 / 25;
    if (tmp == 0)
      tmp = 1;
    backflow_percent = 25 + min_count * 10 / tmp;
    if (min_count == user_datas.curve_temp_buf[1])
    {
      backflow_sta = backflow_uping;
      pwm.backflow_temp_tmp = user_datas.curve_temp_buf[2];
    }
  }
  else if (backflow_sta == backflow_uping)
  {

    if (user_datas.curve_temp_buf[2] < 217)
      tmp = (user_datas.curve_temp_buf[2] - user_datas.curve_temp_buf[0]) * 10 / 25;
    else
      tmp = (217 - user_datas.curve_temp_buf[0]) * 10 / 25;
    if (tmp == 0)
      tmp = 1;
    tmp = 50 + (adc.now_temp - user_datas.curve_temp_buf[0]) * 10 / tmp;
    if (tmp > 75)
      tmp = 75;
    if (backflow_percent < tmp)
      backflow_percent = tmp;
    if (adc.now_temp >= 217 || adc.now_temp >= user_datas.curve_temp_buf[2])
    {
      backflow_sta = backflow_hold;
      min_count = 0;
    }
  }
  else if (backflow_sta == backflow_hold)
  {
    tmp = user_datas.curve_temp_buf[3] * 10 / 25;
    if (tmp == 0)
      tmp = 1;
    backflow_percent = 75 + min_count * 10 / tmp;

    if (min_count == (user_datas.curve_temp_buf[3] - 10))
    {
      pwm.backflow_temp_tmp = 0;
    }
    else if (min_count == user_datas.curve_temp_buf[3])
    {
      backflow_sta = backflow_idle;
      backflow_percent = 100;
      pwm.end();
    }
  }
}

void ms25_tic()
{
  oled_flg = 1;
}

void ms50_tic()
{
  adc.get();
}

void ms100_tic()
{

  if (eeprom.write_t < 21)
  {
    eeprom.write_t++;

    if (eeprom.write_t == 20)
    {
      if (eeprom.write_flg)
        eeprom.write_flg = 2;
      if (ui.show_temp_mode == show_set_temp)
      {
        ui.show_temp_mode = show_now_temp;
        ui.temp_move_flg = 1;
      }
    }
  }
}

void Ticker_init()
{
  ticker25ms.attach_ms(25, ms25_tic);
  ticker100_ms.attach_ms(100, ms100_tic);

  if (user_datas.hardware_version == 0)
    ticker50_ms.attach_ms(50, ms50_tic);
  else
    ticker50_ms.attach_ms(33, ms50_tic);

  ticker1s.attach(1, s1_tic);
}
