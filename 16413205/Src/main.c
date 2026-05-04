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
uint8_t RX_BUFF[20];
uint8_t RX_stand;
uint8_t stc;
uint8_t RX_start;
uint8_t state;
char buf[30];
float adc_value;
double adc_Value;
uint8_t mode;   //模式的切换
uint8_t start_stop;
uint8_t RX_BUF[20];   //串口数据缓冲区
uint8_t stand;    //串口接收标志位
char  REC_buf[10]={'2','5','1','2'};  //最大数据缓冲区
uint8_t CNT;    //定时器标志位
uint8_t cnt;     //计数值
uint8_t ucled;    //LED变量

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
//重定义函数
int fputc(int ch,FILE *f)
{
  HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,50);
  return ch;
}
//LED
void led_display(uint8_t led)
{
   HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);
	 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
	|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,1);
	HAL_GPIO_WritePin(GPIOC,led<<8,0);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,0);
}
//anjianshaoiao
uint8_t key_scan()
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
//LCD
void lcd_show()
{
   switch(state)
	 {
		 case 0:
			 switch(mode)
			 {
				 case 0:
					 LCD_DisplayStringLine(Line1,(uint8_t *)"       DATA         ");
		       sprintf(buf,"       R37:%.2f",adc_Value);
		       LCD_DisplayStringLine(Line3,(uint8_t *)buf);
		       sprintf(buf,"       MODE:KEY        ");
		       LCD_DisplayStringLine(Line4,(uint8_t *)buf);
		       sprintf(buf,"       COUNT:%d  ",cnt);
		       LCD_DisplayStringLine(Line5,(uint8_t *)buf);
					 break;
				 case 1:
					 LCD_DisplayStringLine(Line1,(uint8_t *)"       DATA         ");
		       sprintf(buf,"       R37:%.2f",adc_Value);
		       LCD_DisplayStringLine(Line3,(uint8_t *)buf);
		       sprintf(buf,"       MODE:UART        ");
		       LCD_DisplayStringLine(Line4,(uint8_t *)buf);
		       sprintf(buf,"       COUNT:%d  ",cnt);
		       LCD_DisplayStringLine(Line5,(uint8_t *)buf);
					 break;
				 default:
					 break;
			 }
			 
			 break;
		 case 1:
			  LCD_DisplayStringLine(Line1,(uint8_t *)"       PARA         ");
		   sprintf(buf,"       VMAX:%.1f     ",((REC_buf[0]-48)*10.0+REC_buf[1]-48)/10.0);
		   LCD_DisplayStringLine(Line3,(uint8_t *)buf);
		   sprintf(buf,"       VMIN:%.1f        ",((REC_buf[2]-48)*10.0+REC_buf[3]-48)/10.0);
		   LCD_DisplayStringLine(Line4,(uint8_t *)buf);
		   sprintf(buf,"                       ");
		   LCD_DisplayStringLine(Line5,(uint8_t *)buf);
			 break;
		 default:
			 break;
	 }
}
//按键处理函数
void key_pro(uint8_t Key)//
{
    switch(Key)
		{
			case 1:
				state++;
			  if(state>1)
				{
					state=0;
				}
				break;
			case 2:
				mode++;
			  if(mode>1)
				{
				mode=0;
				}
				break;
			case 3:
				HAL_UART_Transmit(&huart1,RX_BUF,5,10);
				break;
			case 4:
				start_stop++;
			if(start_stop>1)
			{
				start_stop=0;
			}
				break;
			default:
				break;
		}
}
//adc处理函数
void ADC_pro()
{
	if(start_stop==0)
	{
	HAL_ADC_Stop(&hadc2);
	}
	else
	{
	HAL_ADC_Start(&hadc2);
	}
	adc_value=HAL_ADC_GetValue(&hadc2);
	adc_Value=adc_value*3.3/4096;
}
//串口数据处理函数
void UART_pro()
{
  if(stand==1)
		{
		if(mode==1)
		{
		if(strstr((char*)RX_BUF,"STATE"))
		{
		  start_stop=1;
		}
		if(strstr((char*)RX_BUF,"END"))
		{
		start_stop=0;
		}
	 }
   else
	 { 
		   printf("ERROR");
	 }
	 if(strstr((char*)RX_BUF,"V")&&strstr((char*)RX_BUF,","))
	 {
		   REC_buf[0]=RX_BUF[0];
			 REC_buf[1]=RX_BUF[2];
			 REC_buf[2]=RX_BUF[5];
			 REC_buf[3]=RX_BUF[7];
			
	 }
	 stand=0;
//	 sprintf((char*)RX_BUFF,"receive:%s\r\n",(char*)REC_buf);
//	 HAL_UART_Transmit_DMA(&huart1,RX_BUFF,strlen((char*)RX_BUFF));
	 memset(RX_BUF,0,20);
 }	 
}
//计数函数
void CNT_pro()
{
  if(CNT==1&&adc_Value>((REC_buf[0]-48)*10.0+REC_buf[1]-48)/10.0)
	{
	   cnt++;
		 CNT=0;
	}
	if(CNT==1&&adc_Value<((REC_buf[2]-48)*10.0+REC_buf[3]-48)/10.0)
	{
	  cnt+=2;
		CNT=0;
	}
}
//LED处理函数
void LED_pro()
{
   if(state==0)
	 {
	   ucled |=0x01;//点亮led1
	 }
	 else
	 {
	 ucled &=~0x01;   //熄灭led1
	 }
	 if(state==1)
	 {
	   ucled |=0x02;//点亮led2
	 }
	 else
	 {
	 ucled &=~0x02;   //熄灭led2
	 }
	 if(cnt>3)
	 {
	    ucled |=0x04;//点亮led3
	 }
	 else
	 {
	  ucled &=~0x04;   //熄灭led3
	 }
	 if(mode==1)
	 {
	   ucled |=0x80;//点亮led8
	 }
	 else
	 {
	 ucled &=~0x80;   //熄灭led8
	 }
	led_display(ucled);
}
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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_ADC2_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);  ///校准ADC
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1,RX_BUF,20);     
  HAL_TIM_Base_Start_IT(&htim6);	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
//		printf("111\r\n");
//		HAL_Delay(100);
	uint8_t KEy;
	  lcd_show();
		KEy=key_scan();
		key_pro(KEy);
		ADC_pro();
		UART_pro();
		CNT_pro();
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
//串口接收函数
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
   if(huart->Instance==USART1)
	 {
		 stand=1;
	   HAL_UARTEx_ReceiveToIdle_DMA(&huart1,RX_BUF,20);
	 }
}
//定时器回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
   if(htim->Instance==TIM6)
	 {
	     CNT=1;
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
