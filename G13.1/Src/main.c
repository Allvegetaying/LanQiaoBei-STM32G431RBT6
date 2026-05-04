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
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
  uint8_t stand=0;
	char uspsw[3]={'1','2','3'};    //当前密码值
	char uspsd[3]={'@','@','@'};    // 设置密码值
	char num[20];
	char i='0',j='0',k='0';
	uint8_t key;
	uint8_t view;
	uint8_t stand;  //用于计时
	uint8_t RX_BUFF[20];
uint8_t RX_stand;
uint8_t stc;
uint8_t RX_start;
	uint8_t led;
	int led_time;
	uint8_t Num;   //计算输入错误的次数的	
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
	uint8_t  uclcd[21];    //缓冲区
	
void  LCD_diaplay()  //LCD显示任务
{
	if(view==0)
	{

//	  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);    //打开锁存器，防止数据干扰
    LCD_DisplayStringLine(Line2,(uint8_t*)"        PSD");
	
		sprintf((char*)uclcd,"    B 1: %c %c         ",uspsd[0],uspsw[0]);
		LCD_DisplayStringLine(Line4,(uint8_t*)uclcd);
		
		sprintf((char*)uclcd,"    B 2: %c %C       ",uspsd[1],uspsw[1]);
		LCD_DisplayStringLine(Line5,(uint8_t*)uclcd);
		
		sprintf((char*)uclcd,"    B 3: %c %c        ",uspsd[2],uspsw[2]);
		LCD_DisplayStringLine(Line6,(uint8_t*)uclcd);
	}
	else if(view==1)
	{
		
//    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);    //打开锁存器，防止数据干扰
	  LCD_DisplayStringLine(Line2,(uint8_t*)"        STA");
		
		LCD_DisplayStringLine(Line4,(uint8_t*)"      F : 2000HZ  ");
	   
		LCD_DisplayStringLine(Line5,(uint8_t*)"      D : 10% ");
		
		LCD_DisplayStringLine(Line6,(uint8_t*)"                       ");
	}
	
}
void KEY_read( uint8_t KEY_VALUE)
{
	switch (KEY_VALUE){
	
		case 1:
			KEY_VALUE--;
		if(uspsd[KEY_VALUE]=='@')
		{
		  uspsd[KEY_VALUE]='0';
		}
		else 
		{
		uspsd[KEY_VALUE]++;
			if(uspsd[KEY_VALUE]==':')
			{
			uspsd[KEY_VALUE]='0';
			}
		}
			break;
		
	  case 2:
			KEY_VALUE--;
		if(uspsd[KEY_VALUE]=='@')
		{
		  uspsd[KEY_VALUE]='0';
		}
		else 
		{
		uspsd[KEY_VALUE]++;
			if(uspsd[KEY_VALUE]==':')
			{
			uspsd[KEY_VALUE]='0';
			}
		}
		      break;
 	  case 3:
			KEY_VALUE--;
		if(uspsd[KEY_VALUE]=='@')
		{
		  uspsd[KEY_VALUE]='0';
		}
		else 
		{
		uspsd[KEY_VALUE]++;
			if(uspsd[KEY_VALUE]==':')
			{
			uspsd[KEY_VALUE]='0';
			}
		}
		       break;
	  case 4:
			if(uspsd[0]==uspsw[0]&&uspsd[1]==uspsw[1]&&uspsd[2]==uspsw[2])
			{
			  view=1;
				__HAL_TIM_SetAutoreload(&htim2,999);
				__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,100);
				Num=0;
			}
			else 
			{
			uspsd[0]=uspsd[1]=uspsd[2]='@';
			Num++;
			
			}
			stand=0;
		        break;
	       
	}
		
	
//  if(KEY_VALUE==1)
//	{
//		
//		if(i==cha)
//		{
//			i='0';
//			
//		}
//		else
//		i=i+1;
//		if(i>'9')
//		{
//		  i='0';
//		}
//	
//	}
//	
//	
//	if(KEY_VALUE==2)
//	{
//		j=j+1;
//		if(j>'9')
//		{
//		  j='0';
//		}
//		
//	}
//	if(KEY_VALUE==3)
//	{
//		
//	  k=k+1;
//		if(k>'9')
//		{
//		  k='0';
//		}	
//		
//		
//	}
//	if(KEY_VALUE==4)
//	{
//	
//	  view++;
//		if(view>1)
//		{
//		  view=0;
//		}
//	
//	
//	
//	}
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
    if(view==1)
        led |= 1;
    else
        led &= ~1;
    if((Num>=3)&&(stand<5))
		{
		led^=2;  //异或处理
		}
		else
		{
		led&=~2;
		}
    LED_Disp(led);
}

 //按键扫描函数
uint8_t KEY_scean()
{
   static  uint8_t KEY=1;
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
  MX_TIM4_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
//  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,0);    //打开锁存器，防止数据干扰
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		key=KEY_scean();
		KEY_read(key);
		LCD_diaplay();
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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    
   if(htim->Instance==TIM4)
	 {
		 stand++;
		 if(stand>=5&&view==1)
		 {
		 
		   view=0;
		 __HAL_TIM_SetAutoreload(&htim2,1999);
		 __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,100);
		  uspsd[0]=uspsd[1]=uspsd[2]='@';
		 
		 }
	 }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
    if(huart->Instance == USART1)
    { RX_BUFF[RX_stand]=(uint8_t)(huart1.Instance->RDR&(uint8_t)0X00ff);
       
			RX_stand++;
			__HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_RXNE);

//			HAL_UART_Transmit(&huart1,RX_BUFF,1,10);
			
			if(RX_stand==7)
			{
			RX_stand=0;
			if(uspsw[0]==RX_BUFF[0]&&uspsw[1]==RX_BUFF[1]&&uspsw[2]==RX_BUFF[2])
			{
			HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_All);
			uspsw[0]=RX_BUFF[4];
			uspsw[1]=RX_BUFF[5];
			uspsw[2]=RX_BUFF[6];
			memset(RX_BUFF,0,7);
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
