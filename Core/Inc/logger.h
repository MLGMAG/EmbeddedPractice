#ifndef __LOGGER_H
#define __LOGGER_H

#include "logger_conf.h"

#if defined(STM32F3)
#include "stm32f3xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#else
#error "Logger library was tested only on STM32F3, STM32F4!"
#endif

#if defined(LOGGER_USE_SWD)
#elif defined(LOGGER_USE_UART)
extern UART_HandleTypeDef LOGGER_UART_PORT;
#elif defined(LOGGER_SPI_PORT)
extern SPI_HandleTypeDef LOGGER_SPI_PORT;
#else
#error "You should define SSD1306_USE_SPI or SSD1306_USE_I2C macro!"
#endif

typedef enum {
    LOGGER_OK = 0x00,
    LOGGER_ERR = 0x01
} LOGGER_Error_t;

typedef enum {
	LOGGER_TRACE = 0,
	LOGGER_DEBUG = 1,
	LOGGER_INFO = 2,
	LOGGER_WARN = 3,
	LOGGER_ERROR = 4
} LOGGER_Level_t;

void LOGGER_trace(const char*);
void LOGGER_debug(const char*);
void LOGGER_info(const char*);
void LOGGER_warn(const char*);
void LOGGER_error(const char*);

#endif /* __LOGGER_H */
