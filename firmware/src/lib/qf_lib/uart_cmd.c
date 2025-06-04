#include "uart_cmd.h"

static char _cmd[UART_CMD_MAX_LEN + 1] = {0};
static char buffer[UART_CMD_MAX_LEN + 1] = {0};
static uint8_t cnt = 0;
static uint8_t sta = 0;
static const char cmd_list[2] = {'\r', '\n'};
static uint8_t ready = 0;

void uart_cmd_get_byte(char byte);
uint8_t uart_cmd_available();
uint8_t uart_cmd_cmp(const char *cmd, size_t len);
uint8_t uart_cmd_get_string(char *str);
void uart_cmd_clear(void);

void uart_cmd_get_byte(char byte)
{
    if (byte == cmd_list[sta])
    {
        sta++;
        if (sta == 2 && ready == 0)
        {
            buffer[cnt] = 0;
            _cmd[cnt] = 0;
            strcpy(_cmd, buffer);
            ready = 1;
            sta = 0;
            cnt = 0;
        }
    }
    else
    {
        sta = 0;
        buffer[cnt++] = byte;
        if (cnt == UART_CMD_MAX_LEN)
            cnt = 0;
    }
}

uint8_t uart_cmd_available()
{
    return ready;
}

uint8_t uart_cmd_cmp(const char *cmd, size_t len)
{
    if (memcmp(cmd, buffer, len) == 0)
        return 1;
    else
        return 0;
}

uint8_t uart_cmd_get_string(char *str)
{
    if (ready)
    {
        if (str != NULL)
            strcpy(str, _cmd);
        return strlen(_cmd);
    }
    else
        return 0;
}

void uart_cmd_clear(void)
{
    ready = 0;
}
