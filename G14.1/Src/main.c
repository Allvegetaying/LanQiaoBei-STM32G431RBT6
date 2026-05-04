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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t Key_value; // 按键值
uint8_t view;  //系统的状态
uint8_t buf[10];
uint8_t cu[2]={'L','H'};
uint8_t xz;  //选择是高还是低（频）
uint8_t R=1,K=1;
uint8_t yue;  //在参考界面选择R还是K 高频（1）还是低频（0）
uint8_t N;//PWM切换的次数
uint8_t MH,ML;  //在低频和高频的最大速度
char buff[30];  
float ADC_value;   //ADC采集到的电压值
float zkb;  //占空比
char ucchar='%';   //百分号的字符
 float speed_value; //速度值
uint16_t stand=4000;    //频率值
uint8_t cnt;  //时间计数值
uint16_t cha[4];   //存储低频和高频的最大速度



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
//按键扫描函数
uint8_t Key_scean()
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

//按键读取函数
void key_read(uint8_t key_value)
{
  switch(key_value)
	{
		case 1:
			view++;
		
		if(view>2)
		{
		  view=0;
		}
			break;
		case 2:
			if(view==0)
			{
				stand=8000;  //频率值
			  xz++;
				
		if(xz>1)
		{
			stand=4000;   //频率值
			N++;      //切换高低频的次数
		  xz=0;
		} 
	   }
			if(view==1)
			{
			  yue++;
				if(yue>1)
				{
				yue=0;
			
				}
			}
			break;
		case 3:
			if(yue==0&&view==1)
			{
			   R++;
				if(R>10)
				{
				R=1;
				}
			}
			if(yue==1&&view==1)
			{
			  K++;
				if(K>10)
				{
				K=1;
				}
			}
			break;
		case 4:
			if(yue==0&&view==1)
			{
			   R--;
				if(R<1)
				{
				R=10;
				}
			}
			if(yue==1&&view==1)
			{
			  K--;
				if(K<1)
				{
				K=10;
				}
			}
			break;
		default:
			
			break;
	}
}

void lcd_pro()
{
	
	if(view==0)
	{
		LCD_DisplayStringLine(Line2, (uint8_t*) "         DATA");
		sprintf((char*)buf,"         M=%c  ",cu[xz]);
		LCD_DisplayStringLine(Line4, buf);
		sprintf((char*)buf,"         P=%.0f %c ",zkb,ucchar);
		LCD_DisplayStringLine(Line5, buf);
		speed_value=(2*3.14*R*stand)/10000;
		sprintf((char*)buf,"         V=%.1f    ",speed_value);
	  LCD_DisplayStringLine(Line6,buf);
	}
	if(view==1)
	{
		LCD_DisplayStringLine(Line2, (uint8_t*) "         PARA");
		sprintf((char*)buf,"         R=%d     ",R);
		LCD_DisplayStringLine(Line4, buf);
		sprintf((char*)buf,"         K=%d    ",K);
		LCD_DisplayStringLine(Line5, buf);

	  LCD_DisplayStringLine(Line6,(uint8_t*)"                    ");
		
	}
	if(view==2)
	{
		LCD_DisplayStringLine(Line2, (uint8_t*) "         RECD");
		sprintf((char*)buf,"         N=%d ",N); 
		LCD_DisplayStringLine(Line4, buf);
		sprintf((char*)buf,"         MH=%.1f    ",MH/10.0);
		LCD_DisplayStringLine(Line5, buf);
		sprintf((char*)buf,"         ML=%.1f    ",ML/10.0);
	  LCD_DisplayStringLine(Line6,buf);
	}
}

void ADC_pro()
{
 	HAL_ADC_Start(&hadc2);
	ADC_value=(HAL_ADC_GetValue(&hadc2)*3.3)/4096;
  sprintf(buff,"ADC_Value:%.3fV",ADC_value);
	LCD_DisplayStringLine(Line8,(uint8_t *)buff );
	if(ADC_value<=1)
	{
	zkb=10;
	}
	else if(ADC_value>=3)
	 {
	 zkb=85;
	 }
	 else if(ADC_value>1||ADC_value<3)
   {
	 zkb=(375*ADC_value-275)/10.0;    //通过表格得到的计算公式
	 }
	HAL_Delay(50);
}

void LED_Disp(uint8_t ucled)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|
										GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,GPIO_PIN_SET);//灭灯
	HAL_GPIO_WritePin(GPIOC,ucled<<8,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	
}
uint8_t led;
void led_pro(void)
{
   if(view==0)
	 {
	   led|=1;   //点亮LED1
	 }
   else
	 {
	   led&=~1;  //熄灭LED1
	 
	 }
	 LED_Disp(led);
	 
	
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
  MX_ADC2_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_IC_Start(&htim2,TIM_CHANNEL_1);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		Key_value=Key_scean();
		key_read(Key_value);
		ADC_pro();
		lcd_pro();
		led_pro();
		
		
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
   if(htim->Instance==TIM4)
	 {
//	   HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_All);
		 cnt++;
		 cha[0]= __HAL_TIM_GET_COUNTER(&htim2);             //捕获脉宽
		 __HAL_TIM_SetCounter(&htim2,0);
		 cha[2]=628000*R/K/cha[0];
		 if(cnt>2)
		 {
			 cnt=0;
		  if(yue==0)
			{
				if(cha[2]>ML)
				{
				ML=cha[2];
				
				}
			}
			else
			{
			if(cha[2]>MH)
				{
				MH=cha[2];
				
				}
			}
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
