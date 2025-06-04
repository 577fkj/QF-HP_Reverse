#ifndef _EC11_H_
#define _EC11_H_

/*
    Chinese is encoded in Unicode. If it is garbled, please change the encoding method of the editor.
    EC11编码器库 By启凡科创 适用于Arduino平台下ESP8266
    version:v1.2.1  2023-1-21

    版本特性：
        1）相较于上一个版本，修改了事件类型和结果的传参方式，接口更加规范
        2）优化编码纠错算法，进一步规避可能的丢码、错码现象
*/

#include <Arduino.h>
// #include <Ticker.h>

// IO定义  需支持外部中断且能设置为上拉输入的IO
// 如编码方向反了调换A\B即可

typedef enum
{
    sw_clr,
    sw_click,  // 单击
    sw_double, // 双击
    sw_long    // 长按
} ec11_key_result_type;

typedef enum
{
    ec11_task_is_key,   // 按键事件
    ec11_task_is_encode // 编码事件
} ec11_task_result_type;

typedef enum
{
    ec11_cw, // 按键事件
    ec11_ccw // 编码事件
} ec11_rotation_t;

#define key_time_out 300   // 长按多少ms识别为长按
#define key_double_out 400 // 双击间隔在多少ms内有效 如不需要双击功能 值设为1
#define shake_t 5          // 抖动抑制时间  默认5ms

class Skey
{

public:
    /*
        初始化编码器，绑定SW、SA\SB的IO口，如发现旋转方向反了，将SA\SB的位置调换即可
        按键事件回调函数注册
            ec11_task_result_type:
                ec11_task_is_key,   //按键事件
                ec11_task_is_encode //编码事件
                用于回调函数内判断事件输入类型
            rusult_value：
                如果为按键事件，则值为
                    sw_click,  //单击
                    sw_double, //双击
                    sw_long    //长按
                如果为编码事件，则值为一般逆时针负数顺时针正数
            ec11_rotation_t:注册时的旋转方向，默认cw
    */
    void begin(uint8_t sw, uint8_t sa, uint8_t sb, void (*func)(ec11_task_result_type task_type, int16_t rusult_value), ec11_rotation_t rotation = ec11_cw);

    /**
     * @brief 更换旋转方向
     */
    void change_rotation();

    /**
     * @brief 设置旋转方向
     *
     * @param rotation
     */
    void set_rotation(ec11_rotation_t rotation);

    /**
     * @brief 获取旋转方向
     *
     * @return ec11_rotation_t
     */
    ec11_rotation_t get_rotation();

    // 按键事件注销
    void detach();

    // 启用按键中断  写入EEPROM时必须关闭
    void int_work();
    // 关闭按键中断
    void int_close();

    // 加速算法是否启用 val:true/false 默认关闭false
    void speed_up(bool val);

    // 加速算法最大加速值,1-65535  默认100
    void speed_up_max(uint16_t val);

    // 加速算法手感  默认15  0-255
    void speed_up_feel(uint8_t val);

    // 双击识别是否启用 val:true/false 默认开启true
    void double_click(bool val);

    /*
        回调函数触发任务后，用于获取按键输入类型，回调函数结束后将清零
        sw_click,   //单击
        sw_double,  //双击
        sw_long     //长按
    */
    ec11_key_result_type get_key_result();

    /*
        回调函数触发任务后，用于获取旋转编码的输入值，回调函数结束后将清零
        一般逆时针负数顺时针正数
    */
    int16_t get_encode_value();

private:
    void (*attch_p)(ec11_task_result_type task_type, int16_t rusult_value) = NULL;
    void task();
    void key_mode_read();
    friend void io_sw_int();
    friend void io_sa_sb_int();
    friend void key_timer();
    bool work_flg = false;
    bool state = 1;
    bool state_buf = 1;
    uint8_t state_count = 0;
    bool speed_up_flg = false;
    uint16_t speed_up_max_num = 100;
    uint8_t speed_up_feel_count = 15;
    uint16_t double_click_count = key_double_out;
    ec11_task_result_type _take_type_ec_sw = ec11_task_is_key;

    // 编码值调用识别 未启用加速算法:ec_left左转(值-1) ec_right右转(值1)   启用加速算法后值为±1至±speed_up_max()所设置值
    int16_t ec_buf = 0;
    // 按键值调用识别 sw_click单击 sw_double双击 sw_long长按
    ec11_key_result_type sw_buf = sw_clr;
    ec11_rotation_t _rotation = ec11_cw;
};

extern Skey ec11;

#endif
