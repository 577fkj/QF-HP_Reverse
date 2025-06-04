#include "EC11.h"
#include "User.h"

Skey ec11;
static Ticker key_tic;

static int8_t io_sw = -1;
static int8_t io_sa = -1;
static int8_t io_sb = -1;

static uint16_t key_time = 10000;
static uint16_t double_time = 10000;
static uint8_t key_scan_buf = 0;
static uint8_t last_buf = 0x03;

static uint8_t speed_up_feel_num = 0;
static uint16_t speed_up_count = 0;

static uint8_t fist_flg = 1;

void Skey::key_mode_read()
{
    if (fist_flg == 1)
    {
        fist_flg = 0;
        key_scan_buf = 0;
        return;
    }
    if (key_scan_buf < 20)
    {

        if (speed_up_flg)
            speed_up_feel_num = speed_up_feel_count;
        ec11.ec_buf = -1 - speed_up_count;
        _take_type_ec_sw = ec11_task_is_encode;
        task();
    }
    else
    {
        if (speed_up_flg)
            speed_up_feel_num = speed_up_feel_count;
        ec11.ec_buf = 1 + speed_up_count;
        _take_type_ec_sw = ec11_task_is_encode;
        task();
    }
    key_scan_buf = 0;
}

void key_timer()
{

    if (ec11.state_count < 255)
    {
        ec11.state_count++;
        if (ec11.state_count == shake_t)
            ec11.state = ec11.state_buf;
    }
    if (!ec11.state && key_time < 10000)
    {
        key_time++;
        if (key_time == key_time_out)
        {
            ec11.sw_buf = sw_long;
            ec11._take_type_ec_sw = ec11_task_is_key;
            ec11.task();
        }
    }

    if (double_time < 10000)
    {
        double_time++;
        if (double_time == ec11.double_click_count)
        {
            double_time = 10000;
            ec11.sw_buf = sw_click;
            ec11._take_type_ec_sw = ec11_task_is_key;
            ec11.task();
        }
    }

    if (ec11.state)
    {
        if (key_time > 5 && key_time < key_time_out)
        {
            if (ec11.sw_buf == sw_clr && double_time > key_double_out)
            {
                double_time = 0;
            }
            else if (double_time < key_double_out)
            {
                double_time = 10000;
                ec11.sw_buf = sw_double;
                ec11._take_type_ec_sw = ec11_task_is_key;
                ec11.task();
            }
        }
        key_time = 0;
    }

    if (speed_up_feel_num)
    {
        speed_up_feel_num--;
        if (speed_up_count < (ec11.speed_up_max_num - 1))
            speed_up_count++;
    }
    else
    {
        speed_up_count = 0;
    }
}

IRAM_ATTR void io_sw_int()
{
    ec11.state_buf = digitalRead(io_sw);
    ec11.state_count = 0;
}

IRAM_ATTR void io_sa_sb_int()
{
    uint8_t tmp = digitalRead(io_sb) << 1;
    tmp |= digitalRead(io_sa);

    if (tmp != 3)
    {
        if (tmp == 0)
        {
            key_scan_buf &= 0x3f;
        }
        else
        {
            if (last_buf != tmp)
            {
                last_buf = tmp;
                if (key_scan_buf & 0xc0)
                {
                    key_scan_buf |= tmp;
                }
                else
                {
                    key_scan_buf |= tmp << 4;
                    ec11.key_mode_read();
                    key_scan_buf = 0xc0;
                }
            }
        }
    }
}

void Skey::detach()
{
    key_tic.detach();
    work_flg = 0;
    int_close();
    attch_p = NULL;
}

void Skey::task()
{
    if (attch_p == NULL)
        return;
    if (_take_type_ec_sw == ec11_task_is_key)
        attch_p(_take_type_ec_sw, sw_buf);
    else
        attch_p(_take_type_ec_sw, ec_buf);
    sw_buf = sw_clr;
    ec_buf = 0;
}

void Skey::int_work()
{
    if (work_flg)
    {
        attachInterrupt(io_sw, io_sw_int, CHANGE);
        attachInterrupt(io_sa, io_sa_sb_int, CHANGE);
        attachInterrupt(io_sb, io_sa_sb_int, CHANGE);
    }
}

void Skey::int_close()
{
    detachInterrupt(io_sw);
    detachInterrupt(io_sa);
    detachInterrupt(io_sb);
}

void Skey::begin(uint8_t sw, uint8_t sa, uint8_t sb, void (*func)(ec11_task_result_type task_type, int16_t rusult_value), ec11_rotation_t rotation)
{
    io_sw = sw;
    io_sa = sa;
    io_sb = sb;
    _rotation = rotation;
    pinMode(sw, INPUT_PULLUP);
    pinMode(sa, INPUT_PULLUP);
    pinMode(sb, INPUT_PULLUP);

    attch_p = func;
    work_flg = 1;
    key_time = 10000;
    double_time = 10000;
    key_scan_buf = 0;
    last_buf = 0;
    if (io_sw != -1)
        pinMode(io_sw, INPUT_PULLUP);
    if (io_sa != -1)
        pinMode(io_sa, INPUT_PULLUP);
    if (io_sb != -1)
        pinMode(io_sb, INPUT_PULLUP);
    int_work();
    key_tic.attach_ms(1, key_timer);
}

void Skey::change_rotation()
{
    int_close();
    io_sa ^= io_sb;
    io_sb ^= io_sa;
    io_sa ^= io_sb;
    int_work();
    if (_rotation == ec11_cw)
    {
        _rotation = ec11_ccw;
    }
    else
    {
        _rotation = ec11_cw;
    }
}

void Skey::set_rotation(ec11_rotation_t rotation)
{
    if (_rotation == rotation)
        return;
    change_rotation();
}

ec11_rotation_t Skey::get_rotation()
{
    return _rotation;
}

void Skey::speed_up(bool val)
{
    speed_up_flg = val;
}

void Skey::speed_up_max(uint16_t val)
{
    speed_up_max_num = val;
}

void Skey::speed_up_feel(uint8_t val)
{
    speed_up_feel_count = val;
}

void Skey::double_click(bool val)
{
    if (val)
        double_click_count = key_double_out;
    else
        double_click_count = 1;
}

ec11_key_result_type Skey::get_key_result()
{
    return sw_buf;
}

int16_t Skey::get_encode_value()
{
    return ec_buf;
}
