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
#include "dma.h"
#include "tim.h"
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


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t state;   //系统状态
char BUFF[20];   //缓冲区
float adc_value;  //得到的adc计数值
float ADC_value;   //得到的adc电压值
float Value;     //距离
char buf[3]={'S','R','L'};    //车辆的三个方向
char N;     //车辆的方向
uint8_t TIM;    //计时变量
uint8_t RX_BUF[20];    //串口数据缓冲区
uint8_t uart_flag;   //串口标志位
uint8_t RX_buff[20];    //串口缓冲区2
uint8_t Key;     //获取的按键值
uint8_t ucled;    //控制LED的变量
uint8_t uctim;    //定时器7控制led灯
uint8_t stand;       //LED灯1变量
uint8_t stand1;     //led灯2变量
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//重定义函数
int fputc(int ch,FILE *f)
{
   HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,50); 
	return ch;
}
//LED灯控制函数
void LED_dispaly(uint8_t led)
{
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
	|GPIO_PIN_14|GPIO_PIN_15,1);
	HAL_GPIO_WritePin(GPIOC,led<<8,0);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,0);
}
//按键扫描
uint8_t KEY_scan()
{
	static uint8_t KEY=1;
    if(KEY==1&&(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==0||HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==0||HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==0||HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==0))
		{
			KEY=0;
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
    if(KEY==0&&HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==1&&HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==1&&HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==1&&HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1)
		{
			KEY=1;
		}
		return 0;
}
//LCD显示函数
void LCD_show()
{
  switch(state)
	{
			case 0:
				LCD_DisplayStringLine(Line1,(uint8_t*)"        DATA     ");
			  sprintf(BUFF,"       N:%c",N);
			  LCD_DisplayStringLine(Line3,(uint8_t *)BUFF);
			  sprintf(BUFF,"       D:%.1f ",Value);
			  LCD_DisplayStringLine(Line4,(uint8_t *)BUFF);
				break;
				case 1:
				LCD_DisplayStringLine(Line1,(uint8_t*)"                         ");
				LCD_DisplayStringLine(Line3,(uint8_t *)"                        ");
			  sprintf(BUFF,"        WARN              ");
			  LCD_DisplayStringLine(Line4,(uint8_t *)BUFF);
			break;
				default:
					break;
	}
}
//ADC处理函数
void ADC_PRO()
{
  HAL_ADC_Start(&hadc2);
	adc_value=HAL_ADC_GetValue(&hadc2);
	ADC_value=adc_value*3.3/4096;
	Value=ADC_value*100;
	if(ADC_value>3.0)
	{
	Value=300;
	}
}
//按键处理函数
void KEY_deal_pro(uint8_t key)
{
	if(state==1)
		{
   if(key==1)    //按键1按下的话
	 {
			 state=0;
		   N=buf[0];    //方向为直行
		   printf("Success\r\n");
	 }
  }
		if(state==0)
		{
	 if(N=='L')     //接收到L字符的话
	 {
	   HAL_TIM_Base_Start_IT(&htim6);
	    if(TIM>=0&&TIM<=5)
			{
				stand=1;
				if(key==3)
				{
				stand=0;
				N=buf[0];
				TIM=0;
				HAL_TIM_Base_Stop_IT(&htim6);
				printf("Success\r\n");
				}
			}
			else
			{
			stand=0;
			state=1;
			TIM=0;
			HAL_TIM_Base_Stop_IT(&htim6);
			printf("Warn\r\n");
			}
	 }
	 else if(N=='R')     //接收到L字符的话
	 {
	   HAL_TIM_Base_Start_IT(&htim6);
	    if(TIM>=0&&TIM<=5)
			{
				stand1=1;
				if(key==4)
				{
			  stand1=0;
				N=buf[0];
				TIM=0;
				HAL_TIM_Base_Stop_IT(&htim6);
				printf("Success\r\n");
				}
			}
			else
			{
			stand1=0;
			state=1;
			TIM=0;
			HAL_TIM_Base_Stop_IT(&htim6);
			printf("Warn\r\n");
			}
	 }
	else if(N!='R'||N!='L')
	 {
	    if(key==3||key==4)
			{
			printf("Warn\r\n");
			}
	 }
 }
 }
//串口数据处理函数
void uart_deal_pro()
{
	if(uart_flag==1)
	{
		if(state==0)
		{
		uart_flag=0;
		if(RX_BUF[0]=='L'&&strlen((char *)RX_BUF)==1)
		{
			N=buf[2];
		}
		if(RX_BUF[0]=='R'&&strlen((char *)RX_BUF)==1)
		{
			N=buf[1];
		}
		if((RX_BUF[0]!='R'||RX_BUF[0]!='L')&&strlen((char *)RX_BUF)>1)
		{
			printf("ERROR\r\n");
		}
	 }
		if(state==1)
		{
		  if(uart_flag==1)
			{
				uart_flag=0;
			  printf("WAIT\r\n");
			}
		}
		memset(RX_BUF,0,20);
	}
}
//led处理函数
void LED_pro()
{
  if(state==1)
	{
	  ucled|=0x80;
	}
	LED_dispaly(ucled);
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
  MX_ADC2_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);     //校准adc
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1,RX_BUF,20);
  HAL_TIM_Base_Start_IT(&htim7);
	N=buf[0];
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		LCD_show();
		ADC_PRO();
		Key=KEY_scan();
		KEY_deal_pro(Key);
		uart_deal_pro();
		LED_pro();
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
//定时器中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance==TIM6)
	{
		TIM++;
	}
	if(htim->Instance==TIM7)
	{
		if(stand==1)
		{
		uctim++;
			if(uctim%2==0)
			{
			  ucled|=0x01;
			}
			else
			{
		  ucled&=~0x01;
			}
		}
		else
		{
		ucled&=~0x01;
		}	
		if(stand1==1)
		{
		uctim++;
			if(uctim%2==0)
			{
			  ucled|=0x02;
			}
			else
			{
		  ucled&=~0x02;
			}
		}
		else
		{
		ucled&=~0x02;
		}	
	}
}
//串口回调函数
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  uart_flag=1;
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1,RX_BUF,20);
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
