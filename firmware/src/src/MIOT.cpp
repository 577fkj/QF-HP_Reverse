#include "MIOT.h"
#include "Blinker.h"
#include "User.h"


MIOT miot;

BlinkerButton Button1(BUTTON_1);
BlinkerButton Button2(BUTTON_2);
BlinkerButton Button3(BUTTON_3);
BlinkerButton Button4(BUTTON_4);
BlinkerButton Button5(BUTTON_5);
BlinkerSlider Slider1(Slider_1);

void button1_callback(const String &state) // 按键事件
{
    if (!pwm.power)
    {
        pwm.begin();
        ui.oled_display_set();
        BlinkerMIOT.powerState("on");
        BlinkerMIOT.print();
    }
}

void button2_callback(const String &state) // 按键事件
{
    if (pwm.power)
    {
        pwm.end();
        ui.oled_display_set();
        BlinkerMIOT.powerState("off");
        BlinkerMIOT.print();
    }
}
void button3_callback(const String &state) // 按键事件
{
    if (user_datas.pwm_temp_mode == 0)
    {
        user_datas.pwm_temp_mode = 1;
        ui.temp_mode_flg = 1;
        if (pwm.power)
        {
            pwm.end();
            eeprom.write_flg = 1;
            eeprom.write_t = 0;
        }
        ui.oled_display_set();
    }
}

void button4_callback(const String &state) // 按键事件
{
    if (user_datas.pwm_temp_mode)
    {
        user_datas.pwm_temp_mode = 0;
        ui.temp_mode_flg = 1;
        if (pwm.power)
        {
            pwm.end();
            eeprom.write_flg = 1;
            eeprom.write_t = 0;
        }
        ui.oled_display_set();
    }
}

void button5_callback(const String &state) // 按键事件
{
    pwm.fan_state = !pwm.fan_state;
    pwm.fan(pwm.fan_state);
}

void slider1_callback(int32_t value) // 滑块处理
{
    if (user_datas.pwm_temp_mode)
    {
        if (value < 40)
            value = 40;
        else if (value > 250)
            value = 250;
        user_datas.pwm_temp_buf = value;

        ui.temp_move_flg = 1;
        eeprom.write_t = 0;
        eeprom.write_flg = 1;
        ui.show_temp_mode = show_set_temp;
        ui.oled_display_set();
    }
}

void miotPowerState(const String &state) // 电源事件
{

    if (state == BLINKER_CMD_ON)
    {
        if (!pwm.power)
        {
            pwm.begin();
            ui.oled_display_set();
            BlinkerMIOT.powerState("on");
            BlinkerMIOT.print();
        }
    }
    else if (state == BLINKER_CMD_OFF)
    {
        if (pwm.power)
        {
            pwm.end();
            ui.oled_display_set();
            BlinkerMIOT.powerState("off");
            BlinkerMIOT.print();
        }
    }
}

void miotColor(int32_t color)
{
    if (user_datas.pwm_temp_mode)
    {
        color &= 0xffffff;

        switch (color)
        {
        case 9498256:
            user_datas.pwm_temp_buf = qianlvse;
            break;
        case 16711935:
            user_datas.pwm_temp_buf = zihongse;
            break;
        case 16761035:
            user_datas.pwm_temp_buf = fenhongse;
            break;
        case 16738740:
            user_datas.pwm_temp_buf = fense;
            break;
        case 10494192:
            user_datas.pwm_temp_buf = zise;
            break;
        case 8900331:
            user_datas.pwm_temp_buf = tianlanse;
            break;
        case 255:
            user_datas.pwm_temp_buf = lanse;
            break;
        case 65535:
            user_datas.pwm_temp_buf = qingse;
            break;
        case 65280:
            user_datas.pwm_temp_buf = lvse;
            break;
        case 16753920:
            user_datas.pwm_temp_buf = juhuangse;
            break;
        case 16776960:
            user_datas.pwm_temp_buf = huangse;
            break;
        case 16711680:
            user_datas.pwm_temp_buf = hongse;
            break;
        }
        ui.temp_move_flg = 1;
        eeprom.write_t = 0;
        eeprom.write_flg = 1;
        ui.show_temp_mode = show_set_temp;
        ui.oled_display_set();
        BlinkerMIOT.color(color);
        BlinkerMIOT.print();
    }
}

void miotMode(uint8_t mode)
{

    if (mode == BLINKER_CMD_MIOT_DAY)
    { // 日光模式
        if (user_datas.pwm_temp_mode)
        {
            user_datas.pwm_temp_buf = MIOT_DAY;
            ui.temp_move_flg = 1;
            ui.show_temp_mode = show_set_temp;
            eeprom.write_t = 0;
            eeprom.write_flg = 1;
            ui.oled_display_set();
        }
    }
    else if (mode == BLINKER_CMD_MIOT_NIGHT)
    { // 夜光模式
        if (user_datas.pwm_temp_mode)
        {
            user_datas.pwm_temp_mode = 0;
            ui.temp_mode_flg = 1;
            ui.oled_display_set();
        }
    }
    else if (mode == BLINKER_CMD_MIOT_COLOR)
    { // 彩光模式
        if (!user_datas.pwm_temp_mode)
        {
            user_datas.pwm_temp_mode = 1;
            ui.temp_mode_flg = 1;
            ui.oled_display_set();
        }
    }
    else if (mode == BLINKER_CMD_MIOT_WARMTH)
    { // 温馨模式
        if (!user_datas.pwm_temp_mode)
        {
            user_datas.pwm_temp_mode = 1;
            ui.temp_mode_flg = 1;
            if (pwm.power)
            {
                pwm.end();
                eeprom.write_flg = 1;
                eeprom.write_t = 0;
            }
            ui.oled_display_set();
        }

        user_datas.pwm_temp_buf = 100;
        ui.temp_move_flg = 1;
        eeprom.write_t = 0;
        eeprom.write_flg = 1;
        ui.show_temp_mode = show_set_temp;
        ui.oled_display_set();

        if (!pwm.power)
        {
            pwm.begin();
            ui.oled_display_set();
        }
    }
    else if (mode == BLINKER_CMD_MIOT_TV)
    { // 电视模式
        if (user_datas.pwm_temp_mode)
        {
            user_datas.pwm_temp_mode = 0;
            ui.temp_mode_flg = 1;
            if (pwm.power)
            {
                pwm.end();
                eeprom.write_flg = 1;
                eeprom.write_t = 0;
            }
            ui.oled_display_set();
        }
        if (!pwm.power)
        {
            pwm.begin();
            ui.oled_display_set();
        }
    }
    else if (mode == BLINKER_CMD_MIOT_READING)
    { // 阅读模式
        if (!user_datas.pwm_temp_mode)
        {
            user_datas.pwm_temp_mode = 1;
            ui.temp_mode_flg = 1;
            if (pwm.power)
            {
                pwm.end();
                eeprom.write_flg = 1;
                eeprom.write_t = 0;
            }
            ui.oled_display_set();
        }

        user_datas.pwm_temp_buf = 240;
        ui.temp_move_flg = 1;
        eeprom.write_t = 0;
        eeprom.write_flg = 1;
        ui.show_temp_mode = show_set_temp;
        ui.oled_display_set();

        if (!pwm.power)
        {
            pwm.begin();
            ui.oled_display_set();
        }
    }
    else if (mode == BLINKER_CMD_MIOT_COMPUTER)
    { // 电脑模式
    }
    BlinkerMIOT.mode(mode);
    BlinkerMIOT.print();
}

void MIOT::begin()
{

    if (user_datas.miot_miot_able)
    {
        open_flg = 1;
        if (WiFi.status() != WL_CONNECTED)
        {
            wifi_conect_flg = setwifi.power_on_conect();
            if (!wifi_conect_flg)
            {
                Serial.println("connect wifi error!");
                return;
            }
            Serial.println("connect wifi ok!");
        }
    }
    else
        return;

    if (strlen(user_datas.blinker_id) == 12)
    {
        Serial.println("blinker start!");
        Blinker.begin(user_datas.blinker_id, (const char *)&WiFi.SSID()[0], (const char *)&WiFi.psk()[0]);
    }
    else
    {
        Serial.println("blinker id error!");
        return;
    }

#ifdef DEBUG
    {
        BLINKER_DEBUG.stream(Serial);
    }
#else
    {
        const char *p = &wifima.blinker_id[0];
        Blinker.begin(p);
    }
#endif

    Button1.attach(button1_callback);
    Button2.attach(button2_callback);
    Button3.attach(button3_callback);
    Button4.attach(button4_callback);
    Button5.attach(button5_callback);
    BlinkerMIOT.attachPowerState(miotPowerState);
    BlinkerMIOT.attachColor(miotColor);
    Slider1.attach(slider1_callback);
    BlinkerMIOT.attachMode(miotMode);
}

void MIOT::run_task()
{
    if (user_datas.miot_miot_able && wifi_conect_flg)
    {
        Blinker.run();
    }
}
