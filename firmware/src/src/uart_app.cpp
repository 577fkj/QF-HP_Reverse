
#include "User.h"
#include "uart_cmd.h"
#include "eeprom_flash.h"

extern float kpv, kiv, kdv, kiv_high;
extern void get_mac(uint8_t *mac_ret);
extern void get_sn(uint8_t *sn_ret);

static void char_to_sn_hex(uint8_t *sn, uint8_t hex, uint8_t cnt)
{
    if ((cnt & 1) == 0)
        hex <<= 4;
    sn[cnt / 2] |= hex;
}

static uint16_t get_num_int(const char *str)
{
    uint16_t num = 0;
    while (*str >= '0' && *str <= '9')
    {
        num = num * 10 + (*str - '0');
        str++;
    }
    return num;
}

static float get_num_f(const char *str)
{
    float tmp = 0;
    uint16_t num = 0;
    uint16_t xiaoshu = 0;
    uint8_t xiaoshu_cnt = 0;
    uint32_t pow = 10;
    while (*str >= '0' && *str <= '9')
    {
        num = num * 10 + (*str - '0');
        str++;
    }
    str++;
    while (*str >= '0' && *str <= '9')
    {
        xiaoshu = xiaoshu * 10 + (*str - '0');
        str++;
        xiaoshu_cnt++;
    }

    if (xiaoshu_cnt > 0)
    {
        for (uint8_t i = 1; i < xiaoshu_cnt; i++)
        {
            pow *= 10;
        }
    }

    tmp = xiaoshu;
    tmp /= pow;
    tmp += num;

    return tmp;
}

void uart_app_handler()
{
    if (uart_cmd_available())
    {
        uint8_t buf[64];
        /// cmd
        if (uart_cmd_cmp("reset", 5))
        {
            ESP.restart();
        }
        else if (uart_cmd_cmp("T", 1))
        {
            char buffer[24];
            memset(buffer, 'a', sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = 0;
            uart_cmd_get_string(buffer);
            user_datas.pwm_temp_buf = get_num_int((const char *)&buffer[1]);
            Serial.printf("%d\n", user_datas.pwm_temp_buf);
        }
        else if (uart_cmd_cmp("P", 1))
        {
            char buffer[24];
            memset(buffer, 'a', sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = 0;
            uart_cmd_get_string(buffer);
            kpv = get_num_f((const char *)&buffer[1]);
            Serial.printf("SET KP:%f\n", kpv);
        }
        else if (uart_cmd_cmp("IH", 2))
        {
            char buffer[24];
            uart_cmd_get_string(buffer);
            kiv_high = get_num_f((const char *)&buffer[2]);
            Serial.printf("SET KIH:%f\n", kiv_high);
        }
        else if (uart_cmd_cmp("I", 1))
        {
            char buffer[24];
            uart_cmd_get_string(buffer);
            kiv = get_num_f((const char *)&buffer[1]);
            Serial.printf("SET KI:%f\n", kiv);
        }
        else if (uart_cmd_cmp("D", 1))
        {
            char buffer[24];
            uart_cmd_get_string(buffer);
            kdv = get_num_f((const char *)&buffer[1]);
            Serial.printf("SET KD:%f\n", kdv);
        }
        else if (uart_cmd_cmp("get_pid", 7))
        {
            Serial.printf("KP:%f\n", kpv);
            Serial.printf("KI:%f\n", kiv);
            Serial.printf("KI HIGH:%f\n", kiv_high);
            Serial.printf("KD:%f\n", kdv);
        }
        else if (uart_cmd_cmp("get_id", 6))
        {
            uint8_t mac[6];
            get_mac(mac);
            Serial.printf("get_id_ok %02X-%02X-%02X-%02X-%02X-%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        }
        else if (uart_cmd_cmp("get_sn", 6))
        {
            uint8_t sn[3];
            get_sn(sn);
            Serial.printf("get_sn_ok %02X-%02X-%02X-%02X-%02X-%02X\r\n", sn[0], sn[1], sn[2], sn[3], sn[4], sn[5]);
        }
        else
        {
            char buffer[36];
            memset(buffer, 0, sizeof(buffer));
            uart_cmd_get_string(buffer);

            if (memcmp(buffer, "set_sn", 6) == 0)
            {
                uint8_t sn[6] = {0, 0, 0, 0, 0, 0};

                uint8_t cnt = 0;

                for (size_t i = 6; i < 36; i++)
                {
                    if (buffer[i] >= '0' && buffer[i] <= '9')
                    {
                        char_to_sn_hex(sn, buffer[i] - 0x30, cnt++);
                    }
                    else if (buffer[i] >= 'A' && buffer[i] <= 'F')
                    {
                        char_to_sn_hex(sn, buffer[i] - 55, cnt++);
                    }
                    else if (buffer[i] >= 'a' && buffer[i] <= 'f')
                    {
                        char_to_sn_hex(sn, buffer[i] - 87, cnt++);
                    }
                    else
                    {
                        if (cnt & 1)
                        {
                            sn[cnt / 2] >>= 4;
                            cnt++;
                        }
                    }
                    if (cnt == 12)
                        break;
                    if (buffer[i] == 0)
                        break;
                }
                if (cnt != 12)
                {
                    Serial.print("set_sn_err no_sn\r\n");
                    delay(3000);
                }
                else
                {
                    if (eeprom.userdata_updata_user_key(sn))
                    {
                        Serial.print("set_sn_ok\r\n");
                        Serial.print("set_sn_ok\r\n");
                        delay(1000);
                        ESP.restart();
                    }
                    else
                    {
                        Serial.print("set_sn_err sn_err\r\n");
                        delay(3000);
                    }
                }
                uart_cmd_clear();
                return;
            }

            Serial.printf("input cmd error, cmd:%s\n", buffer);
        }

        uart_cmd_clear();
    }
}
