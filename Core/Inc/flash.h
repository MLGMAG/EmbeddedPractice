#ifndef __FLASH_H
#define __FLASH_H

#include <stdint.h>

#define FLASH_USER_START_ADDR    (0x08060000) // 7 sector address
#define FLASH_DATA_SIZE 4 // one word

uint8_t FLASH_Read(uint32_t *data, uint16_t size);

uint8_t FLASH_Write(uint32_t *data, uint16_t size, void (*error_handler) (void));

#endif /* __FLASH_H */
