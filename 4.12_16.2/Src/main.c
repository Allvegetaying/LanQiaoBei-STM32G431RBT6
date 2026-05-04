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
uint8_t RX_BUFF[20];   //串口数据缓冲区
char buf[80];     //缓冲区
char buf1[80];     //缓冲区
char buf3[80];     //缓冲区
uint8_t UAR_flag;  //串口标志位
uint8_t state=0;    //系统状态
uint8_t mode;     //控制模式
uint8_t start_stop_F1=0 ;   //控制F1的采集和停止
uint8_t start_stop_F2=0;   //控制F2的采集和停止
uint16_t PF=1000;     //频率参数变量
double compare1;    //捕获得到F1的计数值
double compare2;    //捕获得到F2的计数值
double frq1;        //F1的频率
double frq2;       //F2的频率

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//led控制函数
void LED_dispaly(uint8_t led)
{
   HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);
	 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
	|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,1);
	HAL_GPIO_WritePin(GPIOC,led<<8,0);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,0);
}
//按键扫描函数
uint8_t  KEY_scan()
{
  static uint8_t KEY=1;
	if(KEY==1&&(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==0||HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==0||HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==0||HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==0))
	{
		KEY=0;
		HAL_Delay(10);    //消抖
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
//重定义函数
int fputc(int ch,FILE *f)
{
  HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,50);
	return ch;
}
//LCD显示函数
void LCD_show()
{
  switch(state)
	{
		case 0:
			if(mode==0)
			{
			LCD_DisplayStringLine(Line1,(uint8_t*)"        DATA           ");
		  sprintf(buf3,"     F1=%.0f      ",frq1);
		  LCD_DisplayStringLine(Line3,(uint8_t*)buf3);
    	sprintf(buf1,"     F2=%.0f       ",frq2);
		  LCD_DisplayStringLine(Line4,(uint8_t*)buf1);
		  sprintf(buf,"     MODE=KEY");
		  LCD_DisplayStringLine(Line5,(uint8_t*)buf);
			}
			else
			{
			LCD_DisplayStringLine(Line1,(uint8_t*)"        DATA           ");
		  sprintf(buf3,"     F1=%.0f       ",frq1);
		  LCD_DisplayStringLine(Line3,(uint8_t*)buf3);
    	sprintf(buf,"     F2=%.0f       ",frq2);
		  LCD_DisplayStringLine(Line4,(uint8_t*)buf);
		  sprintf(buf,"     MODE=USART");
		  LCD_DisplayStringLine(Line5,(uint8_t*)buf);
			}
			break;
		case 1:
			LCD_DisplayStringLine(Line1,(uint8_t*)"        PARA           ");
		  sprintf(buf,"      PF=%d  ",PF);
		  LCD_DisplayStringLine(Line3,(uint8_t*)"                      ");
		  LCD_DisplayStringLine(Line4,(uint8_t*)"                      ");
		  LCD_DisplayStringLine(Line5,(uint8_t*)"                      ");
			break;
		default:
			break;
	}
}
//按键处理函数
void KEY_deal_pro(uint8_t key)
{
  switch(key)
	{
		case 1:  //按键1按下的话，系统切换
		state=!state;
			break;
		case 2:      //按键2按下的话，模式切换
		mode=!mode;
			break;
		case 3:
			if(state==0&&mode==0)
			{
			   start_stop_F1=!start_stop_F1;
				  if(start_stop_F1==1)
					{
					  HAL_TIM_IC_Stop_IT(&htim3,TIM_CHANNEL_1);           //停止采集
					}
					else
					{
					 HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);              //启动采集
					}
			}
			if(state==1)
			{
			   PF+=1000;
			}
			if(PF>10001)
				{
				  PF=1000;
				}
			break;
		case 4:
			if(state==0&&mode==0)
			{
			   start_stop_F2=!start_stop_F2;
				  if(start_stop_F2==1)
					{
					  HAL_TIM_IC_Stop_IT(&htim2,TIM_CHANNEL_1);                //停止采集
				
					}
					else
					{
					  HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);             //启动采集
					}
			}
			if(state==1)
			{
			   PF-=1000;
			}
			if(PF<1001)
				{
				  PF=10000;
				}
			break;
		default:
			break;
	}
}
//串口数据处理函数
void UART_deal_pro()
{
    if(UAR_flag==1)
		{
			UAR_flag=0;
			if(mode==1)
			{
			if(RX_BUFF[0]=='T'&&RX_BUFF[1]=='F'&&RX_BUFF[1]=='1'&&strlen((char *)RX_BUFF)==3)
			{
			start_stop_F1=0;    //启动F1
			}
			if(RX_BUFF[0]=='P'&&RX_BUFF[1]=='F'&&RX_BUFF[1]=='1'&&strlen((char *)RX_BUFF)==3)
			{
			start_stop_F1=1;     //停止F1采集
			}
			if(RX_BUFF[0]=='T'&&RX_BUFF[1]=='F'&&RX_BUFF[1]=='2'&&strlen((char *)RX_BUFF)==3)
			{
			start_stop_F2=0;      //启动F2
			}
			if(RX_BUFF[0]=='P'&&RX_BUFF[1]=='F'&&RX_BUFF[1]=='2'&&strlen((char *)RX_BUFF)==3)
			{
			start_stop_F2=1;      //停止F2采集
			}
			if(RX_BUFF[0]=='F'&&RX_BUFF[1]=='1'&&strlen((char *)RX_BUFF)==2)
			{
			  printf("F1:%.0f",frq1);
			}
			if(RX_BUFF[0]=='F'&&RX_BUFF[1]=='2'&&strlen((char *)RX_BUFF)==2)
			{
			  printf("F2:%.0f",frq2);
			}
		}
			if(mode==0)
			{
			 printf("NULL");
			}
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
  HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1); 
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1,RX_BUFF,20);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
			uint8_t Key;
		LCD_show();
		Key=KEY_scan();
		KEY_deal_pro(Key);
		UART_deal_pro();
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
//串口回调函数
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	UAR_flag=1;
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1,RX_BUFF,20);
}
//捕获回调函数
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM2)
	{
	  compare1=HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_1)+1;
	  __HAL_TIM_SetCounter(&htim2,0);
		frq1=(1000000*1.0)/(compare1+1);
		HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
	}
	if(htim->Instance==TIM3)
	{
	  compare2=HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_1)+1;
	  __HAL_TIM_SetCounter(&htim3,0);
		frq2=(1000000*1.0)/(compare2+1);
		HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
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
