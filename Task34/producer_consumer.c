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
#define SLAVE_ADDR  (0x19<< 1)
#define WHO_AM_I    0x0F
#define CTRL1       0x10
#define CTRL4       0x13
#define CTRL3       0x12
#define CTRL5       0x14
#define AXIS_STR_ADDR 0x28
#define SHT40_ADDR   (0x44<< 1)

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for sensor_Timer01 */
osTimerId_t sensor_Timer01Handle;
const osTimerAttr_t sensor_Timer01_attributes = {
  .name = "sensor_Timer01"
};
/* Definitions for logTimer */
osTimerId_t logTimerHandle;
const osTimerAttr_t logTimer_attributes = {
  .name = "logTimer"
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
void StartDefaultTask(void *argument);
void Callback01(void *argument);
void LogTimerCallback(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
    //osMutexAcquire(uart_mutexHandle, osWaitForever);
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 10);
    //osMutexRelease(uart_mutexHandle);
    return ch;
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
//	    printf("\r\n[INIT] Sensor configuration summary:\r\n");
//	        printf("  Sensor        : LIS2DU12 (STMicroelectronics)\r\n");
//	        printf("  Interface     : I2C @ 100 kHz\r\n");
//	        printf("  Mode          : High-Performance\r\n");
//	        printf("  Output Rate   : 100 Hz\r\n");
//	        printf("  Full Scale    : +/- 2g\r\n");
//	        printf("  Sensitivity   : 0.061 mg/LSB\r\n");
//	        printf("  BDU           : Enabled\r\n");
//	        printf("  Auto-Increment: Enabled\r\n");
//	        printf("\r\n[INIT] LIS2DU12 ready. Starting data acquisition...\r\n");
//	        printf("========================================\r\n\r\n");
	        //osMutexRelease(uart_mutexHandle);
}
uint8_t buffer[6];
void LIS2DUX12_data(){



	int16_t x, y, z;
	//osMutexAcquire(uart_mutexHandle, osWaitForever);

	//osMutexRelease(uart_mutexHandle);

	//osMutexAcquire(i2c_mutexHandle, osWaitForever);
	HAL_I2C_Mem_Read(&hi2c1,SLAVE_ADDR,0x28 | 0x80,I2C_MEMADD_SIZE_8BIT,buffer,6,100);
	//osMutexRelease(i2c_mutexHandle);

	//osMutexAcquire(uart_mutexHandle, osWaitForever);

	//osMutexRelease(uart_mutexHandle);
//    x = (int16_t)(buffer[1] << 8 | buffer[0]);
//    y = (int16_t)(buffer[3] << 8 | buffer[2]);
//    z = (int16_t)(buffer[5] << 8 | buffer[4]);

    //printf("raw_X:%d  raw_Y:%d  raw_Z:%d \r\n", x, y, z);

//    float ax = x*0.061f/1000.0f;
//    float ay = y*0.061f/1000.0f;
//    float az = z*0.061f/1000.0f;

    //osMutexAcquire(uart_mutexHandle, osWaitForever);
//    printf("X: %.3f g  Y: %.3f g  Z: %.3f g\r\n\n", ax, ay, az);
    //osMutexRelease(uart_mutexHandle);



}
void LOG_DATA(){
	int16_t x, y, z;
	    x = (int16_t)(buffer[1] << 8 | buffer[0]);
	    y = (int16_t)(buffer[3] << 8 | buffer[2]);
	    z = (int16_t)(buffer[5] << 8 | buffer[4]);

	    float ax = x*0.061f/1000.0f;
	    float ay = y*0.061f/1000.0f;
	    float az = z*0.061f/1000.0f;

	    printf("X: %.3f g  Y: %.3f g  Z: %.3f g\r\n\n", ax, ay, az);

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
  LIS2DUX12_Init();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of sensor_Timer01 */
  sensor_Timer01Handle = osTimerNew(Callback01, osTimerPeriodic, NULL, &sensor_Timer01_attributes);

  /* creation of logTimer */
  logTimerHandle = osTimerNew(LogTimerCallback, osTimerPeriodic, NULL, &logTimer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  osTimerStart(sensor_Timer01Handle, 1000);  // 1 sec
   osTimerStart(logTimerHandle, 2000);     // 2 sec
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

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

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* Callback01 function */
void Callback01(void *argument)
{
  /* USER CODE BEGIN Callback01 */
	printf("Sensor Read Triggered\r\n");
	LIS2DUX12_data();
  /* USER CODE END Callback01 */
}

/* LogTimerCallback function */
void LogTimerCallback(void *argument)
{
  /* USER CODE BEGIN LogTimerCallback */
	printf("Logging Data\r\n");
	LOG_DATA();
  /* USER CODE END LogTimerCallback */
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

