/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//POINTERS FOR ENABLING CLOCK
volatile uint32_t* RCC_AHB1ENR=(volatile uint32_t*)0X40023830;//ALSO USED FOR PORT A
volatile uint32_t* RCC_APB1ENR=(volatile uint32_t*)0X40023840;
//POINTERS FOR GPIOA
volatile uint32_t* GPIOA_MODER=(volatile uint32_t*)0X40020000;
volatile uint32_t* GPIOA_AFRL=(volatile uint32_t*)0X40020020;
volatile uint32_t* GPIOA_ODR=(volatile uint32_t*)0X40020014;
//POINTERS FOR USART
volatile uint32_t* USART_SR=(volatile uint32_t*)0X40004400;
volatile uint32_t* USART_DR=(volatile uint32_t*)0X40004404;
volatile uint32_t* USART_BRR=(volatile uint32_t*)0X40004408;
volatile uint32_t* USART_CR1=(volatile uint32_t*)0X4000440C;
volatile uint32_t* USART_CR3 = (volatile uint32_t*)0x40004414;
//POINTERS FOR DMA
volatile uint32_t* DMA_S5CR=(volatile uint32_t*)(0X40026000+0X10+(0X18*5));//STREAM5
volatile uint32_t* DMA_S6CR=(volatile uint32_t*)(0X40026000+0X10+(0X18*6));//STREAM6
volatile uint32_t* DMA_S5NDTR=(volatile uint32_t*)(0X40026000+0X14+(0X18*5));//STREAM5
volatile uint32_t* DMA_S6NDTR=(volatile uint32_t*)(0X40026000+0X14+(0X18*6));//STREAM6
volatile uint32_t* DMA_S5PAR=(volatile uint32_t*)(0X40026000+0X18+(0X18*5));//STREAM5
volatile uint32_t* DMA_S6PAR=(volatile uint32_t*)(0X40026000+0X18+(0X18*6));//STREAM6
volatile uint32_t* DMA_S5M0AR=(volatile uint32_t*)(0X40026000+0X1C+(0X18*5));//STREAM5
volatile uint32_t* DMA_S6M0AR=(volatile uint32_t*)(0X40026000+0X1C+(0X18*6));//STREAM6
volatile uint32_t* DMA_HISR=(volatile uint32_t*)(0x40026004);//polling
volatile uint32_t* DMA_HIFCR=(volatile uint32_t*)(0x4002600C);//polling

//defining buffer
const char arr[]="Hello From STM32 Hello World Hi Guys Hi Everyone\r\n";
volatile char buffer[5];
#define array_size(arr) (sizeof(arr)/sizeof(arr[0]))
uint8_t length=array_size(arr)-1;//sub 1 because extra null character is also counted
uint8_t size=array_size(buffer);
//INTERRUPT RELATED POINTERS
//DMA1_Stream5 INTERRUPT IS NO.16.ISER0
volatile uint32_t* NVIC_ISER0=(volatile uint32_t*)0XE000E100;
void DMA1_Stream5_IRQHandler();
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
  /* USER CODE BEGIN 2 */
  //CONFIGURING GPIOA
  *RCC_AHB1ENR|=(1U<<0);
  *GPIOA_MODER&=~(3U<<4);//PA2 MODER BITS CLEARED
  *GPIOA_MODER&=~(3U<<6);//PA3 MODER BITS CLEARED
  *GPIOA_MODER&=~(3U<<10);//PA5 MODER BITS CLEARED
  *GPIOA_MODER|=(2U<<4);//PA2 SET TO AF MODE
  *GPIOA_MODER|=(2U<<6);//PA3 SET TO AF MODE
  *GPIOA_MODER|=(1U<<10);//PA5 SET TO OUTPUT MODE
  *GPIOA_AFRL&=~(15U<<8);//PA2 AFRL CLEARED
  *GPIOA_AFRL&=~(15U<<12);//PA3 AFRL CLEARED
  *GPIOA_AFRL|=(7U<<8);//PA2 SET TO AF07(USART2 TX)
  *GPIOA_AFRL|=(7U<<12);//PA3 SET TO AF07(USART2 RX)
  //CONFIGURING USART2
  *RCC_APB1ENR|=(1U<<17);
  *USART_BRR=0X0683;
  *USART_CR1|=((1U<<13)|(1U<<2)|(1U<<3));//USART2 ENABLED,TX ENABLED,RX ENABLED
  *USART_CR3 |= (1U << 7) | (1U << 6);   // DMAT + DMAR — enable DMA requests for TX and RX
  //CONFIGURING DMA
  *RCC_AHB1ENR|=(1U<<21);//CLOCK ENABLE FOR DMA
  *DMA_S5CR&=~(7U<<25);//S5-RX;S6-TX
  *DMA_S5CR|=(4U<<25);//SELECTING CHANNEL(CHANNEL 4 USED FOR USART2)
  *DMA_S6CR|=(4U<<25);//SELECTING CHANNEL(CHANNEL 4 USED FOR USART2)
  *DMA_S5CR|=(1U<<8);//CIRCULAR MODE ENABLED ONLY NEEDED FOR RX BUFFER
  *DMA_S5CR&=~(3U<<6);//BITS 6 AND 7 ARE CLEARED-PERIPHERAL TO MEMORY
  *DMA_S6CR&=~(3U<<6);//BITS 6 AND 7 ARE CLEARED
  *DMA_S6CR|=(1U<<6);//MEMORY TO PERIPHERAL FOR TX
  *DMA_S5CR|=(1U<<10);//MEMORY INCREAMENT
  *DMA_S6CR|=(1U<<10);//MEMORY INCREAMENT
  *DMA_S5CR|=(1U<<4);//TC MODE ENABLED,INTERRUPT GENERATED WHEN RX BUFFER IS FULL
  *DMA_S5CR&=~(1U<<19);//INDICATING MEMORY 0 IN USE
  *DMA_S6NDTR=length;//INDICATING NO.OF ITEMS TO TRANSFER
  *DMA_S5NDTR=size;//INDICATING SIZE OF RX BUFFER
  *DMA_S6PAR=(uint32_t)USART_DR;//BASE ADDRESS REQUIRED FOR TX IS USART_DR
  *DMA_S6M0AR=(uint32_t)arr;//BASE ADDRESS FROM WHERE TX GETS DATA IS ARR
  *DMA_S5PAR=(uint32_t)USART_DR;//GETS DATA FROM PERIPHERAL
  *DMA_S5M0AR=(uint32_t)buffer;//SEND IT TO RX BUFFER
  //STREAM ENABLE MUST ALWAYS BE AT LAST
  *DMA_S5CR|=(1U<<0);//STREAM ENABLE
  *DMA_S6CR|=(1U<<0);//STREAM ENABLE
  *NVIC_ISER0|=(1U<<16);//ENABLING INTERRUPTS FROM DMA1_STREAM5





  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //if(((*DMA_HISR>>21)&0X01)){//BECOMES 1 WHEN ALL DATA ITEMS ARE TRANSFERRED

	  //}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void DMA1_Stream5_IRQHandler(void)
{
    if (*DMA_HISR & (1U << 11))   // TCIF5 — confirm this is really what triggered it
    {
        *GPIOA_ODR^=(1U<<5);

        *DMA_HIFCR |= (1U << 11);   // clear TCIF5 — required, or it refires immediately
    }
}
//IN THIS PROGRAM,CIRCULAR MODE IS ENABLED FOR STREAM 5 WHICH IS RX.AS SOON AS RX BUFFER IS FILLED
//TC INTERRUPT IS GENERATED WHICH IS WHY WE CHECK TCIF5
//ALSO WE USED NORMAL BEHAVIOUR FOR STREAM 6
//IF TC MODE IS ENABLED FOR STREAM 6,INTERRUPT WOULD BE GENERATED AFTER ALL DATA ITEMS ARE TRANSFERRED
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
#ifdef USE_FULL_ASSERT
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
