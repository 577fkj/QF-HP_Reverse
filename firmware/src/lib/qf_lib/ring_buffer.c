#include "ring_buffer.h"

#if ring_buffer_compile_en

ring_buffer_handle_t ring_buffer_create(ring_buffer_sample_t sample, ring_buffer_signed_t signed_flg, size_t size)
{
    ring_buffer_t *handle = NULL;
    void *buffer = NULL;
    int len = sample;

#if ring_buffer_suppot_float
    if (sample == sample_float)
        len = sizeof(float);
#endif

    len *= size;

    handle = ring_buffer_malloc(sizeof(ring_buffer_t));
    if (handle == NULL)
        return NULL;

    buffer = ring_buffer_malloc(len);
    if (buffer == NULL)
    {
        ring_buffer_free(handle);
        return NULL;
    }

    memset(buffer, 0, len);

    handle->_buffer = buffer;

#if ring_buffer_suppot_8bit
#if ring_buffer_suppot_unsigned
    handle->buffer_8 = (uint8_t *)buffer;
#endif
#if ring_buffer_suppot_signed
    handle->buffer_8_s = (int8_t *)buffer;
#endif
#endif
#if ring_buffer_suppot_16bit
#if ring_buffer_suppot_unsigned
    handle->buffer_16 = (uint16_t *)buffer;
#endif
#if ring_buffer_suppot_signed
    handle->buffer_16_s = (int16_t *)buffer;
#endif
#endif
#if ring_buffer_suppot_32bit
#if ring_buffer_suppot_unsigned
    handle->buffer_32 = (uint32_t *)buffer;
#endif
#if ring_buffer_suppot_signed
    handle->buffer_32_s = (int32_t *)buffer;
#endif
#endif
#if ring_buffer_suppot_float
    handle->buffer_float = (float *)buffer;
#endif

#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
    handle->signed_flg = signed_flg;
#endif

#if ring_buffer_moving_flitering_en
    handle->moveing_flitering_flg = 0;
#if ring_buffer_suppot_8bit || ring_buffer_suppot_16bit || ring_buffer_suppot_32bit
#if ring_buffer_suppot_signed
    handle->sum_s = 0;
#endif
#if ring_buffer_suppot_unsigned
    handle->sum = 0;
#endif
#endif
#if ring_buffer_suppot_float
    handle->sum_f = 0;
#endif
#endif

    handle->add_p = 0;
    handle->read_p = 0;
    handle->size = size;
    handle->type = sample;
    handle->count = 0;

    return handle;
}

void ring_buffer_write(ring_buffer_handle_t handle, void *dat)
{
    ////////////////////////////滑动滤波删除旧数据
#if ring_buffer_moving_flitering_en
#if ring_buffer_suppot_signed || ring_buffer_suppot_unsigned
    int32_t _var = 0;
#endif
#if ring_buffer_suppot_float
    float _f_var = 0;
#endif
    if (handle->moveing_flitering_flg) // 开启了滑动滤波
    {
#if ring_buffer_suppot_signed || ring_buffer_suppot_float

#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
        if (handle->signed_flg == 1) // 有符号
        {
#endif
            switch (handle->type)
            {
#if ring_buffer_suppot_8bit
            case sample_8bit:
                handle->sum_s -= handle->buffer_8_s[handle->add_p];
                break;
#endif // # ring_buffer_suppot_8bit
#if ring_buffer_suppot_16bit
            case sample_16bit:
                handle->sum_s -= handle->buffer_16_s[handle->add_p];
                break;
#endif // # ring_buffer_suppot_16bit
#if ring_buffer_suppot_32bit
            case sample_32bit:
                handle->sum_s -= handle->buffer_32_s[handle->add_p];
                break;
#endif // # ring_buffer_suppot_32bit

#if ring_buffer_suppot_float
            case sample_float:
                handle->sum_f -= handle->buffer_float[handle->add_p];
                break;
#endif // # ring_buffer_suppot_float
            default:
                break;
            }
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
        }
#endif

#endif // # ring_buffer_suppot_signed

#if ring_buffer_suppot_unsigned
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
        if (handle->signed_flg == 0)
        {
#endif // # ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
            switch (handle->type)
            {
#if ring_buffer_suppot_8bit
            case sample_8bit:
                handle->sum -= handle->buffer_8[handle->add_p];
                break;
#endif // # ring_buffer_suppot_8bit
#if ring_buffer_suppot_16bit
            case sample_16bit:
                handle->sum -= handle->buffer_16[handle->add_p];
                break;
#endif // # ring_buffer_suppot_16bit
#if ring_buffer_suppot_32bit
            case sample_32bit:
                handle->sum -= handle->buffer_32[handle->add_p];
                break;
#endif // # ring_buffer_suppot_32bit
            default:
                break;
            }
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
        }
#endif
#endif // # ring_buffer_suppot_unsigned
    }
#endif // # ring_buffer_moving_flitering_en

    //////////////////写入数据///////////////////////

#if ring_buffer_suppot_signed || ring_buffer_suppot_float
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
    if (handle->signed_flg == 1) // 有符号
    {
#endif
        switch (handle->type)
        {
#if ring_buffer_suppot_8bit
        case sample_8bit:
            handle->buffer_8_s[handle->add_p] = *(int8_t *)dat;
#if ring_buffer_moving_flitering_en
            if (handle->moveing_flitering_flg == 1)
                _var = handle->buffer_8_s[handle->add_p];
#endif
            break;
#endif

#if ring_buffer_suppot_16bit
        case sample_16bit:
            handle->buffer_16_s[handle->add_p] = *(int16_t *)dat;
#if ring_buffer_moving_flitering_en
            if (handle->moveing_flitering_flg == 1)
                _var = handle->buffer_16_s[handle->add_p];
#endif
            break;
#endif

#if ring_buffer_suppot_32bit
        case sample_32bit:
            handle->buffer_32_s[handle->add_p] = *(int32_t *)dat;
#if ring_buffer_moving_flitering_en
            if (handle->moveing_flitering_flg == 1)
                _var = handle->buffer_32_s[handle->add_p];
#endif
            break;
#endif

#if ring_buffer_suppot_float
        case sample_float:
            handle->buffer_float[handle->add_p] = *(float *)dat;
#if ring_buffer_moving_flitering_en
            if (handle->moveing_flitering_flg == 1)
                _f_var = handle->buffer_float[handle->add_p];
#endif
            break;
#endif

        default:
            break;
        }
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
    }
#endif
#endif // # ring_buffer_suppot_signed

#if ring_buffer_suppot_unsigned
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
    if (handle->signed_flg == 0)
    {
#endif
        switch (handle->type)
        {
#if ring_buffer_suppot_8bit
        case sample_8bit:
            handle->buffer_8[handle->add_p] = *(uint8_t *)dat;
#if ring_buffer_moving_flitering_en
            if (handle->moveing_flitering_flg == 1)
                _var = handle->buffer_8[handle->add_p];
#endif
            break;
#endif

#if ring_buffer_suppot_16bit
        case sample_16bit:
            handle->buffer_16[handle->add_p] = *(uint16_t *)dat;
#if ring_buffer_moving_flitering_en
            if (handle->moveing_flitering_flg == 1)
                _var = handle->buffer_16[handle->add_p];
#endif
            break;
#endif

#if ring_buffer_suppot_32bit
        case sample_32bit:
            handle->buffer_32[handle->add_p] = *(uint32_t *)dat;
#if ring_buffer_moving_flitering_en
            if (handle->moveing_flitering_flg == 1)
                _var = handle->buffer_32[handle->add_p];
#endif
            break;
#endif

#if ring_buffer_suppot_float
        case sample_float:
            handle->buffer_float[handle->add_p] = *(float *)dat;
#if ring_buffer_moving_flitering_en
            if (handle->moveing_flitering_flg == 1)
                _f_var = handle->buffer_float[handle->add_p];
#endif
            break;
#endif

        default:
            break;
        }
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
    }
#endif
#endif // # ring_buffer_suppot_unsigned

#if ring_buffer_moving_flitering_en
    if (handle->moveing_flitering_flg)
    {
#if ring_buffer_suppot_float
        if (handle->type == sample_float)
        {
            handle->sum_f += _f_var;
        }
        else
        {
#endif
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
            if (handle->signed_flg)
#endif
#if ring_buffer_suppot_signed
                handle->sum_s += _var;
#endif
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
            else
#endif
#if ring_buffer_suppot_unsigned
                handle->sum += _var;
#endif
#if ring_buffer_suppot_float
        }
#endif
    }
#endif

    if (handle->count < handle->size)
        handle->count++;
    handle->add_p++;
    if (handle->add_p == handle->size)
        handle->add_p = 0;
}

void ring_buffer_init_value(ring_buffer_handle_t handle, void *var)
{
    size_t i;
    for (i = 0; i < handle->size; i++)
        ring_buffer_write(handle, var);
}

#if ring_buffer_moving_flitering_en

void ring_buffer_set_moveing_flitering_en(ring_buffer_handle_t handle, uint8_t enable)
{
    handle->moveing_flitering_flg = enable;
#if ring_buffer_suppot_signed
    handle->sum_s = 0;
#endif
#if ring_buffer_suppot_unsigned
    handle->sum = 0;
#endif
#if ring_buffer_suppot_float
    handle->sum_f = 0;
#endif
}

void ring_buffer_get_moveing_flitering(ring_buffer_handle_t handle, void *dat)
{
#if ring_buffer_suppot_float
    if (handle->type == sample_float)
    {
        *(float *)dat = handle->sum_f / (float)handle->size;
        return;
    }
#endif

#if ring_buffer_suppot_signed
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
    if (handle->signed_flg)
    {
#endif
        switch (handle->type)
        {
#if ring_buffer_suppot_8bit
        case sample_8bit:
            *(int8_t *)dat = handle->sum_s / (int8_t)handle->size;
            break;
#endif

#if ring_buffer_suppot_16bit
        case sample_16bit:
            *(int16_t *)dat = handle->sum_s / (int16_t)handle->size;
            break;
#endif

#if ring_buffer_suppot_32bit
        case sample_32bit:
            *(int32_t *)dat = handle->sum_s / (int32_t)handle->size;
            break;
#endif

        default:
            break;
        }
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
    }
#endif
#endif
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
    else
    {
#endif
#if ring_buffer_suppot_unsigned
        switch (handle->type)
        {
#if ring_buffer_suppot_8bit
        case sample_8bit:
            *(uint8_t *)dat = handle->sum / handle->size;
            break;
#endif

#if ring_buffer_suppot_16bit
        case sample_16bit:
            *(uint16_t *)dat = handle->sum / handle->size;
            break;
#endif

#if ring_buffer_suppot_32bit
        case sample_32bit:
            *(uint32_t *)dat = handle->sum / handle->size;
            break;
#endif

        default:
            break;
        }
#endif
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
    }
#endif
}

void ring_buffer_get_sum(ring_buffer_handle_t handle, void *dat)
{
#if ring_buffer_suppot_float
    if (handle->type == sample_float)
    {
        *(float *)dat = handle->sum_f;
        return;
    }
#endif

#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
    if (handle->signed_flg)
#endif
#if ring_buffer_suppot_signed
        *(int32_t *)dat = handle->sum_s;
#endif
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
    else
#endif
#if ring_buffer_suppot_unsigned
        *(uint32_t *)dat = handle->sum;
#endif
}

#endif

size_t ring_buffer_available(ring_buffer_handle_t handle)
{
    return handle->count;
}

void ring_buffer_read(ring_buffer_handle_t handle, void *dat)
{
    if (handle->count == 0)
        return;

#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
    if (handle->signed_flg)
    {
#endif

#if ring_buffer_suppot_signed || ring_buffer_suppot_float
        switch (handle->type)
        {

#if ring_buffer_suppot_8bit
        case sample_8bit:
            *(int8_t *)dat = handle->buffer_8_s[handle->read_p++];
            break;
#endif

#if ring_buffer_suppot_16bit
        case sample_16bit:
            *(int16_t *)dat = handle->buffer_16_s[handle->read_p++];
            break;
#endif

#if ring_buffer_suppot_32bit
        case sample_32bit:
            *(int32_t *)dat = handle->buffer_32_s[handle->read_p++];
            break;
#endif

#if ring_buffer_suppot_float
        case sample_float:
            *(float *)dat = handle->buffer_float[handle->read_p++];
            break;
#endif

        default:
            break;
        }
#endif

#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
    }
#endif

#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
    else
    {
#endif
#if ring_buffer_suppot_unsigned
        switch (handle->type)
        {
#if ring_buffer_suppot_8bit
        case sample_8bit:
            *(uint8_t *)dat = handle->buffer_8[handle->read_p++];
            break;
#endif

#if ring_buffer_suppot_16bit
        case sample_16bit:
            *(uint16_t *)dat = handle->buffer_16[handle->read_p++];
            break;
#endif

#if ring_buffer_suppot_32bit
        case sample_32bit:
            *(uint32_t *)dat = handle->buffer_32[handle->read_p++];
            break;
#endif

        default:
            break;
        }
#endif
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
    }
#endif

    if (handle->read_p == handle->size)
        handle->read_p = 0;
    handle->count--;
}

void ring_buffer_del(ring_buffer_handle_t handle)
{
    ring_buffer_free(handle->_buffer);
    ring_buffer_free(handle);
}

#endif
