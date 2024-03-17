#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
int __io_putchar(int ch);
void vBlueLedControllerTask(void *pvParameters);
void vRedLedControllerTask(void *pvParameters);
void vGreenLedControllerTask(void *pvParameters);

typedef uint32_t TaskProfiler;
TaskProfiler BlueTaskProfiler,RedTaskProfiler,GreenTaskProfiler,IdleTaskProfiler;//used for checking whether tasks are getting started or not

const TickType_t _250ms = pdMS_TO_TICKS(250);

int main(void)
{

  HAL_Init();// initialises hardware stuff.It also initialize the timer that we want to use for delay function

  /* Configure the system clock */
  SystemClock_Config(); // configures main system clock and other buses

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();

//idle task
//There must be at least one task in the running state at any given momemt. Because of this, the Idle task automatically moves to the running state when there is no task there
//The idle task is created automatically when we start the scheduler
//The idle task has the lowest priority(0), this ensure that it never prevents a higher priority application task from entering the running state
//we can add application specific functionality directly into the idle task through the use of an idle hook(or idle callback) function
//this function is called automatically by the idle task once per iteration of the idle task loop
//we can keep some cleanup code in the idle task callback
//we have to manually enable the idle hook/task function using configUSE_IDLE_HOOK() in freeRTOSConfig.h
//set configIDLE_SHOULD_YIELD to 1 is used to prevent idle task from consuming CPU time. It means idle task should yield to any high priority task when high priority task comes during idle task time slice

//tick hook is a function that is called by the kernel during each tick interrupt
//tick hook functions execute within the context of the tick interrupt, and so must be kept very short,must use only a moderate amount of stack space and must not call any FreeRTOS API functions that donot end with FromISR()
//tick hook function should be light. we need to configure this using configUSE_TICK_HOOK(0 in FreeRTOSConfig.h

  xTaskCreate(vBlueLedControllerTask,
		  	  "Blue Led Controller",
			  100,
			  NULL,
			  1,
			  NULL);

  xTaskCreate(vRedLedControllerTask,
 		  	  "Red Led Controller",
 			  100,
 			  NULL,
 			  1,
 			  NULL);

  xTaskCreate(vGreenLedControllerTask,
  		  	  "Green Led Controller",
  			  100,
  			  NULL,
  			  1,
  			  NULL);

  /* Init scheduler */
  vTaskStartScheduler();

  while(1)
  {
  }
}

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2,(uint8_t* )&ch, 1, 0xFFFF);
	return ch;

}

void vBlueLedControllerTask(void *pvParameters)
{
	while(1)
	{
		BlueTaskProfiler++;
		vTaskDelay(_250ms); //delaying blue task for some time for making sure that idle task gets into running state
	}
}

void vRedLedControllerTask(void *pvParameters)
{
	while(1)
	{
		RedTaskProfiler++;
		vTaskDelay(_250ms); //delaying red task for some time for making sure that idle task gets into running state

	}
}

void vGreenLedControllerTask(void *pvParameters)
{
	while(1)
	{
		GreenTaskProfiler++;
		vTaskDelay(_250ms); //delaying green task for some time for making sure that idle task gets into running state

	}
}

void vApplicationIdleHook(void)
{
	//callback function. This gets called automatically. We have enabled config_idle_hook from freertosconfig.h file
	IdleTaskProfiler++; // this will get incremented when blue,red,green task is in block stage because of timer
	//this task wont get time quantum without adding delay in ble,red,green because the priority of idle task is 0 which is less than 1.
	//blue,red,green tasks has priority as 1
	//clean up code goes here
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
