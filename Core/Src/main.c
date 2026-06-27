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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "traffic_light.h"
#include "scheduler.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SCHEDULER_TASKS_COUNT 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

static uint8_t data[100];
static uint16_t len;

static QUEUE_DATA_t queue_data = { };

static UAL_SCHEDULER_TASK_t task1 = { };
static UAL_SCHEDULER_TASK_t scheduler_tasks[SCHEDULER_TASKS_COUNT];
static UAL_SCHEDULER_t scheduler = { };

static UAL_TRAFFIC_LIGHT_t traffic_light = { };

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
static void SCHEDULER_TASKS_Init(void);
static void SCHEDULER_Init(void);
static uint32_t SCHEDULER_Get_current_time_ms(void);
static void SCHEDULER_DEFAULT_TASK_Start(void);
static void SCHEDULER_TASK1_Start(void);
static void TRAFFIC_LIGHT_Init(void);
static void TRAFFIC_LIGHT_Turn_off_pin(UAL_TRAFFIC_LIGHT_OUTPUT_PIN_t*);
static void TRAFFIC_LIGHT_Turn_on_pin(UAL_TRAFFIC_LIGHT_OUTPUT_PIN_t*);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void SCHEDULER_TASKS_Init(void) {
	task1.task_func = SCHEDULER_TASK1_Start;
	task1.delay_ms = 1000;

	scheduler_tasks[0] = task1;
}

static void SCHEDULER_Init(void) {
	scheduler.get_current_time_ms = SCHEDULER_Get_current_time_ms;
	scheduler.tasks_queue = &queue_data;
	scheduler.tasks_count = SCHEDULER_TASKS_COUNT;
	scheduler.tasks = (UAL_SCHEDULER_TASK_t*) scheduler_tasks;
	scheduler.default_task = SCHEDULER_DEFAULT_TASK_Start;

	UAL_SCHEDULER_Init(&scheduler, Error_Handler);
}

static uint32_t SCHEDULER_Get_current_time_ms(void) {
	return HAL_GetTick();
}

static void TRAFFIC_LIGHT_Init(void) {
	traffic_light.state = GREEN;

	traffic_light.green_pin.pin_number = GPIO_PIN_14;
	traffic_light.green_pin.port = (void*) GPIOB;

	traffic_light.yellow_pin.pin_number = GPIO_PIN_13;
	traffic_light.yellow_pin.port = (void*) GPIOB;

	traffic_light.red_pin.pin_number = GPIO_PIN_12;
	traffic_light.red_pin.port = (void*) GPIOB;

	traffic_light.turn_off_pin = TRAFFIC_LIGHT_Turn_off_pin;
	traffic_light.turn_on_pin = TRAFFIC_LIGHT_Turn_on_pin;

	UAL_TRAFFIC_LIGHT_init(&traffic_light);
}

static void TRAFFIC_LIGHT_Turn_off_pin(UAL_TRAFFIC_LIGHT_OUTPUT_PIN_t *data) {
	HAL_GPIO_WritePin((GPIO_TypeDef*) data->port, data->pin_number,
			GPIO_PIN_RESET);
}

static void TRAFFIC_LIGHT_Turn_on_pin(UAL_TRAFFIC_LIGHT_OUTPUT_PIN_t *data) {
	HAL_GPIO_WritePin((GPIO_TypeDef*) data->port, data->pin_number,
			GPIO_PIN_SET);
}

static void SCHEDULER_TASK1_Start(void) {
	UAL_TRAFFIC_LIGHT_handle(&traffic_light);

	sprintf((char*) data, "'TASK1' task is executed\n");
	len = strlen((char*) data);
	HAL_UART_Transmit(&huart1, data, len, 100);
}

static void SCHEDULER_DEFAULT_TASK_Start(void) {
	HAL_Delay(100);

	sprintf((char*) data, "'DEFAULT' task is executed\n");
	len = strlen((char*) data);
	HAL_UART_Transmit(&huart1, data, len, 100);
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
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */
	SCHEDULER_TASKS_Init();
	SCHEDULER_Init();
	TRAFFIC_LIGHT_Init();

	sprintf((char*) data, "MCU is initialized\n");
	len = strlen((char*) data);
	HAL_UART_Transmit(&huart1, data, len, 100);

	UAL_SCHEDULER_Start(&scheduler);

	/* USER CODE END 2 */

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
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
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
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14,
			GPIO_PIN_RESET);

	/*Configure GPIO pins : PB12 PB13 PB14 */
	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
