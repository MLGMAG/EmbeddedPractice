#ifndef __LOGGER_CONF_H
#define __LOGGER_CONF_H

// Choose a microcontroller family
//#define STM32F3
#define STM32F4

// Choose log level
#define LOGGER_Level  LOGGER_TRACE

// Choose log output
//#define LOGGER_USE_SWD
#define LOGGER_USE_UART
//#define LOGGER_USE_SPI_SD_CARD

// UART Configuration
#define LOGGER_UART_BUFFER_LEN 256
#define LOGGER_UART_PORT  huart1

// SPI Configuration
//#define LOGGER_SPI_PORT  hspi1


#endif /* __LOGGER_CONF_H */
