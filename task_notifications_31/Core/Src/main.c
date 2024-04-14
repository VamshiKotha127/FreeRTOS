#include "main.h"
#include "cmsis_os.h"
#include "uart.h"
#include "adc.h"
#include "exti.h"

#include <stdio.h>

	//we want to send a notification to handler function when a push button is pressed


void SystemClock_Config(void);
static void MX_GPIO_Init(void);

void vHandlerTask(void* pvParameters);

#define STACK_SIZE 128 // 128 * 4 = 512 Bytes
static TaskHandle_t xHandlerTask = NULL;
int main(void)
{

  HAL_Init();// initialises hardware stuff.It also initialise the timer that we want to use for delay function

  /* Configure the system clock */
  SystemClock_Config(); // configures main system clock and other buses

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  USART2_UART_TX_Init();
  p13_interrupt_init(); //pc13 enables in interrupt mode

  xTaskCreate(vHandlerTask, "Handler Task", STACK_SIZE, NULL, 3, &xHandlerTask);
  /* Init scheduler */
  vTaskStartScheduler();

  while(1)
  {

  }
}

void vHandlerTask(void* pvParameters)
{
	const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS(100);// 100ms
	while(1)
	{
		//receive/take notification.
		//xClearCountOnExit -->pdFALSE means RTOS notification value gets decremented before we exit this function(its like counting semaphore getting decremented after taking)
		//if its true, it is like binary semaphore made to zero

		if(ulTaskNotifyTake(pdFALSE, xMaxExpectedBlockTime) != 0)
		{
			//printing to usart
			printf("Handler Task - Processing event \r\n");
		}
		else
		{
			//do something
		}
	}
}

void EXTI15_10_IRQHandler(void)
{
	//IRQ handler for pc13. This function gets executed when push button is pressed
	//we want to send a notification to handler function when a push button is pressed

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	vTaskNotifyGiveFromISR(xHandlerTask, &xHigherPriorityTaskWoken);

	EXTI->PR = 0x2000; // clear interrupt pending flag from PR register after handling interrupt

	//request for a context switch
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
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

