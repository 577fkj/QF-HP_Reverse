#ifndef ring_buffer_H
#define ring_buffer_H

#ifdef __cplusplus
extern "C"
{
#endif

        /****************************************************************/
        /*
            数据环形缓冲库 By 启凡科创。
            Version:1.0.3 2023-9-2

            支持创建：
                8位、16位、32位的整形有符号、无符号环形缓冲区
                float类型环形环形缓冲
                以上任意类型缓冲提供可开启的滑动滤波功能，开启后还可提供整个缓冲区数据求和功能

            1.0.3:
                    修复float使能关闭后编译报错的问题
                    修复无符号16 32bit数据无法写入的问题

        */
        /****************************************************************/

        // #include <LIB_CONFIG.h>
        // #include "freertos/FreeRTOS.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ring_buffer_compile_en 1 // 库编译时能

#if ring_buffer_compile_en

// 一下任意功能不需要时关闭都可以提升性能
#define ring_buffer_moving_flitering_en 1 // 滑动滤波功能使能
#define ring_buffer_suppot_unsigned 1     // 支持无符号型数据使能
#define ring_buffer_suppot_signed 0       // 支持有符号型数据使能
#define ring_buffer_suppot_8bit 0         // 支持8bit数据使能
#define ring_buffer_suppot_16bit 1        // 支持16bit数据使能
#define ring_buffer_suppot_32bit 0        // 支持32bit数据使能
#define ring_buffer_suppot_float 0        // 支持浮点型数据使能

#define ring_buffer_malloc(x) malloc(x)
#define ring_buffer_free(x) free(x)

        typedef enum
        {
#if ring_buffer_suppot_8bit
                sample_8bit = 1,
#endif
#if ring_buffer_suppot_16bit
                sample_16bit = 2,
#endif
#if ring_buffer_suppot_32bit
                sample_32bit = 4,
#endif
#if ring_buffer_suppot_float
                sample_float = 5,
#endif
        } ring_buffer_sample_t;

        typedef enum
        {
                type_unsigned,
                type_signed
        } ring_buffer_signed_t;

        typedef struct
        {
#if ring_buffer_suppot_8bit
#if ring_buffer_suppot_unsigned
                uint8_t *buffer_8;
#endif
#if ring_buffer_suppot_signed
                int8_t *buffer_8_s;
#endif
#endif
#if ring_buffer_suppot_16bit
#if ring_buffer_suppot_unsigned
                uint16_t *buffer_16;
#endif
#if ring_buffer_suppot_signed
                int16_t *buffer_16_s;
#endif
#endif
#if ring_buffer_suppot_32bit
#if ring_buffer_suppot_unsigned
                uint32_t *buffer_32;
#endif
#if ring_buffer_suppot_signed
                int32_t *buffer_32_s;
#endif
#endif
#if ring_buffer_suppot_float
                float *buffer_float;
#endif
                size_t size;
                size_t add_p;
                size_t read_p;
                ring_buffer_sample_t type;
                size_t count;
                void *_buffer;
#if ring_buffer_suppot_signed && ring_buffer_suppot_unsigned
                ring_buffer_signed_t signed_flg;
#endif
#if ring_buffer_moving_flitering_en
#if ring_buffer_suppot_8bit || ring_buffer_suppot_16bit || ring_buffer_suppot_32bit
#if ring_buffer_suppot_signed
                int32_t sum_s;
#endif
#if ring_buffer_suppot_unsigned
                uint32_t sum;
#endif
#endif
#if ring_buffer_suppot_float
                float sum_f;
#endif
                uint8_t moveing_flitering_flg;
#endif
        } ring_buffer_t;

        typedef ring_buffer_t *ring_buffer_handle_t;

        /**
         * @brief 创建一个环形缓冲区
         *
         * @param sample 样本宽度，sample_8bit，sample_16bit，sample_32bit,sample_float
         * @param signed_flg 符号标志，1：有符号类型，0：无符号类型,仅对于整形有作用
         * @param size 样本数量
         * @return ring_buffer_handle_t 句柄，用于后续操作样本，NULL：创建失败
         */
        ring_buffer_handle_t ring_buffer_create(ring_buffer_sample_t sample, ring_buffer_signed_t signed_flg, size_t size);

        /**
         * @brief 初始化缓冲区数据
         *
         * @param handle 缓冲区句柄
         * @param var 初始值，类型必须跟创建环形缓冲区的样本类型相同
         */
        void ring_buffer_init_value(ring_buffer_handle_t handle, void *var);

        /**
         * @brief 删除环形缓冲区
         *
         * @param handle 缓冲区句柄
         */
        void ring_buffer_del(ring_buffer_handle_t handle);

        /**
         * @brief 向缓冲区写一个样本
         *
         * @param handle 缓冲区句柄
         * @param dat 指向待写入的数据，类型必须跟创建环形缓冲区的样本类型相同
         */
        void ring_buffer_write(ring_buffer_handle_t handle, void *dat);

        /**
         * @brief 环形缓冲区是否可用
         *
         * @param handle 缓冲区句柄
         * @return size_t 可用样本数
         */
        size_t ring_buffer_available(ring_buffer_handle_t handle);

        /**
         * @brief 从缓冲区读取一个样本
         *
         * @param handle 缓冲区句柄
         * @param dat 用于接收数据的变量，类型必须跟创建环形缓冲区的样本类型相同
         */
        void ring_buffer_read(ring_buffer_handle_t handle, void *dat);

#if ring_buffer_moving_flitering_en

        /**
         * @brief 使能滑动滤波,默认关闭
         *
         * @param enable 1：启用，0：禁用
         */
        void ring_buffer_set_moveing_flitering_en(ring_buffer_handle_t handle, uint8_t enable);

        /**
         * @brief 获取滑动滤波结果值
         *
         * @param dat 接收结果的变量，类型必须跟创建环形缓冲区的样本类型相同
         */
        void ring_buffer_get_moveing_flitering(ring_buffer_handle_t handle, void *dat);

        /**
         * @brief 获取整个缓冲区求和,需在开启滑动滤波使能时可用
         *
         * @param handle 缓冲区句柄
         * @param dat 有符号类型缓冲结果为int32_t，无符号为uint32_t，浮点型为float
         */
        void ring_buffer_get_sum(ring_buffer_handle_t handle, void *dat);

#endif

#endif
#ifdef __cplusplus
}
#endif
#endif
