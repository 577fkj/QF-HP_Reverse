#include "eeprom_flash.h"
#include "User.h"
#include "PID.h"
#include "qf_key_chek.h"
extern void get_mac(uint8_t *mac_ret);
extern void get_sn(uint8_t *sn_ret);

eeprom_flash eeprom;

void eeprom_flash::data_init()
{
    // EEPROM初始化数据

    user_datas_init();

    write_flg = 2;

    EEPROM.write(eeprom_write_add, ee_head_0);
    EEPROM.write(eeprom_write_add + 1, ee_head_1);
}

void eeprom_flash::resume_factory()
{
    EEPROM.begin(eeprom_size);
    data_init();
    EEPROM.commit();
    EEPROM.end();
    write_flg = 2;
    write_task();
    ESP.restart();
}

uint8_t eeprom_flash::userdata_updata_user_key(uint8_t *sn)
{
    uint8_t mac[6];
    uint8_t key[6];
    get_mac(mac);
    qf_pass_key_get(mac, key, 6);

    if (memcmp(sn, key, sizeof(key)))
        return 0;

    memcpy(user_datas.key, sn, sizeof(user_datas.key));
    write_flg = 2;
    write_task();
    return 1;
}

void eeprom_flash::read_all_data()
{

    ec11.int_close();
    EEPROM.begin(eeprom_size);
    if (EEPROM.read(eeprom_write_add) != ee_head_0 || EEPROM.read(eeprom_write_add + 1) != ee_head_1)
    {
        data_init();
        write_bytes(eeprom_write_add + 2, &user_datas, sizeof(user_datas));
        EEPROM.commit();
    }
    else
    {
        read_bytes(eeprom_write_add + 2, &user_datas, sizeof(user_datas));
        Serial.print("Blinker ID:");
        Serial.println(user_datas.blinker_id);
    }

    EEPROM.end();

    ec11.int_work();
    oled.light(user_datas.ui_oled_light);
}

void eeprom_flash::read_bytes(int add, void *buf, size_t size)
{
    uint8_t *p = (uint8_t *)buf;
    while (size--)
        *p++ = EEPROM.read(add++);
}

void eeprom_flash::write_bytes(int add, void *buf, size_t size)
{
    uint8_t *p = (uint8_t *)buf;
    while (size--)
        EEPROM.write(add++, *p++);
}

void eeprom_flash::write_task()
{
    if (write_flg == 2)
    {
        write_flg = 0;
        ec11.int_close();
#ifdef DEBUG
        Serial.println("EE WRITE");
#endif
        EEPROM.begin(eeprom_size);
        write_bytes(eeprom_write_add + 2, &user_datas, sizeof(user_datas));
        EEPROM.commit();
        EEPROM.end();
#ifdef DEBUG
        Serial.println("END");
#endif
        ec11.int_work();
    }
}
