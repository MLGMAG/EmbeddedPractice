#include "flash.h"
#include "stm32f4xx_hal.h"
#include <string.h>

uint8_t FLASH_Read(uint32_t *data, uint16_t size) {
	memcpy((void*) data, (void*) FLASH_USER_START_ADDR, size * FLASH_DATA_SIZE);
	return 1;
}

uint8_t FLASH_Write(uint32_t *data, uint16_t size, void (*error_handler)(void)) {
	HAL_StatusTypeDef status;
	HAL_FLASH_Unlock();

	FLASH_EraseInitTypeDef eraseConfig;
	eraseConfig.TypeErase = FLASH_TYPEERASE_SECTORS;
	eraseConfig.Sector = FLASH_SECTOR_7;
	eraseConfig.NbSectors = 1;
	eraseConfig.VoltageRange = FLASH_VOLTAGE_RANGE_3;

	uint32_t sectorError = 0;
	if (HAL_FLASHEx_Erase(&eraseConfig, &sectorError) != HAL_OK) {
		HAL_FLASH_Lock();
		error_handler();
		return 0;
	}
	uint32_t address = FLASH_USER_START_ADDR;

	for (uint16_t i = 0; i < size; i++) {
		for (int j = 0; j < 5; j++) {
			status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address,
					data[i]);
			if (status == HAL_OK) {
				address += FLASH_DATA_SIZE;
				break;
			}
			if (j == 4) {
				HAL_FLASH_Lock();
				error_handler();
			}
		}
	}
	HAL_FLASH_Lock();

	return 1;
}
