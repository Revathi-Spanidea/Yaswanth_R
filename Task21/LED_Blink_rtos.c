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
#include "cmsis_os.h"

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
#define RCC_AHB1ENR  (*(volatile unsigned int*)0x40023830)

#define GPIOA_MODER  (*(volatile unsigned int*)0x40020000)
#define GPIOA_ODR    (*(volatile unsigned int*)0x40020014)


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

typedef enum {
    REQ_SHT40,
    REQ_LIS2DUX12,
	REQ_LIS2DUX12_INIT
} I2C_Request_t;
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LED_Task */
osThreadId_t LED_TaskHandle;
const osThreadAttr_t LED_Task_attributes = {
  .name = "LED_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for i2c_mutex */
osMutexId_t i2c_mutexHandle;
const osMutexAttr_t i2c_mutex_attributes = {
  .name = "i2c_mutex"
};
/* Definitions for uart_mutex */
osMutexId_t uart_mutexHandle;
const osMutexAttr_t uart_mutex_attributes = {
  .name = "uart_mutex"
};

osThreadId_t I2C_TaskHandle;
const osThreadAttr_t I2C_Task_attributes = {
  .name = "I2C_Task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */

osMessageQueueId_t i2cQueue;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
void StartDefaultTask(void *argument);
void StartTask02(void *argument);
void I2C_Task(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//int __io_putchar(int ch)
//{
//	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
//    return ch;
//}
int __io_putchar(int ch)
{
    //osMutexAcquire(uart_mutexHandle, osWaitForever);
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 10);
    //osMutexRelease(uart_mutexHandle);
    return ch;
}
void delay()
{
    for(volatile int i = 0; i < 500000; i++);
}
void LED1(){
	/* 3. Turn LED ON */
		          GPIOA_ODR |= (1 << 5);
		              //osMutexAcquire(uart_mutexHandle, osWaitForever);
		              printf("LED1 ON\r\n");
		              //osMutexRelease(uart_mutexHandle);
		          osDelay(100);

		          /* 4. Turn LED OFF */
		          GPIOA_ODR &= ~(1 << 5);
		              //osMutexAcquire(uart_mutexHandle, osWaitForever);
		              printf("LED1 OFF\r\n");
		              //osMutexRelease(uart_mutexHandle);
		          osDelay(100);
}
void LED2(){
	/* 3. Turn LED ON */
	              GPIOA_ODR |= (1<<6);
		          printf("LED2 ON\r\n");
		          delay();

		          /* 4. Turn LED OFF */
		          GPIOA_ODR &= ~(1 << 5);
		          printf("LED1 OFF\r\n");
		          delay();
}
void LIS2DUX12_Init(void)
{
	osDelay(50);

	    uint8_t data;

	    /* Basic configuration */
	    data = 0x37;
	    HAL_I2C_Mem_Write(&hi2c1,
	                      SLAVE_ADDR,
	                      CTRL1,
	                      I2C_MEMADD_SIZE_8BIT,
	&data,
	                      1,
	                      100);

	    /* High performance mode */
	    data = 0x20;
	    HAL_I2C_Mem_Write(&hi2c1,
	                      SLAVE_ADDR,
	                      CTRL3,
	                      I2C_MEMADD_SIZE_8BIT,
	&data,
	                      1,
	                      100);

	    /* Enable BDU (Block Data Update) */
	    data = 0x08;
	    HAL_I2C_Mem_Write(&hi2c1,
	                      SLAVE_ADDR,
	                      CTRL4,
	                      I2C_MEMADD_SIZE_8BIT,
	&data,
	                      1,
	                      100);

	    /* ODR = 100 Hz, FS = ±2g */
	    data = 0x80;

	    HAL_I2C_Mem_Write(&hi2c1,
	                      SLAVE_ADDR,
	                      CTRL5,
	                      I2C_MEMADD_SIZE_8BIT,
	&data,
	                      1,
	                      100);
	    osDelay(50);

	    //osMutexAcquire(uart_mutexHandle, osWaitForever);
	    printf("\r\n[INIT] Sensor configuration summary:\r\n");
	        printf("  Sensor        : LIS2DU12 (STMicroelectronics)\r\n");
	        printf("  Interface     : I2C @ 100 kHz\r\n");
	        printf("  Mode          : High-Performance\r\n");
	        printf("  Output Rate   : 100 Hz\r\n");
	        printf("  Full Scale    : +/- 2g\r\n");
	        printf("  Sensitivity   : 0.061 mg/LSB\r\n");
	        printf("  BDU           : Enabled\r\n");
	        printf("  Auto-Increment: Enabled\r\n");
	        printf("\r\n[INIT] LIS2DU12 ready. Starting data acquisition...\r\n");
	        printf("========================================\r\n\r\n");
	        //osMutexRelease(uart_mutexHandle);
}

uint8_t data[6];   // moved outside function
HAL_StatusTypeDef status;

//void SHT40()
//{
//    uint8_t cmd = 0xFD;
//
////    if(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
////        return;
//    if(HAL_I2C_IsDeviceReady(&hi2c1, SHT40_ADDR, 2, 100) != HAL_OK)
//    {
//        printf("SHT40 NOT DETECTED!\n");
//        return;
//    }
//
//    // Send command (blocking is fine here)
//    if(HAL_I2C_Master_Transmit(&hi2c1, SHT40_ADDR, &cmd, 1, 100) != HAL_OK)
//    {
//        printf("TX Failed!\r\n");
//        return;
//    }
//
//    // Sensor conversion delay
//    osDelay(20);
//
//    // Receive using interrupt
//    if(HAL_I2C_Master_Receive_IT(&hi2c1, SHT40_ADDR, data, 6) != HAL_OK)
//    {
//        printf("RX Failed!\r\n");
//        return;
//    }
//}

void SHT40()
{
    uint8_t cmd = 0xFD;
    uint8_t data_local[6];

    osMutexAcquire(i2c_mutexHandle, osWaitForever);

    if(HAL_I2C_Master_Transmit(&hi2c1, SHT40_ADDR, &cmd, 1, 100) != HAL_OK)
    {
        osMutexRelease(i2c_mutexHandle);
        return;
    }

    osMutexRelease(i2c_mutexHandle);   // 🔥 RELEASE EARLY
    osThreadYield();

    osDelay(20);

    osMutexAcquire(i2c_mutexHandle, osWaitForever);

    if(HAL_I2C_Master_Receive(&hi2c1, SHT40_ADDR, data_local, 6, 100) != HAL_OK)
    {
        osMutexRelease(i2c_mutexHandle);
        return;
    }

    osMutexRelease(i2c_mutexHandle);   // 🔥 RELEASE EARLY
    osThreadYield();

    // Process outside mutex
    uint16_t temp_raw = (data_local[0] << 8) | data_local[1];
    uint16_t hum_raw  = (data_local[3] << 8) | data_local[4];

    float temp = -45 + 175 * (temp_raw / 65535.0f);
    float hum  = -6  + 125 * (hum_raw  / 65535.0f);

    printf("Temp: %.2f C  Hum: %.2f %%\r\n", temp, hum);
}

void LIS2DUX12_data(){


	uint8_t buffer[6];
	int16_t x, y, z;
	//osMutexAcquire(uart_mutexHandle, osWaitForever);

	//osMutexRelease(uart_mutexHandle);

	//osMutexAcquire(i2c_mutexHandle, osWaitForever);
	HAL_I2C_Mem_Read(&hi2c1,SLAVE_ADDR,0x28 | 0x80,I2C_MEMADD_SIZE_8BIT,buffer,6,100);
	//osMutexRelease(i2c_mutexHandle);

	//osMutexAcquire(uart_mutexHandle, osWaitForever);

	//osMutexRelease(uart_mutexHandle);
    x = (int16_t)(buffer[1] << 8 | buffer[0]);
    y = (int16_t)(buffer[3] << 8 | buffer[2]);
    z = (int16_t)(buffer[5] << 8 | buffer[4]);

    //printf("raw_X:%d  raw_Y:%d  raw_Z:%d \r\n", x, y, z);

    float ax = x*0.061f/1000.0f;
    float ay = y*0.061f/1000.0f;
    float az = z*0.061f/1000.0f;

    //osMutexAcquire(uart_mutexHandle, osWaitForever);
    printf("X: %.3f g  Y: %.3f g  Z: %.3f g\r\n\n", ax, ay, az);
    //osMutexRelease(uart_mutexHandle);



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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  /* 1. Enable clock for GPIOA */
      RCC_AHB1ENR |= (1 << 0);

      /* 2. Set PA5 as output */
      GPIOA_MODER &= ~(3 << 10);   // clear bits
      GPIOA_MODER |=  (1 << 10);   // output mode
      /*3. Set PA6 as output */
      GPIOA_MODER &= ~(3<<12);     // clear bits
      GPIOA_MODER |= (1<<12);      // output mode

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();


  /* Create the mutex(es) */
  /* creation of i2c_mutex */
  i2c_mutexHandle = osMutexNew(&i2c_mutex_attributes);

  /* creation of uart_mutex */
  uart_mutexHandle = osMutexNew(&uart_mutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  i2cQueue = osMessageQueueNew(5, sizeof(I2C_Request_t), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of LED_Task */
  LED_TaskHandle = osThreadNew(StartTask02, NULL, &LED_Task_attributes);

  I2C_TaskHandle = osThreadNew(I2C_Task, NULL, &I2C_Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

//	  LED1();
//	  LED2();

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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */
  HAL_NVIC_SetPriority(I2C1_EV_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);

  HAL_NVIC_SetPriority(I2C1_ER_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);

  /* USER CODE END I2C1_Init 2 */

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
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin PA13 */
  GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    // Do nothing here
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if(hi2c == &hi2c1)
    {
        printf("RX DONE\r\n");  // debug

        uint16_t temp_raw = (data[0] << 8) | data[1];
        uint16_t hum_raw  = (data[3] << 8) | data[4];

        float temp = -45 + 175 * (temp_raw / 65535.0f);
        float hum  = -6  + 125 * (hum_raw  / 65535.0f);

        printf("Temp: %.2f C  Hum: %.2f %%\r\n", temp, hum);
    }
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  //I2C_Request_t init_req = REQ_LIS2DUX12_INIT;
  //I2C_Request_t data_req = REQ_LIS2DUX12;

  //osMessageQueuePut(i2cQueue, &init_req, 0, 0);

  for(;;)
  {
      //osMessageQueuePut(i2cQueue, &data_req, 0, 0);
       LED1();
      //printf("TASK1\r\n");

      osDelay(200);
  }
}

void I2C_Task(void *argument)
{
    I2C_Request_t req;

    for(;;)
    {
        if(osMessageQueueGet(i2cQueue, &req, NULL, osWaitForever) == osOK)
        {
            switch(req)
            {
                case REQ_SHT40:
                    SHT40();
                    break;

                case REQ_LIS2DUX12:
                    LIS2DUX12_data();
                    break;

                case REQ_LIS2DUX12_INIT:
                    LIS2DUX12_Init();
                    break;
            }
        }
    }
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the LED_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  //I2C_Request_t req = REQ_SHT40;

  for(;;)
  {
      //osMessageQueuePut(i2cQueue, &req, 0, 0);

      //printf("TASK2\r\n");
      LED2();
      osDelay(500);
  }
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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

