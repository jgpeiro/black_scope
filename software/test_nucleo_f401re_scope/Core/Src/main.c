/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
	int i;
	for(i = 0; i < len; i++)
	{
		ITM_SendChar(*ptr++);
	}
    return len;
}

#define BUFFER_LEN (4096)
uint16_t buffer[BUFFER_LEN];
int state = 0;
int cndtr = 0;
int offset_h = 50;
int scale_h = 1000;
int trigger_v = 2048;
int channel = 0;
int slope = 1; // rising
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if( state == 0 )
    {
    	LL_ADC_DisableIT_AWD1(hadc1.Instance);
    	if( slope == 1 )
    	{
        	hadc1.Instance->HTR = 0x0FFF;
        	hadc1.Instance->LTR = trigger_v;
    	}
    	else
    	{
        	hadc1.Instance->HTR = trigger_v;
        	hadc1.Instance->LTR = 0x0000;
    	}
    	__HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD);
        LL_ADC_EnableIT_AWD1(hadc1.Instance);
    	state = 1;
    }
}
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
	if( state == 1 )
	{
		LL_ADC_DisableIT_AWD1(hadc1.Instance);
    	if( slope == 1 )
    	{
    		hadc1.Instance->HTR = trigger_v;
    		hadc1.Instance->LTR = 0x0000;
    	}
    	else
    	{
        	hadc1.Instance->HTR = 0x0FFF;
        	hadc1.Instance->LTR = trigger_v;
    	}
    	__HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD);
		LL_ADC_EnableIT_AWD1(hadc1.Instance);
		state = 2;
	}
	else if( state == 2 )
	{
		LL_ADC_DisableIT_AWD1(hadc1.Instance);

        __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_CC1);
        __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_CC2);
        __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);
        __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_CC1);
        __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_CC2);
        __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);

		HAL_TIM_OnePulse_Start_IT(&htim3, TIM_CHANNEL_1);
		HAL_TIM_Base_Start(&htim3);
		state = 3;
	}
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef* htim)
{
	if( state == 3 )
	{
		extern DMA_HandleTypeDef hdma_adc1;
		cndtr = hdma_adc1.Instance->NDTR;

		// Stop the one-pulse mode for TIM stop signal
		HAL_TIM_Base_Stop(&htim3);
		HAL_TIM_OnePulse_Stop_IT(&htim3, TIM_CHANNEL_1);

		// Stop the TIM used for the horizontal synchronization stop signal
		HAL_TIM_Base_Stop(&htim2);
		state= 4;
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
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  unsigned int trigger_h = 0;
  unsigned int n = 0;

  htim2.Init.Prescaler = (SystemCoreClock / (scale_h * 1000))/2 - 1;
  htim2.Init.Period = 2 - 1;
  HAL_TIM_Base_Init(&htim2);

  htim3.Init.Prescaler = (SystemCoreClock / (scale_h * 1000))/2 - 1;
  htim3.Init.Period = 2*(BUFFER_LEN/2 + offset_h) - 1;
  HAL_TIM_Base_Init(&htim3);

  channel = ADC_CHANNEL_0;
  hadc1.Instance->CR1 &= ~ADC_CR1_AWDCH;
  hadc1.Instance->CR1 |= channel;

  __HAL_DBGMCU_FREEZE_TIM2();
  __HAL_DBGMCU_FREEZE_TIM3();

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

  LL_ADC_DisableIT_AWD1(hadc1.Instance);
  __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)buffer, BUFFER_LEN);
  HAL_TIM_Base_Start(&htim2);

#include <stdlib.h>
  HAL_Delay(10);
  srand( buffer[0] );
  int ms = rand()&0xFF;
  HAL_Delay( ms );
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
  while( state != 4 )
  {
	  HAL_Delay(1);
  }

  trigger_h = (BUFFER_LEN-cndtr)-BUFFER_LEN/2-BUFFER_LEN/2;
  trigger_h %= BUFFER_LEN;
  printf( "%d, %d%\n", cndtr, trigger_h );
  printf( "\n" );
  for( int i = 0 ; i < BUFFER_LEN ; i++ )
  {
	  n = trigger_h+i;
	  n %= BUFFER_LEN;
	  if( (i&0x01) == 0 )
	  {
		  printf( "%d, %d, %d\n", i, buffer[n+0], buffer[n+1] );
		  HAL_Delay(1);
	  }
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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

#ifdef  USE_FULL_ASSERT
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
