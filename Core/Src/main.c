/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "bme280.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ds1307.h"
#include "flash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define UART_TX_MSG_LEN_MAX 128

typedef struct {
	char msg[UART_TX_MSG_LEN_MAX];
} UART_QUEUE_MSG_t;

typedef struct {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} DATE_t;

typedef struct {
	uint16_t humidity;
	uint16_t pressure;
	float temp;
} WEATHER_DATA_t;

typedef struct {
	WEATHER_DATA_t weather_data;
	DATE_t timestamp;
} SENSORS_DATA_t;

typedef struct {
	SENSORS_DATA_t sensors_data;
} DISPLAY_MSG_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define QUEUE_MSG_MAX 4

#define YEAR 2026
#define MONTH 6
#define DATE 28
#define DAY_OF_WEEK 6
#define HOUR 16
#define MIN 5
#define SEC 22
#define TIME_ZONE 2

#define TIMESTAMP_FORMAT "%d-%02d-%02d %02d:%02d:%02d"
#define UART_DATA_FORMAT "{\"timestamp\":\"%d-%02d-%02d %02d:%02d:%02d\",\"data\":{\"temperature\":%.2f,\"humidity\":%d,\"pressure\":%d}}\n"

#define UART_RX_MSG_LEN_MAX 32

#define SCREEN_MSG_LEN_MAX 64

#define FLASH_LOGS_LEN_MAX 32
#define FLASH_WRITE_PER_HOUR 60

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* Definitions for SENSORS_TASK */
osThreadId_t SENSORS_TASKHandle;
const osThreadAttr_t SENSORS_TASK_attributes = { .name = "SENSORS_TASK",
		.stack_size = 512 * 4, .priority = (osPriority_t) osPriorityLow, };
/* Definitions for UART_TASK */
osThreadId_t UART_TASKHandle;
const osThreadAttr_t UART_TASK_attributes = { .name = "UART_TASK", .stack_size =
		256 * 4, .priority = (osPriority_t) osPriorityLow, };
/* Definitions for DISPLAY_TASK */
osThreadId_t DISPLAY_TASKHandle;
const osThreadAttr_t DISPLAY_TASK_attributes = { .name = "DISPLAY_TASK",
		.stack_size = 256 * 4, .priority = (osPriority_t) osPriorityHigh, };
/* Definitions for MANAGER_TASK */
osThreadId_t MANAGER_TASKHandle;
const osThreadAttr_t MANAGER_TASK_attributes = { .name = "MANAGER_TASK",
		.stack_size = 512 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for FLASH_TASK */
osThreadId_t FLASH_TASKHandle;
const osThreadAttr_t FLASH_TASK_attributes = { .name = "FLASH_TASK",
		.stack_size = 256 * 4, .priority = (osPriority_t) osPriorityLow, };
/* USER CODE BEGIN PV */

static osMessageQueueId_t uart_queue;
static osMessageQueueId_t display_queue;

static const osMutexAttr_t sensor_read_mutext_attributes = { "MUTEX1", 0U, NULL,
		0U };
static osMutexId_t sensor_read_mutext;

static uint8_t uart_tx_message_buffer[UART_TX_MSG_LEN_MAX];
static uint8_t uart_rx_message_buffer[UART_RX_MSG_LEN_MAX];
static uint8_t uart_dma_buffer[UART_RX_MSG_LEN_MAX];
static volatile uint16_t dma_buffer_len = 0;
static volatile uint16_t last_rx_offset = 0;
static volatile uint16_t uart_rx_available = 0;

static SENSORS_DATA_t sensor_data_logs[FLASH_LOGS_LEN_MAX];
static uint16_t sensor_data_logs_index = 0;
static uint8_t is_logs_on = 1;

static uint8_t screen_message_buffer[SCREEN_MSG_LEN_MAX];

static BME280_Data_t BME280;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_I2C1_Init(void);
void SENSORS_TASK_Start(void *argument);
void UART_TASK_Start(void *argument);
void DISPLAY_TASK_Start(void *argument);
void MANAGER_TASK_Start(void *argument);
void FLASH_TASK_Start(void *argument);

/* USER CODE BEGIN PFP */
static void QueueTimeoutError_Handler(void);
static void BME280_Init(void);
static void DS1307_Config(void);
static void MessageQueue_CheckStatus(const osStatus_t*);
static void SSD1306_UpdateScreen(SENSORS_DATA_t*);
static void SENSORS_FillData(SENSORS_DATA_t*);
static void SENSORS_FillDataSync(SENSORS_DATA_t*);
static void SENSORS_LogData(SENSORS_DATA_t*);
static void UART_DMA_ProcessChar(char);
static void UART_HandleCommand(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void QueueTimeoutError_Handler(void) {

}

static void BME280_Init(void) {
	BME280_Init_t BME280_InitStruct = { 0 };

	Reset_BME280();

	BME280_InitStruct.Filter = FILTER_8;
	BME280_InitStruct.Mode = BME280_NORMAL_MODE;
	BME280_InitStruct.OverSampling_H = OVERSAMPLING_16;
	BME280_InitStruct.OverSampling_P = OVERSAMPLING_16;
	BME280_InitStruct.OverSampling_T = OVERSAMPLING_16;
	BME280_InitStruct.SPI_EnOrDıs = SPI3_W_DISABLE;
	BME280_InitStruct.T_StandBy = T_SB_250;

	BME280Init(BME280_InitStruct);
}

static void DS1307_Config(void) {
	DS1307_Init(&hi2c1);

	DS1307_SetTimeZone(TIME_ZONE, 00);
	DS1307_SetDate(DATE);
	DS1307_SetMonth(MONTH);
	DS1307_SetYear(YEAR);
	DS1307_SetDayOfWeek(DAY_OF_WEEK);
	DS1307_SetHour(HOUR);
	DS1307_SetMinute(MIN);
	DS1307_SetSecond(SEC);
}

static void MessageQueue_CheckStatus(const osStatus_t *status) {
	if (*status == osErrorTimeout) {
		QueueTimeoutError_Handler();
	}

	if (*status != osOK && *status != osErrorTimeout) {
		Error_Handler();
	}
}

static void SSD1306_UpdateScreen(SENSORS_DATA_t *data) {
	ssd1306_Fill(Black);

	sprintf((char*) screen_message_buffer, "Date: %d-%02d-%02d",
			data->timestamp.year, data->timestamp.month, data->timestamp.day);
	ssd1306_SetCursor(5, 5);
	ssd1306_WriteString((char*) screen_message_buffer, Font_6x8, White);

	sprintf((char*) screen_message_buffer, "Time: %02d:%02d:%02d",
			data->timestamp.hour, data->timestamp.minute,
			data->timestamp.second);
	ssd1306_SetCursor(5, 20);
	ssd1306_WriteString((char*) screen_message_buffer, Font_6x8, White);

	sprintf((char*) screen_message_buffer, "T: %.1fC RH: %d%%",
			data->weather_data.temp, data->weather_data.humidity);
	ssd1306_SetCursor(5, 35);
	ssd1306_WriteString((char*) screen_message_buffer, Font_6x8, White);

	sprintf((char*) screen_message_buffer, "P: %d hPa",
			data->weather_data.pressure);
	ssd1306_SetCursor(5, 50);
	ssd1306_WriteString((char*) screen_message_buffer, Font_6x8, White);

	ssd1306_UpdateScreen();
}

static void SENSORS_FillDataSync(SENSORS_DATA_t *data) {
	osStatus_t status;
	status = osMutexAcquire(sensor_read_mutext, osWaitForever);
	if (status != osOK) {
		Error_Handler();
	}

	SENSORS_FillData(data);

	status = osMutexRelease(sensor_read_mutext);
	if (status != osOK) {
		Error_Handler();
	}

}

static void SENSORS_FillData(SENSORS_DATA_t *data) {
	BME280Calculation(&BME280);
	data->weather_data.humidity = BME280.Humidity;
	data->weather_data.pressure = BME280.Pressure;
	data->weather_data.temp = BME280.Temperature;

	data->timestamp.year = DS1307_GetYear();
	data->timestamp.month = DS1307_GetMonth();
	data->timestamp.day = DS1307_GetDate();
	data->timestamp.hour = DS1307_GetHour();
	data->timestamp.minute = DS1307_GetMinute();
	data->timestamp.second = DS1307_GetSecond();
}

static void SENSORS_LogData(SENSORS_DATA_t *data) {
	SENSORS_DATA_t *sensors_data = &sensor_data_logs[sensor_data_logs_index];
	memcpy(sensors_data, data, sizeof(SENSORS_DATA_t));
	sensor_data_logs_index++;
	if (sensor_data_logs_index == FLASH_LOGS_LEN_MAX) {
		sensor_data_logs_index = 0;
	}
}

static void UART_DMA_ProcessChar(char c) {
	if (c == '\n') {
		uart_rx_available = 1;
		HAL_UART_DMAStop(&huart1);
	} else {
		if (dma_buffer_len < UART_RX_MSG_LEN_MAX) {
			strncat((char*) &uart_rx_message_buffer, &c, 1);
			dma_buffer_len++;
		} else {
			uart_rx_message_buffer[0] = '\0';
			dma_buffer_len = 0;
		}
	}
}

static void UART_HandleCommand(void) {
	if (strcmp("status", (char*) uart_rx_message_buffer) == 0) {
		SENSORS_DATA_t sensors_data;
		SENSORS_FillDataSync(&sensors_data);

		UART_QUEUE_MSG_t uart_queue_msg;
		sprintf((char*) uart_queue_msg.msg, UART_DATA_FORMAT,
				sensors_data.timestamp.year, sensors_data.timestamp.month,
				sensors_data.timestamp.day, sensors_data.timestamp.hour,
				sensors_data.timestamp.minute, sensors_data.timestamp.second,
				sensors_data.weather_data.temp,
				sensors_data.weather_data.humidity,
				sensors_data.weather_data.pressure);
		osStatus_t status = osMessageQueuePut(uart_queue, &uart_queue_msg, 0U,
				pdMS_TO_TICKS(1000));
		MessageQueue_CheckStatus(&status);
	}

	if (strcmp("log on", (char*) uart_rx_message_buffer) == 0) {
		is_logs_on = 1;
		UART_QUEUE_MSG_t uart_queue_msg;
		sprintf((char*) uart_queue_msg.msg, "Logging is enabled.\n");
		osStatus_t status = osMessageQueuePut(uart_queue, &uart_queue_msg, 0U,
				pdMS_TO_TICKS(1000));
		MessageQueue_CheckStatus(&status);
	}

	if (strcmp("log off", (char*) uart_rx_message_buffer) == 0) {
		is_logs_on = 0;
		UART_QUEUE_MSG_t uart_queue_msg;
		sprintf((char*) uart_queue_msg.msg, "Logging is disabled.\n");
		osStatus_t status = osMessageQueuePut(uart_queue, &uart_queue_msg, 0U,
				pdMS_TO_TICKS(1000));
		MessageQueue_CheckStatus(&status);
	}

	if (strcmp("flush", (char*) uart_rx_message_buffer) == 0) {
		FLASH_Write((uint32_t*) sensor_data_logs,
				FLASH_LOGS_LEN_MAX / FLASH_DATA_SIZE, Error_Handler);
		UART_QUEUE_MSG_t uart_queue_msg;
		sprintf((char*) uart_queue_msg.msg, "Flush is done.\n");
		osStatus_t status = osMessageQueuePut(uart_queue, &uart_queue_msg, 0U,
				pdMS_TO_TICKS(1000));
		MessageQueue_CheckStatus(&status);
	}

	uart_rx_message_buffer[0] = '\0';
	dma_buffer_len = 0;
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart_dma_buffer,
	UART_RX_MSG_LEN_MAX);
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART1_UART_Init();
	MX_RTC_Init();
	MX_I2C1_Init();
	/* USER CODE BEGIN 2 */

	ssd1306_Init();
	DS1307_Config();
	BME280_Init();

	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart_dma_buffer,
	UART_RX_MSG_LEN_MAX);

	/* USER CODE END 2 */

	/* Init scheduler */
	osKernelInitialize();

	/* USER CODE BEGIN RTOS_MUTEX */
	sensor_read_mutext = osMutexNew(&sensor_read_mutext_attributes);
	if (sensor_read_mutext == NULL) {
		Error_Handler();
	}
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* USER CODE BEGIN RTOS_QUEUES */
	uart_queue = osMessageQueueNew(QUEUE_MSG_MAX, sizeof(UART_QUEUE_MSG_t),
	NULL);
	display_queue = osMessageQueueNew(QUEUE_MSG_MAX, sizeof(DISPLAY_MSG_t),
	NULL);
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* creation of SENSORS_TASK */
	SENSORS_TASKHandle = osThreadNew(SENSORS_TASK_Start, NULL,
			&SENSORS_TASK_attributes);

	/* creation of UART_TASK */
	UART_TASKHandle = osThreadNew(UART_TASK_Start, NULL, &UART_TASK_attributes);

	/* creation of DISPLAY_TASK */
	DISPLAY_TASKHandle = osThreadNew(DISPLAY_TASK_Start, NULL,
			&DISPLAY_TASK_attributes);

	/* creation of MANAGER_TASK */
	MANAGER_TASKHandle = osThreadNew(MANAGER_TASK_Start, NULL,
			&MANAGER_TASK_attributes);

	/* creation of FLASH_TASK */
	FLASH_TASKHandle = osThreadNew(FLASH_TASK_Start, NULL,
			&FLASH_TASK_attributes);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
	/* USER CODE END RTOS_EVENTS */

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE
			| RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 12;
	RCC_OscInitStruct.PLL.PLLN = 96;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief RTC Initialization Function
 * @param None
 * @retval None
 */
static void MX_RTC_Init(void) {

	/* USER CODE BEGIN RTC_Init 0 */

	/* USER CODE END RTC_Init 0 */

	RTC_TimeTypeDef sTime = { 0 };
	RTC_DateTypeDef sDate = { 0 };

	/* USER CODE BEGIN RTC_Init 1 */

	/* USER CODE END RTC_Init 1 */

	/** Initialize RTC Only
	 */
	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = 127;
	hrtc.Init.SynchPrediv = 255;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_RTC_Init(&hrtc) != HAL_OK) {
		Error_Handler();
	}

	/* USER CODE BEGIN Check_RTC_BKUP */

	/* USER CODE END Check_RTC_BKUP */

	/** Initialize RTC and set the Time and Date
	 */
	sTime.Hours = 0x0;
	sTime.Minutes = 0x0;
	sTime.Seconds = 0x0;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK) {
		Error_Handler();
	}
	sDate.WeekDay = RTC_WEEKDAY_MONDAY;
	sDate.Month = RTC_MONTH_JANUARY;
	sDate.Date = 0x1;
	sDate.Year = 0x0;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK) {
		Error_Handler();
	}

	/** Enable the WakeUp
	 */
	if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 10, RTC_WAKEUPCLOCK_CK_SPRE_16BITS)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN RTC_Init 2 */

	/* USER CODE END RTC_Init 2 */

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA2_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA2_Stream2_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t offset) {
	if (offset != last_rx_offset) {
		if (offset < last_rx_offset) {
			last_rx_offset = 0;
		}

		while (last_rx_offset < offset) {
			UART_DMA_ProcessChar((char) uart_dma_buffer[last_rx_offset]);
			last_rx_offset++;
		}
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	Error_Handler();
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_SENSORS_TASK_Start */
/**
 * @brief  Function implementing the SENSORS_TASK thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_SENSORS_TASK_Start */
void SENSORS_TASK_Start(void *argument) {
	/* USER CODE BEGIN 5 */
	osStatus_t status;
	uint32_t last_tick = osKernelGetTickCount();
	uint32_t putMessage_delay = pdMS_TO_TICKS(1000);

	/* Infinite loop */
	while (1) {
		last_tick += pdMS_TO_TICKS(1000);
		SENSORS_DATA_t sensors_data;
		SENSORS_FillData(&sensors_data);

		if (is_logs_on == 1) {
			SENSORS_LogData(&sensors_data);
		}

		status = osMessageQueuePut(display_queue, &sensors_data, 0U,
				putMessage_delay);
		MessageQueue_CheckStatus(&status);
		osDelayUntil(last_tick);
	}
	/* USER CODE END 5 */
}

/* USER CODE BEGIN Header_UART_TASK_Start */
/**
 * @brief Function implementing the UART_TASK thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_UART_TASK_Start */
void UART_TASK_Start(void *argument) {
	/* USER CODE BEGIN UART_TASK_Start */
	UART_QUEUE_MSG_t queue_msg;
	osStatus_t status;
	uint32_t getMessage_delay = pdMS_TO_TICKS(10000);

	/* Infinite loop */
	while (1) {
		status = osMessageQueueGet(uart_queue, &queue_msg, NULL,
				getMessage_delay);

		if (status == osOK) {
			sprintf((char*) uart_tx_message_buffer, "%s", queue_msg.msg);
			uint16_t len = strlen((char*) uart_tx_message_buffer) + 1;
			HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1,
					uart_tx_message_buffer, len, 100);
			if (status != HAL_OK) {
				Error_Handler();
			}
		} else if (status != osErrorTimeout) {
			Error_Handler();
		}

		osDelay(1);
	}
	/* USER CODE END UART_TASK_Start */
}

/* USER CODE BEGIN Header_DISPLAY_TASK_Start */
/**
 * @brief Function implementing the DISPLAY_TASK thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_DISPLAY_TASK_Start */
void DISPLAY_TASK_Start(void *argument) {
	/* USER CODE BEGIN DISPLAY_TASK_Start */
	SENSORS_DATA_t sensors_data;
	osStatus_t status;
	uint32_t getMessage_delay = pdMS_TO_TICKS(10000);

	/* Infinite loop */
	while (1) {
		status = osMessageQueueGet(display_queue, &sensors_data, NULL,
				getMessage_delay);

		if (status == osOK) {
			SSD1306_UpdateScreen(&sensors_data);
		} else if (status != osErrorTimeout) {
			Error_Handler();
		}
		osDelay(1);
	}
	/* USER CODE END DISPLAY_TASK_Start */
}

/* USER CODE BEGIN Header_MANAGER_TASK_Start */
/**
 * @brief Function implementing the MANAGER_TASK thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_MANAGER_TASK_Start */
void MANAGER_TASK_Start(void *argument) {
	/* USER CODE BEGIN MANAGER_TASK_Start */
	/* Infinite loop */
	while (1) {
		if (uart_rx_available == 1) {
			UART_HandleCommand();
			uart_rx_available = 0;
		}
		osDelay(pdMS_TO_TICKS(100));
	}
	/* USER CODE END MANAGER_TASK_Start */
}

/* USER CODE BEGIN Header_FLASH_TASK_Start */
/**
 * @brief Function implementing the FLASH_TASK thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_FLASH_TASK_Start */
void FLASH_TASK_Start(void *argument) {
	/* USER CODE BEGIN FLASH_TASK_Start */
	uint32_t task_delay = pdMS_TO_TICKS(3600000 / FLASH_WRITE_PER_HOUR);
	/* Infinite loop */
	while (1) {
		if (is_logs_on == 1) {
			FLASH_Write((uint32_t*) sensor_data_logs,
			FLASH_LOGS_LEN_MAX / FLASH_DATA_SIZE, Error_Handler);
		}
		osDelay(task_delay);
	}
	/* USER CODE END FLASH_TASK_Start */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
