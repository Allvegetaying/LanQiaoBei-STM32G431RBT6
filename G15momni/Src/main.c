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
#include "adc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t state;   //系统状态
char buf[40];      //数据缓存区
uint16_t adc=0;      //adc采集到的值
char adc_buf[80];  //adc数组缓冲区
float instance;    //距离
char RX_buf[20];    //串口数据缓冲区
uint8_t stand;      //串口数据数量参数
uint8_t uart;        //串口数据接收完成标志位
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//重定义函数
int fputc(int ch,FILE *f)
{
    HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,50);
	return ch;
}
//底层led控制
void led_display(uint8_t led)
{
   HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);
	 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
	|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,1);
	HAL_GPIO_WritePin(GPIOC,led<<8,0);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,0);
}
//按键扫描
uint8_t key_scan()
{
   static uint8_t key=1;
	if(key==1&&(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==0||HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==0||HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==0||HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==0))
	{
		key=0;
		HAL_Delay(10);
	    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==0)
			{
			  return 1;
			}
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==0)
			{
			  return 2;
			}
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==0)
			{
			  return 3;
			}
			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==0)
			{
			  return 4;
			}
	}
 if(key==0&&HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==1&&HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==1&&HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==1&&HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1)
 {
   key=1;
 }
return 0;
}
//按键处理函数
void key_pro(uint8_t KEY)
{
    switch(KEY)
		{
		  case 1:
				state++;
			  if(state>1)
				{
				state=0;
				}
				break;
			case 2:
				
			
			
				break;
			case 3:
		HAL_UART_Transmit(&huart1,(uint8_t *)RX_buf,1,10);
			
				break;
			case 4:
				
			
			
			
				break;
			default:
				break;
		}
}
//lcd显示函数
void lcd_show()
{
   switch(state)
	 {
	   case 0:
			 LCD_DisplayStringLine(Line2,(uint8_t *)"       D A T A       ");
		   sprintf(buf,"       N:S       ");
		   LCD_DisplayStringLine(Line4,(uint8_t *)buf);
		   sprintf(buf,"       D:%.1f       ",instance*1.0);
		   LCD_DisplayStringLine(Line6,(uint8_t *)buf);
			 break;
		 case 1:
			 LCD_DisplayStringLine(Line2,(uint8_t *)"       W A R N        ");
		
		   LCD_DisplayStringLine(Line4,(uint8_t *)"                        ");
		 
		   LCD_DisplayStringLine(Line6,(uint8_t *)"                       ");
			 break;
		 default:
			 break;
	 }
}
//ADC处理函数
void ADC_pro()
{
   HAL_ADC_Start(&hadc2);
	 adc=HAL_ADC_GetValue(&hadc2);
	 sprintf(adc_buf,"  adc_value:  %.3f   ",(adc*3.3)/4096);
   LCD_DisplayStringLine(Line8,(uint8_t *)adc_buf);
}
void data_pro()
{
	   if((adc*3.3)/4096<3.0)
		 {
     instance=100.0*((adc*3.3)/4096);
		 }
		 else
		 {
		 instance=300;
		 }
}
void UART_pro()
{
    if(uart==1)
		{
			uart=0;
		 if(RX_buf[0]=='L')
		 {
		   printf("Success\r\n");
		 }
		 else
		 {
		 printf("Warn\r\n");
		 }
		 if(RX_buf[0]=='R')
		 {
		   printf("Success\r\n");
		 }
		 else
		 {
		 printf("Warn\r\n");
		 }
		 if(RX_buf[0]!='R'||RX_buf[0]!='L')
		 {
		 printf("ERROR\r\n");
		 }
		}
}
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
  MX_USART1_UART_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
	LCD_Clear(White);
	LCD_SetBackColor(White);
	LCD_SetTextColor(Magenta);
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		uint8_t Key;
		Key=key_scan();
		key_pro(Key);
		lcd_show();
		ADC_pro();
		data_pro();
		UART_pro();
//		printf("1111");
		
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}
/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
     if(huart->Instance==USART1)
		 {
		   RX_buf[stand]=(uint8_t)(huart1.Instance->RDR&(uint8_t)0x00ff);
			 stand++;
			 __HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_RXNE);
		    if(stand==1)
				{
				  stand=0;
					uart=1;
				}
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
