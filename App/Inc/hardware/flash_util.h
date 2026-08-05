#ifndef __FLASH_UTIL_H
#define __FLASH_UTIL_H

#include <stdint.h>

#define UAL_FLASH_UTIL_USER_START_ADDR    (0x08060000) // 7 sector address

#define UAL_FLASH_UTIL_DATA_SIZE 4   // one word


uint8_t UAL_FLASH_UTIL_Read(uint32_t *data, uint16_t size);
uint8_t UAL_FLASH_UTIL_Write(uint32_t *data, uint16_t size, void (*error_handler)(void));

#endif /* __FLASH_UTIL_H */
