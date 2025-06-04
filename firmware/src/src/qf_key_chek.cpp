#include "qf_key_chek.h"
#include "User.h"
#include "user_datas.h"

void get_mac(uint8_t *mac_ret)
{
    uint64_t chipid = ESP.getChipId();
    uint8_t *mac = (uint8_t *)(&chipid);
    mac[3] = mac[0] + mac[1];
    mac[4] = mac[0] + mac[2];
    mac[5] = mac[1] + mac[2];
    memcpy(mac_ret, mac, 6);
}

void get_sn(uint8_t *sn_ret)
{
    memcpy(sn_ret, user_datas.key, 6);
}

// 函数qf_pass_key_get用于获取密钥
void qf_pass_key_get(uint8_t *mac, uint8_t *key, size_t bytes)
{

}