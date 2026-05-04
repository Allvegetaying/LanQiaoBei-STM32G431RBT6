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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "stdio.h"
#include "string.h"
#include "i2c - hal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t state;   //系统状态
char buff[20];  //数据缓冲区
uint8_t ucled;    //LED灯标志位
uint8_t stand;    //密码位置标志位     
char BUff[3]={'0','1','2'};    //初始密码
char set_buf[3]={'*','*','*'};    //设置新密码数组
//lock
char one_site='*';         //第一位
char two_site='*';    // 第二位
char three_site='*';   // 第三位
//set
uint8_t one_site1='*';         //第一位
uint8_t two_site1='*';    // 第二位
uint8_t three_site1='*';   // 第三位

uint16_t ucadc;   //得到的ADC值
char adcbuf[40];   //ADC数据缓冲区
uint8_t key;     //按键值
int land=0;    
	
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//eeprom写操作
void eeprom_write(uint8_t add,uint8_t data)
{
   I2CStart();     //起始信号
	 I2CSendByte(0xa0);   //发送一个地址
	 I2CWaitAck();      //等待ACK应答
	 I2CSendByte(add);   //发送地址
	 I2CWaitAck();      //等待ACK应答
	 I2CSendByte(data);    //发送数据
	 I2CWaitAck();      //等待ACK应答
	 I2CStop();   //停止信号
}
//eeprom读操作
uint8_t eeprom_read(uint8_t addr)
{
	I2CStart();     //起始信号 
	I2CSendByte(0xa0);  //发送一个地址
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	I2CStop();	
	
	
	I2CStart();
	I2CSendByte(0xa1);
	I2CWaitAck();
	uint8_t dat = I2CReceiveByte();
	I2CSendNotAck();   
	I2CStop();	
	return dat;
}
//重定义函数
int fputc(int ch,FILE *f)
{
   HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,50);
	    return ch;
}
//led控制底层函数
void led_display(uint8_t led)
{
   HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
	|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,1);
	HAL_GPIO_WritePin(GPIOC,led<<8,0);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,0);
}
//按键扫描函数
uint8_t key_scan()
{
   static uint8_t key=1;
	HAL_Delay(10);
	if(key==1&&(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==0||HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==0||HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==0||HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==0))
	{
		
	  key=0;
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
//LCD显示函数
 void LCD_show()
 {
    switch(state)
		{
			case 0:
	      LCD_DisplayStringLine(Line2,(uint8_t *)"       L o c k      ");
				LCD_DisplayStringLine(Line4,(uint8_t *)"       Pass Word      ");
			  sprintf(buff,"        %c  %c  %c          ",one_site,two_site,three_site);
			  LCD_DisplayStringLine(Line6,(uint8_t *)buff); 
				break;
			case 1:
				LCD_DisplayStringLine(Line2,(uint8_t *)"        S e t        ");
				LCD_DisplayStringLine(Line4,(uint8_t *)"        Change      ");
			  sprintf(buff,"        %c  %c  %c          ",set_buf[0],set_buf[1],set_buf[2]);
			  LCD_DisplayStringLine(Line6,(uint8_t *)buff); 
				break;
			default:
				break;
		}
 }
 //按键处理函数
 void Key_pro(uint8_t Key)
 {
    switch(Key)
		{
			case 1:
				stand++;      //确认哪一位密码
				
	      if(stand==3)
				{  
				  if(state==0)
					{
					if(one_site==BUff[0]&&two_site==BUff[1]&&three_site==BUff[2])     //判断输入的密码是否正确
					{
					   state=1;
						 stand=0;
					}
					else         // 密码不正确
					{
					  one_site='*';
						two_site='*';
						three_site='*';
						stand=0;
					}
					}
					if(state==1)
					{   
						if(stand==3)
						{
					  BUff[0]=set_buf[0];
						BUff[1]=set_buf[1];
						BUff[2]=set_buf[2];
						eeprom_write(0X00,BUff[0]);
						eeprom_write(0X01,BUff[1]);
						eeprom_write(0X02,BUff[2]);
						state=0;
						stand=0;
						}
						}
             					
				}
				break;
			default:
				break;
		
		}
 }
 //LED处理函数
void LED_pro()
 {
    if(state==0)
		{
		  ucled|=1;
		}
		else
		{
		  ucled &=~1;
		}
		if(state==1)
		{
		  ucled|=2;
		}
		else 
		{
			ucled &=~2;
		}
		led_display(ucled);
 }
 void adc_pro()
 {
    HAL_ADC_Start(&hadc2);
    ucadc=HAL_ADC_GetValue(&hadc2);
	   sprintf(adcbuf,"      value:%.2f      ",(ucadc*3.3)/4096);
	   LCD_DisplayStringLine(Line8,(uint8_t *)adcbuf);
	 
 }
 //密码函数
 void value()
 {
	 //lock界面
 if(stand==0&&state==0)
	 {
	   if((ucadc*3.3)/4096<=1.5)
		 {
		   one_site='0';
		 }
     else if((ucadc*3.3)/4096<=2.5)
		 {
		   one_site='1';

		 }
     else 
		 {
		   one_site='2';
		 }			 
	 }
	 if(stand==1&&state==0)
	 {
	   if((ucadc*3.3)/4096<=1.5)
		 {
		   two_site='0';
		 }
     else if((ucadc*3.3)/4096<=2.5)
		 {
		   two_site='1';

		 }
     else 
		 {
		   two_site='2';
		 }			 
	 }
	 if(stand==2&&state==0)
	 {
	   if((ucadc*3.3)/4096<=1.5)
		 {
		   three_site='0';
		 }
     else if((ucadc*3.3)/4096<=2.5)
		 {
		   three_site='1';

		 }
     else 
		 {
		   three_site='2';
		 }
      		 
	 }
	 
	 //set界面
	 if(stand==0&&state==1)
	 {
	   if((ucadc*3.3)/4096<=1.5)
		 {
		   set_buf[0]='0';
		 }
     else if((ucadc*3.3)/4096<=2.5)
		 {
		   set_buf[0]='1';

		 }
     else 
		 {
		   set_buf[0]='2';
		 }			 
	 }
	 if(stand==1&&state==1)
	 {
	   if((ucadc*3.3)/4096<=1.5)
		 {
		   set_buf[1]='0';
		 }
     else if((ucadc*3.3)/4096<=2.5)
		 {
		    set_buf[1]='1';

		 }
     else 
		 {
		    set_buf[1]='2';
		 }			 
	 }
	 if(stand==2&&state==1)
	 {
	   if((ucadc*3.3)/4096<=1.5)
		 {
		    set_buf[2]='0';
		 }
     else if((ucadc*3.3)/4096<=2.5)
		 {
		    set_buf[2]='1';

		 }
     else 
		 {
		    set_buf[2]='2';
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
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
	I2CInit();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(Magenta);
	HAL_ADC_Start(&hadc2);
	HAL_TIM_Base_Start_IT(&htim7);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		key=key_scan();
		Key_pro(key);
		adc_pro();
		LCD_show();
		LED_pro();
		value();
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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM7)
		{
		  
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
