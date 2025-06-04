#ifndef qf_key_chek_h
#define qf_key_chek_h


#include "stdint.h"
#include "string.h"
#include "stdlib.h"

#define qf_private_key 0x98, 0x08   // 加密私匙

/**
 * @brief 提供mac计算密匙
 * 
 * @param mac 6字节mac
 * @param key 结果
 * @param bytes 结果返回3字节或6字节
 */
void qf_pass_key_get(uint8_t *mac, uint8_t *key, size_t bytes);



#endif
