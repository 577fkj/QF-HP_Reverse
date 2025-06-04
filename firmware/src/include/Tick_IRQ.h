#ifndef _Tick_IRQ_H_
#define _Tick_IRQ_H_

#include <Arduino.h>

#define oled_display_sleep_time 60 // 屏幕休眠时间 如不需要休眠  可设置为256

void Ticker_init();
void tick_backflow_start();
void tick_backflow_stop();
uint8_t tick_get_backflow_percent();

extern uint8_t oled_flg;
extern int16_t temp_time_buf;
extern int16_t min_count;
extern uint8_t oled_sleep_t;

#endif
