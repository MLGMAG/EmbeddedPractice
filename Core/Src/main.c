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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* Definitions for task1 */
osThreadId_t task1Handle;
const osThreadAttr_t task1_attributes = { .name = "task1",
		.stack_size = 128 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for task2 */
osThreadId_t task2Handle;
const osThreadAttr_t task2_attributes = { .name = "task2",
		.stack_size = 128 * 4, .priority = (osPriority_t) osPriorityLow, };
/* Definitions for task3 */
osThreadId_t task3Handle;
const osThreadAttr_t task3_attributes = { .name = "task3",
		.stack_size = 128 * 4, .priority = (osPriority_t) osPriorityLow, };
/* USER CODE BEGIN PV */

const osMutexAttr_t mutex1_attributes = { "MUTEX1", 0U,
NULL, 0U };

const osMutexAttr_t mutex2_attributes = { "MUTEX2", 0U,
NULL, 0U };

static osMutexId_t mutex1_id;
static osMutexId_t mutex2_id;

static uint8_t data[100];
static uint16_t data_len;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
void TASK1_Start(void *argument);
void TASK2_Start(void *argument);
void TASK3_start(void *argument);

/* USER CODE BEGIN PFP */
static void Mutex_CheckStatus(const osStatus_t*, const char*);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void Mutex_CheckStatus(const osStatus_t *status,
		const char *task_details) {
	if (*status != osOK) {
		sprintf((char*) data, "Queue task '%s' is failed with code %d\n",
				task_details, *status);
		data_len = strlen((char*) data);
		HAL_UART_Transmit(&huart1, data, data_len, 100);
	}
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

	sprintf((char*) data, "MCU is initialized\n");
	data_len = strlen((char*) data);
	HAL_UART_Transmit(&huart1, data, data_len, 100);

	/* USER CODE END 2 */

	/* Init scheduler */
	osKernelInitialize();

	/* USER CODE BEGIN RTOS_MUTEX */
	mutex1_id = osMutexNew(&mutex1_attributes);
	if (mutex1_id == NULL) {
		Error_Handler();
	}
	mutex2_id = osMutexNew(&mutex2_attributes);
	if (mutex2_id == NULL) {
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
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* creation of task1 */
	task1Handle = osThreadNew(TASK1_Start, NULL, &task1_attributes);

	/* creation of task2 */
	task2Handle = osThreadNew(TASK2_Start, NULL, &task2_attributes);

	/* creation of task3 */
	task3Handle = osThreadNew(TASK3_start, NULL, &task3_attributes);

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
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_TASK1_Start */
/**
 * @brief  Function implementing the task1 thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_TASK1_Start */
void TASK1_Start(void *argument) {
	/* USER CODE BEGIN 5 */
	osStatus_t status;
	const char *details_message;
	/* Infinite loop */
	while (1) {
		details_message = "task1 acquire mutex1";
		status = osMutexAcquire(mutex1_id, osWaitForever);
		Mutex_CheckStatus(&status, details_message);
		if (status == osOK) {
			sprintf((char*) data, "%s\n", details_message);
			data_len = strlen((char*) data);
			HAL_UART_Transmit(&huart1, data, data_len, 100);
		}

		osDelay(500);

		details_message = "task1 acquire mutex2";
		status = osMutexAcquire(mutex2_id, osWaitForever);
		Mutex_CheckStatus(&status, details_message);
		if (status == osOK) {
			sprintf((char*) data, "%s\n", details_message);
			data_len = strlen((char*) data);
			HAL_UART_Transmit(&huart1, data, data_len, 100);
		}

		osDelay(500);

		details_message = "task1 release mutex2";
		status = osMutexRelease(mutex2_id);
		Mutex_CheckStatus(&status, details_message);
		if (status == osOK) {
			sprintf((char*) data, "%s\n", details_message);
			data_len = strlen((char*) data);
			HAL_UART_Transmit(&huart1, data, data_len, 100);
		}

		osDelay(500);

		details_message = "task1 release mutex1";
		status = osMutexRelease(mutex1_id);
		Mutex_CheckStatus(&status, details_message);
		if (status == osOK) {
			sprintf((char*) data, "%s\n", details_message);
			data_len = strlen((char*) data);
			HAL_UART_Transmit(&huart1, data, data_len, 100);
		}

		osDelay(500);
	}
	/* USER CODE END 5 */
}

/* USER CODE BEGIN Header_TASK2_Start */
/**
 * @brief Function implementing the task2 thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_TASK2_Start */
void TASK2_Start(void *argument) {
	/* USER CODE BEGIN TASK2_Start */
	osStatus_t status;
	const char *details_message;
	/* Infinite loop */
	while (1) {
		details_message = "task2 acquire mutex2";
		status = osMutexAcquire(mutex2_id, osWaitForever);
		Mutex_CheckStatus(&status, details_message);
		if (status == osOK) {
			sprintf((char*) data, "%s\n", details_message);
			data_len = strlen((char*) data);
			HAL_UART_Transmit(&huart1, data, data_len, 100);
		}

		osDelay(500);

		details_message = "task2 acquire mutex1";
		status = osMutexAcquire(mutex1_id, osWaitForever);
		Mutex_CheckStatus(&status, details_message);
		if (status == osOK) {
			sprintf((char*) data, "%s\n", details_message);
			data_len = strlen((char*) data);
			HAL_UART_Transmit(&huart1, data, data_len, 100);
		}

		osDelay(500);

		details_message = "task2 release mutex1";
		status = osMutexRelease(mutex1_id);
		Mutex_CheckStatus(&status, details_message);
		if (status == osOK) {
			sprintf((char*) data, "%s\n", details_message);
			data_len = strlen((char*) data);
			HAL_UART_Transmit(&huart1, data, data_len, 100);
		}

		osDelay(500);

		details_message = "task2 release mutex2";
		status = osMutexRelease(mutex2_id);
		Mutex_CheckStatus(&status, details_message);
		if (status == osOK) {
			sprintf((char*) data, "%s\n", details_message);
			data_len = strlen((char*) data);
			HAL_UART_Transmit(&huart1, data, data_len, 100);
		}

		osDelay(500);
	}
	/* USER CODE END TASK2_Start */
}

/* USER CODE BEGIN Header_TASK3_start */
/**
 * @brief Function implementing the task3 thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_TASK3_start */
void TASK3_start(void *argument) {
	/* USER CODE BEGIN TASK3_start */
	/* Infinite loop */
	const char *details_message;
	while (1) {
		details_message = "Doing work...";
		sprintf((char*) data, "%s\n", details_message);
		data_len = strlen((char*) data);
		HAL_UART_Transmit(&huart1, data, data_len, 100);

		osDelay(500);

		details_message = "Work is done!";
		sprintf((char*) data, "%s\n", details_message);
		data_len = strlen((char*) data);
		HAL_UART_Transmit(&huart1, data, data_len, 100);

		osDelay(500);
	}
	/* USER CODE END TASK3_start */
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
