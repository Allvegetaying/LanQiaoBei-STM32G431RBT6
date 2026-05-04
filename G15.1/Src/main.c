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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

uint8_t key_value;    //按键值
uint8_t state;   //数据界面状态
uint8_t mode;     //界面模式
char buff[60];   //缓存区
uint8_t A0;   //界面1模式1
uint8_t B0;    //界面1模式2
uint8_t A1;   //界面2模式1
uint8_t B1;    //界面2模式2
uint8_t stand;   // 在参数界面切换参数
uint16_t PD;     //参数界面参数1
uint32_t PH;     //参数界面参数2
int PX;     //参数界面参数3
uint32_t f40=0;
uint32_t f39=0;


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
//按键扫描
uint8_t KEY_scan()
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
//按键读取函数并deal
void KEY_read(uint8_t KEY_VALUE)
{ 
	switch(KEY_VALUE)
	{
		case 1:
			if(state==1)//在参数界面
			{
				if(stand==0)
				{
				  PD=PD+100;
					if(PD<=100)
					{
					PD=100;
					}
				  if(PD>=1000)
					{
					  PD=1000;
					}
				}
				if(stand==1)
				{
				PH=PH+100;
					if(PH<=1000)
					{
						PH=1000;
					}
					if(PH>=100000)
					{
					PH=100000;
					}
				}
				if(stand==2)
				{
				PX=PX+100;
					if(PX<=-1000)
					{
					 PX=-1000;
					
					}
					if(PX>=1000)
					{
					PX=1000;
					}	
				}
			}	
			break;
		case 2:
			
		if(state==1)//在参数界面
			{
				if(stand==0)
				{
				  PD=PD-100;
					if(PD<=100)
					{
					PD=100;
					}
				  if(PD>=1000)
					{
					  PD=1000;
					}
				}
				if(stand==1)
				{
				PH=PH-100;
					if(PH<=1000)
					{
						PH=1000;
					}
					if(PH>=100000)
					{
						
					PH=100000;
					
					}
				}
				if(stand==2)
				{
				PX=PX-100;
					if(PX<=-1000)
					{
					 PX=-1000;
					
					}
					if(PX>=1000)
					{
					PX=1000;
					}	
				}
			}	
			break;
		case 3:
			if(state==0)   //在数据界面
			{
			mode++;
		   if(mode>1)
			 {
			 mode=0;
			 }
		  } 
			if(state==1)    //在参数界面
			{
				stand++;
				if(stand>2)
				{
				stand=0;
				}
			}
			if(state==2)    //在记录界面按下按键3时
			{
				
				
				
				
				
			
				
				
				
				
				
			
			
			}
			break;
		case 4:
			state++;
		  if(state>2)
			{
			state=0;
			}
		
			break;	
			
	}
}
uint8_t ucled;
void led_pro()
{
  if(state==0)
	{
	ucled|=1;   //点亮led1
	
	}
	else
	{
	  ucled&=~1;    //熄灭led1
	}
}
void LED_disp(uint8_t ulcd)
{
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
	|GPIO_PIN_14|GPIO_PIN_15,1);
	HAL_GPIO_WritePin(GPIOC,ulcd<<8,0);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,0);

}
void lcd_display()
{
	
   switch(state)
	 {
	 
		 case 0:
			 switch(mode)
			 {
			 
				 case 0:
					 
					 LCD_DisplayStringLine(Line2,(uint8_t*)"       DATA(1) ");
				     sprintf(buff,"       A=%d HZ      ",f39);
				   LCD_DisplayStringLine(Line4,(uint8_t *)buff);
				    sprintf(buff,"       B=%d HZ        ",f40);
				   LCD_DisplayStringLine(Line5,(uint8_t *)buff);
				   LCD_DisplayStringLine(Line6,(uint8_t *)"               ");
				   LCD_DisplayStringLine(Line7,(uint8_t *)"               ");
				 
					 break;
				 case 1:
					 
					 LCD_DisplayStringLine(Line2,(uint8_t*)"       DATA(2) ");
				 		sprintf(buff,"       A=%f MS       ",1000.0/f39);
				   LCD_DisplayStringLine(Line4,(uint8_t *)buff);
				    sprintf(buff,"       B=%f MS       ",1000.0/f40);
				   LCD_DisplayStringLine(Line5,(uint8_t *)buff);
					 LCD_DisplayStringLine(Line6,(uint8_t *)"               ");
					 LCD_DisplayStringLine(Line7,(uint8_t *)"               ");
				   
					 break;
			 }
			 break;
		 case 1:
			 
		       LCD_DisplayStringLine(Line2,(uint8_t*)"       PARA   ");
				 		sprintf(buff,"       PR=%d HZ      ",PD);
				   LCD_DisplayStringLine(Line4,(uint8_t *)buff);
				    sprintf(buff,"       PH=%d HZ      ",PH);
				   LCD_DisplayStringLine(Line5,(uint8_t *)buff);
		        sprintf(buff,"       PX=%d HZ      ",PX);
				   LCD_DisplayStringLine(Line6,(uint8_t *)buff);
		 
			 break;
		 case 2:
			 
			     LCD_DisplayStringLine(Line2,(uint8_t*)"       RECD   ");
				 		sprintf(buff,"       NDA=%d HZ ",A1);
				   LCD_DisplayStringLine(Line4,(uint8_t *)buff);
				    sprintf(buff,"       NDB=%d HZ ",B1);
				   LCD_DisplayStringLine(Line5,(uint8_t *)buff);
		        sprintf(buff,"       NHA=%d HZ ",B1);
				   LCD_DisplayStringLine(Line6,(uint8_t *)buff);
		        sprintf(buff,"       NHB=%d HZ ",B1);
				   LCD_DisplayStringLine(Line7,(uint8_t *)buff);
		 
			 break;
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	
	HAL_TIM_Base_Start_IT(&htim6);   //启动定时器6


  HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);    //开启定时器输入捕获中断


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)  
  {
		
		key_value=KEY_scan();
		KEY_read(key_value);
		lcd_display();
	
		 
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
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV5;
  RCC_OscInitStruct.PLL.PLLN = 32;
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

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	uint32_t  cc1_value = 0;
	cc1_value = __HAL_TIM_GET_COUNTER(htim);
	__HAL_TIM_SetCounter(htim,0);
	
	if(htim == &htim2) //定时器2，对应的是PA15引脚，R40
	{
		f40 = 1000000/cc1_value;
	}
	
	if(htim == &htim3) //定时器3，对应的是PB4引脚，R39
	{
		f39 = 1000000/cc1_value;
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
   if(htim->Instance==TIM6)
	 {
	   HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_All);
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
