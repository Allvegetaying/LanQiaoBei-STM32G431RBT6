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

uint8_t led;
uint8_t state;      //系统状态
char buf[20];      //缓冲区
uint8_t cnbr=2;     //  停车类型1
uint8_t vnbr=4;     //  停车类型2
uint8_t idle=2;    //空闲车位
float CNBR=3.50;     //停车费用1
float VNBR=2.00;    //停车费用2
uint8_t zkb;    //按键4标志位
uint8_t ucled;   //LED灯标志位
uint8_t RX_BUFF[240] ={0}; //串口数据接收缓冲区
uint8_t stand=0;   //串口数据接收数量
uint8_t I_shi,I_fen;   //小汽车进入的时间
uint8_t O_shi,O_fen;    //小汽车出去的时间
uint8_t shi,fen;      //小汽车停车的时间
float free1;   //CNBR费用
float free2;    //VNBR费用
uint8_t uart;  //串口函数执行标志位


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

void led_display(uint8_t led)
{
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,1);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|
	GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,1);
	HAL_GPIO_WritePin(GPIOC,led<<8,0);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,0);
}

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

void key_process(uint8_t key)
{
    switch (key)
		{
			case 1:     //按键B1按下的话
				state++;
			if(state>1)
			{
			state=0;
			}
				break;
			case 2:     //按键B2按下的话
				if(state==1)
				{
				 CNBR=CNBR+0.5;
				 VNBR=VNBR+0.5;
				}
				break;
			case 3:     //按键B3按下的话
				if(state==1)
				{
				 CNBR=CNBR-0.5;
				 VNBR=VNBR-0.5;
				}
				break;
			case 4:      //按键B4按下的话
			zkb++;
			if(zkb>1)
			{
			zkb=0;
			}
			switch (zkb)
			{
				case 0:
					__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,0);//占空比为0
					break;
				case 1:
					
				__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,100);  //占空比为20%
					break;
				default:
					break;
			}
			
				break;
			default:
				break;
		
		}
}

void lcd_show()
{
  switch (state)
	{
	
	  case 0:
			LCD_SetBackColor(Red);         //设置行的高亮
			LCD_DisplayStringLine(Line2, (uint8_t*)"       DATA            ");
		  sprintf(buf,"      CNBR:   %d    ",cnbr);
		  LCD_DisplayStringLine(Line4,(uint8_t*)buf);		
	  	sprintf(buf,"      VNBR:   %d    ",vnbr);
		  LCD_DisplayStringLine(Line6,(uint8_t*)buf);
    		sprintf(buf,"      IDLE:   %d    ",idle);
		  LCD_DisplayStringLine(Line8,(uint8_t*)buf);
		   LCD_SetBackColor(White);
		
			break;
		case 1:
			
			LCD_DisplayStringLine(Line2, (uint8_t*)"       PARA            ");
		  sprintf(buf,"      CNBR:   %.2f  ",CNBR);
		  LCD_DisplayStringLine(Line4,(uint8_t*)buf);		
	  	sprintf(buf,"      VNBR:   %.2f  ",VNBR);
		  LCD_DisplayStringLine(Line6,(uint8_t*)buf);
		 LCD_DisplayStringLine(Line8,(uint8_t*)"                         ");
			break;
	
		default:
			break;
	}
}

void led_process()
{

   if(zkb==1)
	 {
	    ucled|=2;  //亮起LED2
	 }
	 if(zkb==0)
	 {
	   ucled&=~2;     //熄灭led2
	 }
    
	 if(idle>0)
	 {
		 ucled|=1;  //亮起LED1
	 }
	 else
	 {
	   ucled&=1;     //熄灭led1
	 }
	 led_display(ucled);
}

//串口接收数据处理
void uart_process()
{
	if(RX_BUFF[0]==RX_BUFF[22]&&RX_BUFF[1]==RX_BUFF[23]&&RX_BUFF[2]==RX_BUFF[24]&&RX_BUFF[3]==RX_BUFF[25]&&RX_BUFF[5]==RX_BUFF[27]&&RX_BUFF[6]==RX_BUFF[28]&&RX_BUFF[7]==RX_BUFF[29]&&RX_BUFF[8]==RX_BUFF[30])
	{
  if(RX_BUFF[0]=='C'&&RX_BUFF[22]=='C')
	{
//	   led_display(0xff); 
		 I_shi=(RX_BUFF[16]-48)*10+(RX_BUFF[17]-48);
		 I_fen=(RX_BUFF[18]-48)*10+ (RX_BUFF[19]-48);
		
		 O_shi=(RX_BUFF[38]-48)*10+RX_BUFF[39]-48;
		 O_fen=(RX_BUFF[40]-48)*10+RX_BUFF[41]-48;
		
		shi=O_shi-I_shi;
		fen=O_fen-I_fen;
		
		if(fen<60)
		{
		  shi++;
		}
		free1=shi*CNBR;
		
		printf("CNBR:%c%c%c%c:%d:%.2f\r\n",RX_BUFF[5],RX_BUFF[6],RX_BUFF[7],RX_BUFF[8],shi,free1);  	
	}
		if(RX_BUFF[0]=='V'&&RX_BUFF[22]=='V')
	{
	   led_display(0xff); 
		 I_shi=(RX_BUFF[16]-48)*10+(RX_BUFF[17]-48);
		 I_fen=(RX_BUFF[18]-48)*10+ (RX_BUFF[19]-48);
		
		 O_shi=(RX_BUFF[38]-48)*10+RX_BUFF[39]-48;
		 O_fen=(RX_BUFF[40]-48)*10+RX_BUFF[41]-48;
		
		shi=O_shi-I_shi;
		fen=O_fen-I_fen;
		
		if(fen<60)
		{
		 if(fen>0)
		 {
		 shi++;         //小汽车的停车时间
		 }
		}
		free2=shi*VNBR;
		
		printf("VNBR:%c%c%c%c:%d:%.2f\r\n",RX_BUFF[5],RX_BUFF[6],RX_BUFF[7],RX_BUFF[8],shi,free2);  	
	}
}
	else
 {
	printf("Error\r\n");
	}
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
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
	LCD_Clear(White);
	LCD_SetBackColor(White);
	LCD_SetTextColor(Black);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		    uint8_t Key;
//		  led&=1;   //熄灭led
//		  led|=1;    //亮起led
//			led|=~1;     //亮起除led的所以led
	   	  led^=1;
//		led_display(0xff);
//		led_display(led);
		Key=KEY_scan();
		key_process(Key);
//		if(Key==1)
//		{
//		  HAL_UART_Transmit(&huart1,RX_BUFF,44,10);
//		}
		lcd_show();
		led_process();
		if(uart==1)
		{	
		uart_process();
			uart=0;
		}

//		printf("11111");
		
//		switch (Key)
//		{
//			case  1:
//				led_display(0xf0);
//			break;
//			case 2:
//				led_display(0x01);
//			break;
//			case 3:
//				led_display(0xff);
//			break;
//			case 4:
//				led_display(0x08);
//			default:
//				break;
//		}
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
		RX_BUFF[stand]=(uint8_t)(huart1.Instance->RDR&(uint8_t)0x00ff);
		stand++;
	__HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_RXNE);
		
		if(stand==44)
		{
//		memset(RX_BUFF,0,20);
			stand=0;
			uart=1;
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
