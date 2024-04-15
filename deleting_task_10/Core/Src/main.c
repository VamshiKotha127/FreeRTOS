#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <stdbool.h>

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
int __io_putchar(int ch);
void vBlueLedControllerTask(void *pvParameters);
void vRedLedControllerTask(void *pvParameters);
void vGreenLedControllerTask(void *pvParameters);

uint32_t execution_monitor;
uint32_t resume_monitor;
bool is_killed = false;

typedef uint32_t TaskProfiler;
TaskProfiler BlueTaskProfiler,RedTaskProfiler,GreenTaskProfiler;//used for checking whether tasks are getting started or not

TaskHandle_t blue_handle,red_handle,green_handle;

uint32_t green_priority;//store priority of green task

int main(void)
{

  HAL_Init();// initialises hardware stuff.It also initialize the timer that we want to use for delay function

  /* Configure the system clock */
  SystemClock_Config(); // configures main system clock and other buses

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  xTaskCreate(vBlueLedControllerTask,
		  	  "Blue Led Controller",
			  100,
			  NULL,
			  1,
			  &blue_handle);

  xTaskCreate(vRedLedControllerTask,
 		  	  "Red Led Controller",
 			  100,
 			  NULL,
 			  1,
			  &red_handle);

  xTaskCreate(vGreenLedControllerTask,
  		  	  "Green Led Controller",
  			  100,
  			  NULL,
  			  1,
			  &green_handle);

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
		for(int i=0;i < 100000;i++)
		{

		}
	}
}

void vRedLedControllerTask(void *pvParameters)
{
	while(1)
	{
		RedTaskProfiler++;
		for(int i=0;i < 100000;i++)
		{

		}

		execution_monitor++;

		if(execution_monitor >= 50)
		{
			is_killed = true;
			execution_monitor=0;
			vTaskDelete(NULL); //killing red task. task gets killed at this line only
		}

	}
}

void vGreenLedControllerTask(void *pvParameters)
{
	while(1)
	{
		GreenTaskProfiler++;
		for(int i=0;i < 100000;i++)
		{

		}

		if(is_killed)
		{
			resume_monitor++;
			if(resume_monitor >= 30)
			{
				vTaskResume(red_handle);//resuming red task which was killed. But it wont get resumed as red task is deleted not suspended
				resume_monitor=0;
				is_killed=false;

				//there is another state called block state. task will enter block when its waiting for a resource like semaphore (key) or sleep is called in the task till some event. when it gets the resource again it is added in the ready queue
				//we can also suspend a task from block state. then task goes to suspend stage which can be sent to ready state on resuming
				//functions which make the task go into block state is called as blocking APIs
				//suspended block state --> processes from block state are moved to secondary memory
				//Two things cause task to enter into block state : 1. Time related event 2. Synchronization event

				//vTicksToDelay(TickType_t xTicksToDelay) -->this blocking API is used for delaying the task till some time
				// it takes number of ticks as an argument.
				//The number of tick interrupts that the calling task will remain in the blocked stage before being transitioned back into the ready state

				//pdMS_TO_TICKS() -->	converts a time specified in milliseconds into a time specified in tick
				//vTaskDelay(pdMS_TO_TICKS(100)) --> passing 100ms as delay which gets converted into ticks and then passed into vTaskDelay function
			}
		}
	}
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
