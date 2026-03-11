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
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* I2C3 base address */
/* RCC */
#define RCC_AHB1ENR (*(volatile uint32_t*)0x40023830)
#define RCC_APB1ENR (*(volatile uint32_t*)0x40023840)

/* GPIOA */
#define GPIOA_MODER   (*(volatile uint32_t*)0x40020000)
#define GPIOA_OTYPER  (*(volatile uint32_t*)0x40020004)
#define GPIOA_OSPEEDR (*(volatile uint32_t*)0x40020008)
#define GPIOA_PUPDR   (*(volatile uint32_t*)0x4002000C)
#define GPIOA_AFRH    (*(volatile uint32_t*)0x40020024)

/* GPIOC */
#define GPIOC_MODER   (*(volatile uint32_t*)0x40020800)
#define GPIOC_OTYPER  (*(volatile uint32_t*)0x40020804)
#define GPIOC_OSPEEDR (*(volatile uint32_t*)0x40020808)
#define GPIOC_PUPDR   (*(volatile uint32_t*)0x4002080C)
#define GPIOC_AFRH    (*(volatile uint32_t*)0x40020824)

/* I2C3 */
#define I2C3_CR1   (*(volatile uint32_t*)0x40005C00)
#define I2C3_CR2   (*(volatile uint32_t*)0x40005C04)
#define I2C3_DR    (*(volatile uint32_t*)0x40005C10)
#define I2C3_SR1   (*(volatile uint32_t*)0x40005C14)
#define I2C3_SR2   (*(volatile uint32_t*)0x40005C18)
#define I2C3_CCR   (*(volatile uint32_t*)0x40005C1C)
#define I2C3_TRISE (*(volatile uint32_t*)0x40005C20)

#define SLAVE_ADDR 0x18
#define WHO_AM_I   0x0F
#define CTRL1 0x10
#define CTRL3 0x12
#define CTRL4 0x13
#define CTRL5 0x14
#define AXIS_STR_ADDR 0x28

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

void I2C3_GPIO_Init()
{
    /* Enable GPIOA and GPIOC clocks */
    RCC_AHB1ENR |= (1<<0);
    RCC_AHB1ENR |= (1<<2);

    /* PA8 Alternate function */
    GPIOA_MODER &= ~(3<<16);
    GPIOA_MODER |=  (2<<16);

    /* Open-drain */
    GPIOA_OTYPER |= (1<<8);

    /* High speed */
    GPIOA_OSPEEDR |= (3<<16);

    /* Pull-up */
    GPIOA_PUPDR &= ~(3<<16);
    GPIOA_PUPDR |=  (1<<16);

    /* AF4 for I2C3 */
    GPIOA_AFRH &= ~(0xF<<0);
    GPIOA_AFRH |=  (4<<0);


    /* PC9 Alternate function */
    GPIOC_MODER &= ~(3<<18);
    GPIOC_MODER |=  (2<<18);

    GPIOC_OTYPER |= (1<<9);

    GPIOC_OSPEEDR |= (3<<18);

    GPIOC_PUPDR &= ~(3<<18);
    GPIOC_PUPDR |=  (1<<18);

    GPIOC_AFRH &= ~(0xF<<4);
    GPIOC_AFRH |=  (4<<4);
}
void I2C3_Init()
{
    /* Enable I2C3 clock */
    RCC_APB1ENR |= (1<<23);

    /* Disable I2C before configuration */
    I2C3_CR1 &= ~(1<<0);

    /* APB1 clock frequency = 16 MHz */
    I2C3_CR2 = 16;

    /* 100 kHz I2C */
    I2C3_CCR = 80;

    /* Rise time */
    I2C3_TRISE = 17;

    /* Enable ACK */
    I2C3_CR1 |= (1<<10);

    /* Enable I2C */
    I2C3_CR1 |= (1<<0);
}
uint8_t I2C3_Read_WHOAMI()
{
    uint8_t data;

    /* START */
    I2C3_CR1 |= (1<<8);

    while(!(I2C3_SR1 & (1<<0)));

    /* Address write */
    I2C3_DR = SLAVE_ADDR << 1;

    while(!(I2C3_SR1 & (1<<1)));

    volatile uint32_t temp = I2C3_SR1;
    temp = I2C3_SR2;

    /* Send register address */
    while(!(I2C3_SR1 & (1<<7)));
    I2C3_DR = WHO_AM_I;

    while(!(I2C3_SR1 & (1<<7)));

    /* Repeated START */
    I2C3_CR1 |= (1<<8);

    while(!(I2C3_SR1 & (1<<0)));

    /* Address read */
    I2C3_DR = (SLAVE_ADDR<<1) | 1;

    while(!(I2C3_SR1 & (1<<1)));

    temp = I2C3_SR1;
    temp = I2C3_SR2;

    /* Disable ACK */
    I2C3_CR1 &= ~(1<<10);

    /* STOP */
    I2C3_CR1 |= (1<<9);

    while(!(I2C3_SR1 & (1<<6)));

    data = I2C3_DR;

    return data;
}
void I2C3_Write_Register(uint8_t reg, uint8_t data)
{
    /* START */
    I2C3_CR1 |= (1<<8);
    while(!(I2C3_SR1 & (1<<0)));

    /* Address write */
    I2C3_DR = SLAVE_ADDR << 1;
    while(!(I2C3_SR1 & (1<<1)));

    volatile uint32_t temp = I2C3_SR1;
    temp = I2C3_SR2;

    /* Send register */
    while(!(I2C3_SR1 & (1<<7)));
    I2C3_DR = reg;

    while(!(I2C3_SR1 & (1<<7)));

    /* Send data */
    I2C3_DR = data;

    while(!(I2C3_SR1 & (1<<2))); // BTF

    /* STOP */
    I2C3_CR1 |= (1<<9);
}
void LIS2DUX12_Init_Reg()
{
    HAL_Delay(50);

    /* CTRL1 */
    I2C3_Write_Register(CTRL1, 0x37);

    /* CTRL3 high performance */
    I2C3_Write_Register(CTRL3, 0x20);

    /* CTRL4 BDU enable */
    I2C3_Write_Register(CTRL4, 0x08);

    /* CTRL5 ODR=100Hz */
    I2C3_Write_Register(CTRL5, 0x80);

    HAL_Delay(50);
}
void I2C3_Read_Multi(uint8_t reg, uint8_t *buffer, uint8_t length)
{
    volatile uint32_t temp;

    /* START */
    I2C3_CR1 |= (1<<8);
    while(!(I2C3_SR1 & (1<<0)));

    /* Address write */
    I2C3_DR = SLAVE_ADDR << 1;
    while(!(I2C3_SR1 & (1<<1)));

    temp = I2C3_SR1;
    temp = I2C3_SR2;

    /* Send register */
    while(!(I2C3_SR1 & (1<<7)));
    I2C3_DR = reg;

    while(!(I2C3_SR1 & (1<<2))); // BTF

    /* Repeated START */
    I2C3_CR1 |= (1<<8);
    while(!(I2C3_SR1 & (1<<0)));

    /* Address read */
    I2C3_DR = (SLAVE_ADDR<<1) | 1;
    while(!(I2C3_SR1 & (1<<1)));

    temp = I2C3_SR1;
    temp = I2C3_SR2;

    I2C3_CR1 |= (1<<10); // Enable ACK

    for(int i=0;i<length;i++)
    {
        if(i == length-1)
        {
            I2C3_CR1 &= ~(1<<10); // Disable ACK
            I2C3_CR1 |= (1<<9);   // STOP
        }

        while(!(I2C3_SR1 & (1<<6))); // RXNE

        buffer[i] = I2C3_DR;
    }
}
void Read_XYZ()
{
    uint8_t buffer[6];
    int16_t x,y,z;

    I2C3_Read_Multi(AXIS_STR_ADDR | 0x80, buffer, 6);

    x = (int16_t)(buffer[1] << 8 | buffer[0]);
    y = (int16_t)(buffer[3] << 8 | buffer[2]);
    z = (int16_t)(buffer[5] << 8 | buffer[4]);

    float ax = x * 0.061f / 1000.0f;
    float ay = y * 0.061f / 1000.0f;
    float az = z * 0.061f / 1000.0f;

    printf("X: %.3f g  Y: %.3f g  Z: %.3f g\r\n", ax, ay, az);
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t who;
  I2C3_GPIO_Init();
      I2C3_Init();

      LIS2DUX12_Init_Reg();
  while (1)
  {
	        Read_XYZ();
	  //who = I2C3_Read_WHOAMI();

	          //printf("WHO_I = 0x%X\r\n", who);

	         HAL_Delay(1000);
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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

