/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "gpio.h"
#include "MPU6050.h"
#include "ssd1306.h"
#include <math.h>

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
MPU6050_t MPU6050;          // 存加速度
char oled_buf[32];          // 临时字符串
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_I2C1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	if (MPU6050_Init(&hi2c1) !=0 ) {
		Error_Handler();// 失败就停
	}
	if (ssd1306_Init(&hi2c1) != 0) {
    Error_Handler();          // 初始化失败就停
	}
	
	HAL_TIM_Base_Start_IT(&htim2);
	
	
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
		if (htim->Instance == TIM2)        // 只处理 TIM2
		{
				/* 这里写 1 秒要做的事情 */
//				/* 采样 */
//			MPU6050_Read_Accel(&hi2c1, &MPU6050);
//	
//			/* 组字符串 */
//			sprintf(oled_buf, "Ax:%.2fg", MPU6050.Ax);
//			ssd1306_SetCursor(0, 0);
//			ssd1306_WriteString(oled_buf, Font_7x10, White);

//			sprintf(oled_buf, "Ay:%.2fg", MPU6050.Ay);
//			ssd1306_SetCursor(0, 10);
//			ssd1306_WriteString(oled_buf, Font_7x10, White);

//			sprintf(oled_buf, "Az:%.2fg", MPU6050.Az);
//			ssd1306_SetCursor(0, 20);
//			ssd1306_WriteString(oled_buf, Font_7x10, White);

//			/* 一次性刷新到屏幕 */
//			ssd1306_UpdateScreen(&hi2c1);
		static double buf[5] = {0};              // 存 10 次角度
		static uint8_t idx = 0;                   // 循环指针
		
		MPU6050_Read_All(&hi2c1, &MPU6050);
		
		double pitch = atan2(-MPU6050.Ax, MPU6050.Az) * 57.3;	
    buf[idx] = pitch;                         // 存新角度
		idx = (idx + 1) % 10;                     // 循环存
		double avg = 0;
		for (uint8_t i = 0; i < 5; i++) avg += buf[i];
		avg /= 5;                              // 取平均
		int len = (int)fabs(avg) * 2;
		ssd1306_Fill(Black);
		for(int i=0;i<len;i++)
		{			
			ssd1306_DrawPixel(64-i, 40, White);			
		}ssd1306_UpdateScreen(&hi2c1);
		
//		if(avg<0)
//		{
//			int len = (int)fabs(avg) * 2;
//			ssd1306_Fill(Black);
//			for(int i=0;i<len;i++)
//			{			
//				ssd1306_DrawPixel(64-i, 40, White);			
//			}ssd1306_UpdateScreen(&hi2c1);
//		}
//		else
//		{
//			int len = (int)fabs(avg) * 2;
//			ssd1306_Fill(Black);
//			for(int i=0;i<len;i++)	
//			{
//				ssd1306_DrawPixel(64+i, 40, White);		
//			}ssd1306_UpdateScreen(&hi2c1);
//		}
		
			
		}	
}		
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
