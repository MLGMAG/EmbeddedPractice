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
#define SERVO_MAX_ANGLE 180
#define SERVO_MIN_ANGLE 0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
static volatile uint8_t servo_pwm_flag = 0;
static volatile uint8_t buzzer_pwm_flag = 0;
static volatile uint8_t uart_flag = 0;
static volatile uint8_t encoder_flag = 0;

static float servo_angle = 0;
static uint16_t servo_period = 2000;
static uint16_t servo_counter = 0;

static uint8_t uart_data[100];
static uint16_t uart_data_len;
static uint16_t uart_delay_counter = 0;

static button_info_t encoder_a_contact = {
		.last_value = GPIO_PIN_SET,
		.state = STATE_IDLE,
		.button_state_recording = {GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET},
		.recording_counter = 0
};
static button_info_t encoder_b_contact = {
		.last_value = GPIO_PIN_SET,
		.state = STATE_IDLE,
		.button_state_recording = {GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET},
		.recording_counter = 0
};
static button_info_t encoder_click_contact = {
		.last_value = GPIO_PIN_SET,
		.state = STATE_IDLE,
		.button_state_recording = {GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET},
		.recording_counter = 0
};
static uint8_t encoder_multiplier = 1;

static uint8_t enable_buzzer = 0;
static uint16_t buzzer_period = 681;
static uint16_t buzzer_duty = 340;
static uint16_t buzzer_delay_iter = 0;
static uint16_t buzzer_counter = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

float convert_duty_to_angle(uint8_t duty) {
	if (duty > 250)
	{
		return SERVO_MAX_ANGLE;
	}

	if (duty < 50)
	{
		return SERVO_MIN_ANGLE;
	}

	return ((float) (duty - 50)) / 200 * SERVO_MAX_ANGLE;
}

uint8_t convert_angle_to_duty(float angle) {
	if (angle > SERVO_MAX_ANGLE)
	{
		return 250;
	}

	if (angle < SERVO_MIN_ANGLE)
	{
		return 50;
	}

	return (uint8_t) (angle / SERVO_MAX_ANGLE * 200 + 50);
}

void button_info_update(GPIO_PinState current_value, button_info_t *button_info) {
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

void ENC_Buttons_Read() {
	if (encoder_flag == 1) {
		button_info_update(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0), &encoder_a_contact);
		button_info_update(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1), &encoder_b_contact);
		button_info_update(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2), &encoder_click_contact);

		if (encoder_click_contact.state == STATE_RELEASE) {
			encoder_multiplier = encoder_multiplier == 1 ? 5 : 1;
		}

		int8_t encoder_diff = 0;
		if (encoder_a_contact.state == STATE_RELEASE && encoder_b_contact.state == STATE_HELD) {
			encoder_diff += 1 * encoder_multiplier;
		}

		if (encoder_b_contact.state == STATE_RELEASE && encoder_a_contact.state == STATE_HELD) {
			encoder_diff += -1 * encoder_multiplier;
		}

		if (servo_angle + encoder_diff > SERVO_MAX_ANGLE ||
				servo_angle + encoder_diff < SERVO_MIN_ANGLE)
		{
			enable_buzzer = 1;
		}
		else
		{
			servo_angle += encoder_diff;
		}

		encoder_flag = 0;
	  }
}

void BUZZER_Update() {
	if (buzzer_pwm_flag == 1 && enable_buzzer == 1)
	{

		if (buzzer_counter == buzzer_period)
		{
			buzzer_counter = 0;
			buzzer_delay_iter++;
			if (buzzer_delay_iter == 20)
			{
				buzzer_delay_iter = 0;
				enable_buzzer = 0;
			}
		}

		if (buzzer_counter <= buzzer_duty)
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
		}

		if (buzzer_counter > buzzer_duty)
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
		}

		buzzer_counter++;
		buzzer_pwm_flag = 0;
	}
}

void SERVO_Angle_Set() {
	if (servo_pwm_flag == 1)
	{
		servo_counter++;

		if (servo_counter == servo_period)
		{
			servo_counter = 0;
		}

		uint16_t servo_duty = convert_angle_to_duty(servo_angle);
		if (servo_counter < servo_duty)
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
		}

		if (servo_counter >= servo_duty)
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		}

		servo_pwm_flag = 0;
	}
}

void UART_ServoAngle_Log() {
	if (uart_flag == 1) {
		uart_delay_counter++;

		if (uart_delay_counter == 1000) {
			uart_delay_counter = 0;

			sprintf((char *) uart_data, "Servo Angle is %f\n", servo_angle);
			uart_data_len = strlen((char *) uart_data);
			HAL_UART_Transmit(&huart2, uart_data, uart_data_len, 100);
		}

		uart_flag = 0;
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
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_Base_Start_IT(&htim2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  ENC_Buttons_Read();



	  SERVO_Angle_Set();
	  UART_ServoAngle_Log();
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
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
  htim1.Init.Prescaler = 100 - 1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 10 - 1;
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
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 100 -1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM1)
	{
		servo_pwm_flag = 1;
		buzzer_pwm_flag = 1;
	}

	if (htim->Instance == TIM2)
	{
		uart_flag = 1;
		encoder_flag = 1;
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
