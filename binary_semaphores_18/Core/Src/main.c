#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>

//A semaphore is a signal sent between tasks or between tasks and interrupts
//
//Binary Semaphore :
//This semaphore can assume two values 1 or 0 to indicate whether there is a signal or not. A task either has the key or it does not
//
//Counting Semaphore:
//
//A semaphore with an associate counter which can be incremented or decremented. the counter indicates the number of keys available to access a particular resource
//
//Mutex:
//
//Stands for mutual exclusion. allows multiple tasks to access a single shared resource but only one at a time.
//
//
//For creating binary semaphores --> SemaphoreHandle_t xSempahoreCreateBinary(void);
//
//For giving the semaphore key after usage to others--> BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore);
//
//BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t xSemaphore, BaseType_t *pxHigherPriortyTaskWoken);
//
//In freertos, all the ISR functions will have ISR at the end of the name
//
//Suppose, if we want to use xSemaphoreGive in an interrupt service routine, we need to use xSemaphoreGiveFromISR
//
//For taking the semaphore --> BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait);
//
//By default counting semaphores are not enables. To enable them, use configUSE_COUNTING_SEMAPHORES 1 in FreeRTOSConfig.h
//
//SemaphoreHandle_t xSemaphoreCreateCounting(UBaseType_t uxMaxCount, UBaseType_t uxInitialCount);
//generally initiaCOunt is 0
//
//to use mutexes, we need to enable them
//
//SemaphoreHandle_t xSemaphoreCreateMutex(void);
//
//
//Priority inversion:
//
//This occurs when a higher priority task is waiting for a lower priority task inherently assumes the priority of the lower priority task. Here low priority task is holding a resource like semaphore
//
//priority inheritance
//
//temporarily raising the priority of the resource holder(lower priority task) to the priority of the highest priority task waiting for the resource
//
//deadlock
//
//a deadlock occurs when two tasks cannot proceed because they are both waiting for a resource that is held by the other
//
//Gatekeeper task:
//
//A gatekeeper task is a task that has sole ownership of a resource.
//
//only the gatekeeper task is allowed to access the resource directly. Any other task needing to access the resource can do so only by indirectly by using the services of the gatekeeper
//
//we use gatekeeper task approach to reduce the problems that we face when we do not configure our semaphores
//

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
int __io_putchar(int ch);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

void vRedLedControllerTask(void *pvParameters);
void vYellowLedControllerTask(void *pvParameters);
void vBlueLedControllerTask(void *pvParameters);

SemaphoreHandle_t xBinarySemaphore;
typedef int TaskProfiler;

TaskProfiler RedLedProfiler, YellowLedProfiler, BlueLedProfiler;

int main(void)
{

  HAL_Init();// initialises hardware stuff.It also initialise the timer that we want to use for delay function

  /* Configure the system clock */
  SystemClock_Config(); // configures main system clock and other buses

  /* Initialise all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  printf("System Initialising \r\n");

  //create semaphore
  xBinarySemaphore = xSemaphoreCreateBinary();

  xTaskCreate(vRedLedControllerTask, "Red LED Task", 100, NULL, 1, NULL);
  xTaskCreate(vYellowLedControllerTask, "Yellow LED Task", 100, NULL, 1, NULL);
  xTaskCreate(vBlueLedControllerTask, "Blue LED Task", 100, NULL, 1, NULL);

  /* Init scheduler */
  vTaskStartScheduler();


  while(1)
  {
  }
}

void vRedLedControllerTask(void *pvParameters)
{
	//giving the semaphore for starting the execution of tasks
	xSemaphoreGive(xBinarySemaphore); // this is done for initialisation

	while(1)
	{
		xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
		//incrementing profile only when semaphore is taken
		RedLedProfiler++;

		printf("This is red task \r\n");
		xSemaphoreGive(xBinarySemaphore);
		vTaskDelay(1);//added 1 tick delay
		//This is added because printf may cause the task to get time sliced with releasing the semaphore.
		// when it again gets the chance to start from where it left, it might again take the semaphore making the semaphore inaccessible to other tasks

	}
}


void vYellowLedControllerTask(void *pvParameters)
{
	while(1)
	{
		xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);

		YellowLedProfiler++;

		printf("This is yellow task \r\n");
		xSemaphoreGive(xBinarySemaphore);
		vTaskDelay(1);//added 1 tick delay
	}
}


void vBlueLedControllerTask(void *pvParameters)
{
	while(1)
	{
		xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);

		BlueLedProfiler++;

		printf("This is blue task \r\n");
		xSemaphoreGive(xBinarySemaphore);
		vTaskDelay(1);//added 1 tick delay

	}
}

int uart2_write(int ch)
{
	while(!(USART2->SR & 0x0080)){} //checking the status register. Waiting here for the transmit buffer to be empty

	USART2->DR	=  (ch & 0xFF); // only 8 bit data at once

	return ch;
}

int __io_putchar(int ch)
{
	//HAL_UART_Transmit(&huart2,(uint8_t* )&ch, 1, 0xFFFF);
	// HAL_UART_Transmit function is taking more time to print. so, we are going to make some light function to transmit

	uart2_write(ch); // This is the new function
	return ch;

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

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

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */
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
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1) {
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
