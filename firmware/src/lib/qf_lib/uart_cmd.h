#ifndef UART_CMD_H
#define UART_CMD_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define UART_CMD_MAX_LEN 36 // 单个指令最大长度

    /**
     * @brief 从串口接收到的字节推到库里
     *
     * @param byte 从串口接收到的字节
     */
    void uart_cmd_get_byte(char byte);

    /**
     * @brief 是否接收到可用指令，0：无指令，1：有指令
     *
     * @return uint8_t
     */
    uint8_t uart_cmd_available();

    /**
     * @brief 判断指令
     * 
     * @param cmd 指令
     * @param len 指令长度
     * @return uint8_t 1：是，0：不是
     */
    uint8_t uart_cmd_cmp(const char *cmd,size_t len);

    /**
     * @brief 获取指令字符串
     *
     * @param str 指向用于保存的缓存，NULL则仅返回指令长度
     * @return uint8_t 字符串长度
     */
    uint8_t uart_cmd_get_string(char *str);

    /**
     * @brief 清除指令
     */
    void uart_cmd_clear(void);

#ifdef __cplusplus
}
#endif
#endif
