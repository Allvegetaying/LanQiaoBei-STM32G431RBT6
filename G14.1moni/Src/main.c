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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t KEY;     //按键的键值
uint8_t state;   //界面状态
char buf[400];    //数据缓冲区
//uint8_t tuf[1]="%";    //存储%的数组
uint16_t ZKB1=10;   //PA1占空比
uint16_t ZKB2=10;   //PA7占空比
uint32_t psc1=1000;       //PA1频率
uint32_t psc2=1000;       //PA7频率
uint8_t  uart_key_stand;   //串口选择还是按键选择标志位
uint8_t RX_buff[1];    //串口数据缓冲区
uint8_t stand;      //串口数据接收数量
uint8_t mand;  //串口数据执行函数标志位
uint8_t ucled;   //LED灯标志位



/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//重定义函数
int fputc(int ch,FILE *f)
{
   HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,50);
    return ch;	
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

//LED处理函数
void led_display(uint8_t led)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|
	GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,1);
	HAL_GPIO_WritePin(GPIOC,led<<8,0);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,0);
}
//按键扫描函数
uint8_t Key_scan()
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
//lcd显示函数
void lcd_show()
{
  switch(state)
	{
	  case 0:
			LCD_DisplayStringLine(Line2,(uint8_t *)"       PA1        ");
	      	sprintf(buf,"       F:%d HZ     ",psc1);
		  LCD_DisplayStringLine(Line4,(uint8_t *)buf);
		      sprintf(buf,"       D:%d%%         ",ZKB1);
		  LCD_DisplayStringLine(Line6,(uint8_t *)buf);
			    break;
		case 1:
			
		LCD_DisplayStringLine(Line2,(uint8_t *)"       PA7        ");
		sprintf(buf,"       F:%d HZ     ",psc2);
		LCD_DisplayStringLine(Line4,(uint8_t *)buf);
		sprintf(buf,"       D:%d%%         		",ZKB2);
		LCD_DisplayStringLine(Line6,(uint8_t *)buf);
		
			break;
		default:
			break;
	}
}
//占空比 频率设置函数
//  ZKB:占空比    f：频率
void PWM_Set(TIM_HandleTypeDef htim,uint32_t channel,uint32_t ZKB,uint32_t f)
{
  
	uint64_t sys_clk=80000000;  //系统时钟
	uint32_t psc=80-1;          //预分频
	float arr=(sys_clk*1.0)/f/(psc+1)-1;
	float pulse=ZKB*1.0/100*(arr+1);
	
	  __HAL_TIM_SetAutoreload(&htim,arr);            //设置自动重载值
	  __HAL_TIM_SetCompare(&htim,channel,pulse);        //设置占空比值
   __HAL_TIM_SET_PRESCALER(&htim,psc);          //设置预分频值
}
//按键处理函数
void key_process(uint8_t Key)
{
     switch(Key)
		 {
			 case 1:        //按键B1按下的话控制频率
			 switch(state)
			 {
			   case 0:
//					 printf("%d\r\n",psc1);
					 psc1+=1000;
				    if(psc1>10000)
						{
						psc1=1000;
						}	
//						
					 PWM_Set(htim2,TIM_CHANNEL_2,ZKB1,psc1);
					
					 break;
				 case 1:
					  psc2+=1000;
				    if(psc2>10000)
						{
						psc2=1000;
						}
						printf("%d\r\n",psc2);
					 PWM_Set(htim3,TIM_CHANNEL_2,ZKB2,psc2);
					 
				 
					 break;
				 default:
					 break;
			 }
				 break;
			 case 2:    //按键B2按下的话控制占空比
				 switch(state)
				 {
					 case 0:
						 ZKB1=ZKB1+10;
							 if(ZKB1>90)
							 {
							 ZKB1=10;
							 }
			      PWM_Set(htim2,TIM_CHANNEL_2,ZKB1,psc1);
			       break;
					 case 1:
						  ZKB2=ZKB2+10;
							 if(ZKB2>90)
							 {
							 ZKB2=10;
							 }
						 PWM_Set(htim3,TIM_CHANNEL_2,ZKB2,psc2);
						 break;
					 default:
						 break;
		      }
				 break;
			 case 3:				 //按键3按下切换界面状态
				 if(uart_key_stand==0)    
				 {
			  state++;
			 if(state>1)
			 {
			 state=0;
			 }
		 }
				 break;
			 case 4:
			HAL_UART_Transmit(&huart1,RX_buff,1,10); 
//			 printf("%d",uart_key_stand);
				 uart_key_stand++;
			 if(uart_key_stand>1)
			 {
			 uart_key_stand=0;
			 }
				 break;
			 default:
				 break; 
		 }

}
//串口数据处理函数
void uart_pro()
{
	if(uart_key_stand==1)
	{
  if(RX_buff[0]=='@')
	{
	  state=0;
	}
	else if(RX_buff[0]=='#')
	{
	 state=1;
	
	}
	else
	{
	  printf("ERROR\r\n");
	}
}
}
void led_pro()
{
   if(state==0)
	 {
	   ucled |=1;    //点亮LED1
	 }
	 else
	 {
	 ucled &=~1;   //熄灭LED1
	 }
	 if(state==1)
	 {
	 ucled |=2;        //点亮LED2
	 }
	 else
   {
	 ucled &=~2;    //熄灭LED2
	 }
	 if(uart_key_stand==0)
	 {
	 ucled |=4;     //点亮LED3
	 }
	 else
	 {
	 ucled &=~4;    //熄灭LED3
	 }
	 if(psc1>psc2)
	 {
	   ucled^=1;       //翻转LED1
	 }
	 if(psc1<psc2)
	 {
	   ucled^=2;   //翻转LED2
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	
	LCD_Init();
	LCD_Clear(White);
	LCD_SetBackColor(White);
	LCD_SetTextColor(Black);
	
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		KEY=Key_scan();
		lcd_show();
		key_process(KEY);
		if(mand==1)
		{
		uart_pro();
		mand=0;
		}
		led_pro();
//	  printf("111111111\r\n");
		
		

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
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance==USART1)
	{
//		uint8_t LED;
//		LED^=1;
		RX_buff[stand]=(uint8_t)(huart1.Instance->RDR&(uint8_t)0x00ff);
		stand++;
	  __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_RXNE);
//		led_display(LED);
//		HAL_UART_Transmit(&huart1,RX_buff,1,10);
		if(stand>=1)
		{
			mand=1;
//		memset(RX_BUFF,0,20);
			stand=0;
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
