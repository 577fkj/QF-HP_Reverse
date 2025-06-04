#include "ADC.h"
#include <Ticker.h>
#include "EC11.h"
#include "pwm.h"
#include "User.h"
#include "user_datas.h"
#include "ring_buffer.h"

typedef enum
{
    CH_639_5K = 0,
    CH_19_5K,
    CH_1_5K,
} adc_v2_ch_t;

ADC adc;
Ticker adc_max_temp_tic;

static uint16_t ch_vol[3];
static adc_v2_ch_t ch_sta = CH_639_5K;

static ring_buffer_handle_t ch_handle[3] = {NULL, NULL, NULL};

/**
 * @brief 设置通道
 *
 * @param ch 0：最大电阻值620+18+1.5，1：18+1.5，2：1.5
 */
static void ch_set(adc_v2_ch_t ch)
{
    if (ch & 0x01)
        digitalWrite(CH1_IO, LOW);
    else
        digitalWrite(CH1_IO, HIGH);
    if (ch & 0x02)
        digitalWrite(CH2_IO, LOW);
    else
        digitalWrite(CH2_IO, HIGH);
    ch_sta = ch;
}

ADC::ADC()
{
    for (size_t i = 0; i < 3; i++)
    {
        ch_handle[i] = ring_buffer_create(sample_16bit, type_unsigned, 10);
        ring_buffer_set_moveing_flitering_en(ch_handle[i], true);
    }
}

void ADC::get()
{
    uint16_t tmp = analogRead(A0);

    if (user_datas.hardware_version == 0)
    {
        ring_buffer_write(ch_handle[adc_mode_state], &tmp);
        set_channel(!adc_mode_state);
        return;
    }
    else
    {
        static uint8_t _ch_sta = CH_639_5K;
        ring_buffer_write(ch_handle[(uint8_t)ch_sta], &tmp);
        ch_set((adc_v2_ch_t)_ch_sta++);
        if (_ch_sta > CH_1_5K)
            _ch_sta = CH_639_5K;
    }
}

void ADC::get_voltage()
{
    if (user_datas.hardware_version == 0)
    {
        for (size_t i = 0; i < 2; i++)
            ring_buffer_get_moveing_flitering(ch_handle[i], &ch_vol[i]);
        vol_low = ch_vol[0] * 1000 / 1024;
        vol_high = ch_vol[1] * 1000 / 1024;
    }
    else
    {
        for (size_t i = 0; i < 3; i++)
            ring_buffer_get_moveing_flitering(ch_handle[i], &ch_vol[i]);
    }
}

int16_t chek_temp(float rt)
{
    double R0 = 100000.0; // 参考电阻值（100kΩ）
    double B = 3950.0;    // Beta值
    double T0 = 298.15;   // 参考温度（25°C，单位为开尔文）

    // 计算温度
    double lnRt = log(rt);
    double lnR0 = log(R0);
    double T = 1.0 / ((lnRt - lnR0) / B + 1.0 / T0);
    // 将开尔文温度转换为摄氏温度
    T -= 273.15;
    return (int16_t)T;
}

void ADC::get_temp_task()
{
    float rt;
    float tt;
    float temp_buf = 0;

    double buf;

    get_voltage();

    float vdd = 3300;

    if (user_datas.hardware_version == 0)
    {
        rt = vol_low * 1000 / ((3300 - vol_low) / 13);
        now_temp = (100 / (log(rt / 10000.0) / 3950 + 1 / 298.15) - 27315) / 100;

        if (now_temp < 151)
            return;

        if (now_temp < 160 && !adc_error)
        {
            if (pwm.power || adc_max_temp_auto_flg == 0)
                temp_buf = now_temp;
        }

        vol_high = vol_high * 1000 / 21;

        buf = vol_high * 1000 / ((3300000 - vol_high) / 13.0);
        tt = (100 / (log(buf / 10000) / 3950 + 1 / 298.15) - 27315) / 100;

        if (temp_buf)
            adc_error = temp_buf - tt;

        if (adc_max_temp_auto_flg)
        {
            int16_t tmp = 150 - adc_error;
            rt = user_datas.adc_hotbed_max_temp - adc_error;
            int16_t tmp1 = user_datas.adc_adc_max_temp - rt;
            buf = float(tmp1) / (float)(user_datas.adc_adc_max_temp - tmp);
            tt = (float)tt - ((float)(tt - tmp) * buf);
        }
        if (adc_max_temp_auto_flg)
            now_temp = tt + adc_error;
        else
            now_temp = tt;
    }
    else
    {
        float temp_f[3] = {(float)ch_vol[0], (float)ch_vol[1], (float)ch_vol[2]};
        uint8_t sta = 0;
        if (temp_f[2] < 1023)
        {
            sta = 2;
            rt = (vdd - temp_f[2]); // 上分压电压
            rt = rt / 1500.0;       // 计算电流
            rt = temp_f[2] / rt;    // 计算电阻
            // rt = temp_f[2] * 1000.0f / ((3300.0 - temp_f[2]) / 0.15);
        }
        else if (temp_f[1] < 1023)
        {
            sta = 1;
            rt = (vdd - temp_f[1]); // 上分压电压
            rt = rt / 19500.0;      // 计算电流
            rt = temp_f[1] / rt;    // 计算电阻
            // rt *= 206.0 / 203.0;
            // rt = temp_f[2] * 1000.0f / ((3300.0 - temp_f[2]) / 0.15);
        }
        else
        {
            rt = (vdd - temp_f[0]); // 上分压电压
            rt = rt / 639500.0;     // 计算电流
            rt = temp_f[0] / rt;    // 计算电阻
            // rt = temp_f[0] * 1000.0f / ((3300.0 - temp_f[0]) / 63.95);
        }

        rt = chek_temp(rt);
        if (sta == 0)
        {
            rt *= 93.0 / 86.0;
        }
        else if (sta == 1)
        {
            rt *= 207.0 / 202.0;
        }
        now_temp = rt;
    }
}

void ADC::set_channel(bool channel)
{
    digitalWrite(switch_io, channel);
    adc_mode_state = channel;
}

void adc_max_temp_auto_feed()
{
    static uint16_t last_temp = 0;
    if (last_temp != adc.now_temp)
    {
        last_temp = adc.now_temp;
    }
    else
    {
        adc.adc_max_temp_auto_flg = 1;
        if (adc.now_temp > 200)
        {
            user_datas.adc_adc_max_temp = adc.now_temp;
            ec11.int_close();
            eeprom.write_flg = 2;
            eeprom.write_task();
            ec11.int_work();
        }
        adc_max_temp_tic.detach();
    }
}

bool ADC::adc_max_temp_auto()
{
    return adc_max_temp_auto_flg;
}
