#ifndef _ADC_H_
#define _ADC_H_

#define switch_io 16
#define CH2_IO 16
#define CH1_IO 0

#define channel_low_temp 0
#define channel_high_temp 1

#include <Arduino.h>
// #include "pwm.h"

void adc_max_temp_auto_feed();

class ADC
{

public:
    ADC();

    void get_temp_task();
    void get();
    void set_channel(bool channel);
    bool adc_max_temp_auto();
    uint16_t now_temp = 0;
    uint16_t now_temp_high = 0;
    bool adc_get_temp_flg = 0;
    bool adc_max_temp_auto_flg = 1;
    float now_temp_f = 0;

private:
    void get_voltage();
    // uint16_t adc_buf[10];
    // uint16_t adc_buf_high[10];
    uint16_t vol_low = 0;
    uint16_t vol_high = 0;
    int8_t adc_error = 0;

    bool adc_mode_state = channel_low_temp;
};

extern ADC adc;

#endif
