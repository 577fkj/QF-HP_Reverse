#ifndef _eeprom_flash_H_
#define _eeprom_flash_H_

#include <EEPROM.h>
#include <Arduino.h>

#define eeprom_size 4096

#define eeprom_write_add 3000 // 起始地址

#define ee_head_0 0x98
#define ee_head_1 0x09

class eeprom_flash
{
public:
    void resume_factory();
    void read_all_data(); // 读取所有数据
    void write_task();
    uint8_t userdata_updata_user_key(uint8_t *sn);
    uint8_t write_flg = 0;
    uint8_t write_t = 21;

private:
    void data_init(); // 数据初始化
    void read_bytes(int add, void *buf, size_t size);
    void write_bytes(int add, void *buf, size_t size);
};

extern eeprom_flash eeprom;

#endif
