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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t state;  //系统状态
char buf1[20];   //R37数据缓冲区
char buf2[20];   //R38数据缓冲区
char ADC_buf[40];  //adc数据缓冲区
double ucadc1=0;      //ADC1采集到的数据
double ucadc2=0;      //ADC2采集到的数据
uint8_t stand;    //控制上限与下限标志位
double R37UP=2.2;    //R37上限标志位
double R37down=1.2;   //R37下限标志位
double R38UP=3.0;    //R38上限标志位
double R38down=1.4;   //R38下限标志位
uint8_t R37_stand;   //r37产品合格标志位
uint8_t R38_stand;   //r38产品合格标志位
uint8_t R37_instand;   //r37产品不合格标志位
uint8_t R38_instand;   //r38产品不合格标志位
uint8_t R37_;  //检测R37总的次数
uint8_t R38_;  //检测R38总的次数
uint8_t ucled;    //LED标志位
char RX_buf[20];   //串口数据缓冲区
uint8_t RX_multip;      //串口数据接收数量
uint8_t land;    //串口函数运行标志位
uint8_t timer;    //亮一秒标志位
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//重定义函数
int fputc(int ch,FILE *f)
{
   HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,10);
    return ch;
}
//LED底层控制函数
void led_display(uint8_t led)
{
   HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
	|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,GPIO_PIN_SET);
 	 HAL_GPIO_WritePin(GPIOC,led<<8,GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}
//按键扫描函数
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
//lcd显示函数
void lcd_show()
{
  switch(state)
		{
		  case 0:     //产品参数界面
			LCD_DisplayStringLine(Line1,(uint8_t *)"       GOODS          ");
			sprintf(buf1,"     R37:%.2f V          ",((double)ucadc2));
			LCD_DisplayStringLine(Line3,(uint8_t *)buf1);
      sprintf(buf2,"     R38:%.2f V         ",((double)ucadc1));
			LCD_DisplayStringLine(Line4,(uint8_t *)buf2);
				break;
			case 1:       //标准设置界面
			LCD_DisplayStringLine(Line1,(uint8_t *)"      STANDARD          ");
			sprintf(buf1,"    SR37:%.1f-%.1f            ",R37down,R37UP);
			LCD_DisplayStringLine(Line3,(uint8_t *)buf1);
			sprintf(buf2,"    SR38:%.1f-%.1f            ",R38down,R38UP);
			LCD_DisplayStringLine(Line4,(uint8_t *)buf2);
				break;
			case 2:
			LCD_DisplayStringLine(Line1,(uint8_t *)"        PASS          ");
			sprintf(buf1,"     PR37:%.1f%%          ",R37_stand*100.0/R37_);
			LCD_DisplayStringLine(Line3,(uint8_t *)buf1);
			sprintf(buf2,"     PR38:%.1f%%          ",R38_stand*100.0/R38_);
			LCD_DisplayStringLine(Line4,(uint8_t *)buf2);
				break;
			default:
				break;
    }
  }
//按键处理函数
void key_pro(uint8_t KEY)
{
    switch(KEY)
		{
		  case 1:      //按键B1在产品参数界面按下 
				state++;
			if(state>2)
			{
			state=0;
			}
				break;
			case 2:
				if(state==1)    //按键B2在标准设置界面按下的话
				{
				  stand++;
					if(stand>3)
					{
					stand=0;
					}
				}
	      if(state==0)     //按键B2在产品参数界面按下的话  判断R37
				{   R37_++;
				     if(ucadc2>R37down&&ucadc2<R37UP)
						 {
							 HAL_TIM_Base_Start_IT(&htim6);
						   R37_stand++;
						 }
						 else
						 {
						   R37_instand++;
						 }	 
				}
				break;
			case 3:
				if(state==1)      //按键B3在标准设置界面按下的话
				{
					switch(stand)
					{
					  case 0:
            R37UP+=0.2;
						   if(R37UP>3.1)
							 {
							   R37UP=2.2;
							 }
							break;
						case 1:
							R37down+=0.2;
						  if(R37down>2.1)
							{
							R37down=1.2;
							}
							break;
						case 2:
							R38UP+=0.2;
						   if(R38UP>3.1)
							 {
							   R38UP=2.2;
							 }
							break;
						case 3:
							R38down+=0.2;
						  if(R38down>2.1)
							{
							R38down=1.2;
							}
							break;
						default:
							break;
					}
				}
				if(state==0)     //按键B3在产品参数界面按下的话  判断R38
				{   R38_++;
				     if(ucadc1>R38down&&ucadc1<R38UP)
						 {
						   R38_stand++;
						 }
						 else
						 {
						   R38_instand++;
						 }	 
				}
				break;
			case 4:
				if(state==1)  //按键B4在标准设置界面按下的话 
				{
				switch(stand)
					{
					  case 0:
            R37UP-=0.2;
						if(R37UP<2.1)
						{
						 R37UP=3.0;
						}
							break;
						case 1:
							R37down-=0.2;
						   if(R37down<1.1)
							 {
							 R37down=2.0;
							 }
							break;
						case 2:
							R38UP-=0.2;
						if(R38UP<2.1)
						{
						 R38UP=3.0;
						}
							break;
						case 3:
							R38down-=0.2;
						   if(R38down<1.1)
							 {
							 R38down=2.0;
							 }
							break;
						default:
							break;
					}
				}
				if(state==2)    //按键B4在合格率界面按下的话
				{
					R37_=0;
					R37_instand=0;
					R37_stand=0;
					R38_=0;
					R38_instand=0;
					R38_stand=0;
					HAL_UART_Transmit(&huart1,(uint8_t *)RX_buf,3,50);
				}
				break;
			default:
				break;
		}
}
//adc处理函数
double getADC_value(ADC_HandleTypeDef *hadc)
{
  uint32_t adc;
	uint32_t adc_t=0;
	double adc_t1;
	HAL_ADC_Start(hadc);
	for(int i=0;i<=5;i++)
	{
	adc=HAL_ADC_GetValue(hadc);
	adc_t+=adc;
	}
	adc_t1=(double)adc_t/6.0;
	return adc_t1*3.3/4096;
}
//led处理函数
void led_pro()
{ 
	if(state==0)
	{
		ucled|=4;   //点亮LED3
	}
	else
	{
	ucled&=~4;    //熄灭LED3
	}
	if(state==1)
	{
		ucled|=8;   //点亮LED4
	}
	else
	{
	ucled&=~8;    //熄灭LED4
	}
	if(state==2)
	{
		ucled|=0x10;   //点亮LED5
	}
	else
	{
	ucled&=~0x10;    //熄灭LED5
	}
	if(timer==1&&R37_stand)
	{
	ucled|=1;
	}
	else
	{
//		HAL_TIM_Base_Stop(&htim6);
	  timer=0;
		ucled&=~1;
	}
   led_display(ucled); 
}
//串口数据函数
void uart_pro()
{
	  if(land==1)
			{
				land=0;
    if(RX_buf[2]=='7')
		{
			printf("R37:%d,%d,%.1f%%\r\n",R37_,R37_stand,R37_stand*100.0/R37_);
		}
		if(RX_buf[2]=='8')
		{
			printf("R38:%d,%d,%.1f%%\r\n",R38_,R38_stand,R38_stand*100.0/R38_);
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
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);
	HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED);    //ADC校准
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  uint8_t Key;
		Key=key_scan();
		key_pro(Key);
		lcd_show();
    ucadc1=getADC_value(&hadc1);
	  ucadc2=getADC_value(&hadc2);
		uart_pro();
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
//串口回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    { RX_buf[RX_multip]=(uint8_t)(huart1.Instance->RDR&(uint8_t)0X00ff);       
			RX_multip++;
			__HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_RXNE);
			if(RX_multip==3)
			{
			  RX_multip=0;
				land=1;
			}
    }
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance==TIM6)
	{
		timer++;
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
