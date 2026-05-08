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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
	STATE_IDLE,
	STATE_PRESSED,
	STATE_HELD,
	STATE_RELEASE
} BUTTON_STATE_T;

typedef struct {
	GPIO_PinState last_value;
	BUTTON_STATE_T state;
	GPIO_PinState button_state_recording[5];
	uint8_t recording_counter;
} button_info_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUTTON_STATE_RECORDING_LEN 5
#define RX_BUFFER_LEN 64
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */

static const uint8_t MCU_STARTUP_MESSAGE[] = "MCU_INIT\n";

static const uint8_t LED_TOGGLE_MESSAGE[] = "LED_TOGGLE";
static const uint8_t LED_TOGGLE_MESSAGE_LEN = strlen((char *) LED_TOGGLE_MESSAGE);

static volatile uint8_t tx_available = 1;
static volatile uint8_t tim1_interrupt = 0;
static volatile uint8_t button_press = 0;
static volatile uint8_t toggle_led_tx = 0;


static volatile uint8_t buffer[RX_BUFFER_LEN];
static volatile uint8_t rx_data[RX_BUFFER_LEN];
static volatile uint8_t rx_idle_interrupt = 0;
static volatile uint8_t rx_idle_state_current_index = 0;
static volatile uint8_t rx_idle_state_last_index = 0;
static volatile uint8_t message_available = 0;
static volatile uint16_t current_message_len = 0;
static volatile uint8_t *current_message_pointer = NULL;

static button_info_t button_input = {
		.last_value = GPIO_PIN_SET,
		.state = STATE_IDLE,
		.button_state_recording = {GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET},
		.recording_counter = 0
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static HAL_StatusTypeDef UART_MessageTransmit(const uint8_t *message) {
	HAL_StatusTypeDef result = HAL_BUSY;
	if (tx_available == 1) {
		tx_available = 0;

		uint16_t len = strlen((char *) message);
		result = HAL_UART_Transmit_DMA(&huart1, message, len);

		if (result != HAL_OK) {
			tx_available = 1;
		}
	}
	return result;
}

static void button_info_update(GPIO_PinState current_value, button_info_t *button_info) {
	  if (button_info->state == STATE_IDLE &&
			  current_value == GPIO_PIN_RESET &&
			  button_info->last_value == GPIO_PIN_SET)
	  {
		  button_info->state = STATE_PRESSED;
	  }

	  if (button_info->state == STATE_PRESSED &&
			  button_info->recording_counter < BUTTON_STATE_RECORDING_LEN)
	  {
		  button_info->button_state_recording[button_info->recording_counter] = current_value;
		  button_info->recording_counter++;
	  }

	  if (button_info->state == STATE_PRESSED &&
			  button_info->recording_counter == BUTTON_STATE_RECORDING_LEN)
	  {
		  for (int i = 0; i < button_info->recording_counter; i++)
		  {
			  if (button_info->button_state_recording[i] == GPIO_PIN_SET)
			  {
				  button_info->state = STATE_IDLE;
				  break;
			  }
		  }

		  if (button_info->state == STATE_PRESSED)
		  {
			  button_info->state = STATE_HELD;
		  }

		  button_info->recording_counter = 0;
	  }

	  if (button_info->state == STATE_HELD && current_value == GPIO_PIN_SET)
	  {
		  button_info->state = STATE_RELEASE;
	  }

	  if (button_info->state == STATE_RELEASE && button_info->recording_counter < 2)
	  {
		  button_info->recording_counter++;
	  }

	  if (button_info->state == STATE_RELEASE && button_info->recording_counter == 2)
	  {
		  button_info->recording_counter = 0;
		  button_info->state = STATE_IDLE;
	  }

	  button_info->last_value = current_value;
}

static void BUTTON_SignalReceive() {
	if (button_press == 1)
	{
		button_info_update(1, &button_input);
		HAL_TIM_Base_Start_IT(&htim1);
		button_press = 0;
	}

	if (tim1_interrupt == 1)
	{
		GPIO_PinState value = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13);
		button_info_update(value, &button_input);

		if (button_input.state == STATE_IDLE)
		{
			HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
		}

		if (button_input.state == STATE_PRESSED ||
			  button_input.state == STATE_HELD ||
			  button_input.state == STATE_RELEASE)
		{
			HAL_TIM_Base_Start_IT(&htim1);
		}

		if (button_input.state == STATE_RELEASE)
		{
			toggle_led_tx = 1;
		}

		tim1_interrupt = 0;
	}
}

static void BUTTON_SignalHandle() {
	if (toggle_led_tx == 1 && UART_MessageTransmit(LED_TOGGLE_MESSAGE) == HAL_OK)
	{
		toggle_led_tx = 0;
	}
}

static void UART_MessageReceive() {
	if (rx_idle_interrupt == 1)
	{
		if (rx_idle_state_current_index > rx_idle_state_last_index)
		{
			current_message_len = rx_idle_state_current_index - rx_idle_state_last_index;
			current_message_pointer = rx_data + rx_idle_state_last_index;
		} else
		{
			uint16_t left_part_len = RX_BUFFER_LEN - rx_idle_state_last_index;
			current_message_len = left_part_len + rx_idle_state_current_index;

			memcpy(buffer, (rx_data + rx_idle_state_last_index), left_part_len);
			memcpy((buffer + left_part_len), rx_data, rx_idle_state_current_index);
			current_message_pointer = buffer;
		}
		message_available = 1;
		rx_idle_interrupt = 0;
	}
}

static void UART_MessageHandle() {
	if (message_available == 1)
	{
		if (current_message_len == LED_TOGGLE_MESSAGE_LEN &&
				memcmp(current_message_pointer, LED_TOGGLE_MESSAGE, LED_TOGGLE_MESSAGE_LEN) == 0)
		{
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);
		}
		message_available = 0;
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

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
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  UART_MessageTransmit(MCU_STARTUP_MESSAGE);
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_data, RX_BUFFER_LEN);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  BUTTON_SignalReceive();
	  BUTTON_SignalHandle();

	  UART_MessageReceive();
	  UART_MessageHandle();

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_TIM1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 160 - 1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000 - 1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

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
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_13)
	{
		button_press = 1;
		HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim1)
	{
		tim1_interrupt = 1;
		HAL_TIM_Base_Stop_IT(&htim1);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == &huart1)
	{
		tx_available = 1;
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	if (huart == &huart1 &&
			huart->RxEventType == HAL_UART_RXEVENT_IDLE)
	{
		rx_idle_interrupt = 1;
		rx_idle_state_last_index = rx_idle_state_current_index;
		rx_idle_state_current_index = Size;
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
