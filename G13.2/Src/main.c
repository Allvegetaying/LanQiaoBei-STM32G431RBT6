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
#include "lcd.h"
#include "stdio.h"
#include "EEPROM.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 uint8_t NUM1=0;
 uint8_t buf[20];
 uint8_t cnt;    //商品数量X
 uint8_t preice=10;  //商品价格X
 uint8_t inventory=10;  //商品的库存X
 uint8_t cnt1;    //商品数量Y
 uint8_t preice1=10;  //商品价格Y
 uint8_t inventory1=10;  //商品的库存Y
 uint8_t RX_BUFF[20];
 uint8_t RX_stand;
 uint8_t stand=6;  //时间计数单位
 uint8_t led;
 uint8_t key_value;//按键值

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//重定义函数
int fputc(int ch,FILE *f)
{
    HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,50);
	   return ch;
}
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
//按键过程函数
void KEY_pro(uint8_t KEY_VALUE)
{

   switch(KEY_VALUE)
	 {
		    case 1:
					NUM1++;
				   if(NUM1>2)
					 {
					  NUM1=0;
					 }
					
		        break;
		 		 case 2:
					 if(NUM1==0)
					 {
						 cnt++;    //商品数量
						 if(cnt>inventory)
						 {
						 cnt=0;
						 }
					 }
					 if(NUM1==1)
					 {
						 preice++;
						 if(preice>=21)
						 {
						 
						 preice=10;
							e2prom_write(preice,2);  //x的价格
						 
						 }
						 
					 
					 }
					 if(NUM1==2)
					 {
					  inventory=inventory+1;    //库存增加
						 e2prom_write(inventory,0);   //X的库存
					 
					 }
				 
		         break;
				 case 3:
					 if(NUM1==0)
					 {
						 cnt1++;    //商品数量
						 if(cnt1>inventory1)
						 {
						 cnt1=0;
						 }
					 }
					 if(NUM1==1)
					 {
						 preice1++;   
						 if(preice1>=21)
						 {
						 
						 preice1=10;
						e2prom_write(inventory1,1);   //y的价格
						 
						 }
						 
					 }
					 if(NUM1==2)
					 {
					  inventory1++;    //库存增加
						e2prom_write(inventory1,3);   //y的库存
					 
					 }
					 
		         break;
				 case 4:
				      if(NUM1==0)
							{
							
							inventory1=inventory1-cnt1;  //y的库存
							inventory=inventory-cnt;     //x的库存
						   
								printf("X:%d,y:%d,z:%.1f",cnt,cnt1,(cnt*preice+cnt1*preice1)/10.0);
							cnt=0;
							cnt1=0;
							stand=0;
							}
		        break;
				 
				 default:
					 
					 break;
	 }
		
}

void lcd_pro()
{
   if(NUM1==0)
	 {
		 LCD_DisplayStringLine(Line2,(uint8_t *)"         SHOP ");
		 
		 sprintf((char *)buf,"         x:%d ",cnt);
		 LCD_DisplayStringLine(Line4,buf);
		 
		 sprintf((char *)buf,"         y:%d ",cnt1);
		 LCD_DisplayStringLine(Line5,buf);
		 
	 }
	 if(NUM1==1)
	 {
		  LCD_DisplayStringLine(Line2,(uint8_t *)"         PRICE");
		 
		 sprintf((char *)buf,"         x:%2.1f   ",preice/10.0);
		 	LCD_DisplayStringLine(Line4,buf);
		 
		  sprintf((char *)buf,"         y:%2.1f   ",preice1/10.0);
		  LCD_DisplayStringLine(Line5,buf);
	 
	 }
	 if(NUM1==2)
	 {
		  LCD_DisplayStringLine(Line2,(uint8_t *)"         REP  ");
		 
		  sprintf((char *)buf,"         x:%d   ",inventory);
		  LCD_DisplayStringLine(Line4,buf);
		 
		 sprintf((char *)buf,"         y:%d   ",inventory1);
		  LCD_DisplayStringLine(Line5,buf);
	 
	 }

}

void LED_Disp(uint8_t ucled)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|
										GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,GPIO_PIN_SET);//灭灯
	HAL_GPIO_WritePin(GPIOC,ucled<<8,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	
}

void led_pro(void)
{
	if(stand<5)
	{
		
		
	 led|=1;  //点亮led1
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,150);
	
	}
	else{
	 led &=~1;   //熄灭led1
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,25);
	
	
	}
   if(inventory==0&&inventory1==0)
	 {
	 led^=2;    //LED2闪烁
	 }
	 else 
	 {
	  led&=~2;   // led2熄灭
	 
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
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
   HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);
	 LCD_Init();
	 LCD_Clear(Black);
	 LCD_SetBackColor(Black);
	 LCD_SetTextColor(White);
	 I2CInit();
	 HAL_TIM_Base_Start_IT(&htim4);
	 __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
	 HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	 
	 
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
//		|GPIO_PIN_14|GPIO_PIN_15);
		
		key_value=Key_scean();
		KEY_pro(key_value);
		lcd_pro();
		led_pro();
		HAL_Delay(100);
		
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

 if(huart->Instance == USART1)
	__HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_RXNE);
    { 
			RX_BUFF[RX_stand]=(uint8_t)(huart1.Instance->RDR&(uint8_t)0X00ff);
        
			RX_stand++;
			
			if(RX_BUFF[0]=='?')
			{
				
				printf("X:%2.1f,Y:%2.1f\r\n",preice/10.0,preice1/10.0);
				RX_stand=0;
			
			}
				
		} 
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    
   if(htim->Instance==TIM4)
	 {
//		 HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_All);
		 stand++;
//		 if(stand>=5)
//		 {
//		 stand=0;
//		 
//	   }
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
