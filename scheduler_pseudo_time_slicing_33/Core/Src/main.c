#include "main.h"
#include "cmsis_os.h"
#include "uart.h"
#include "adc.h"
#include "exti.h"

#include <stdio.h>

 	 //here we disable time slicing option. means we will only have pre-emption only
	// here all tasks have equal priority. if we disable time slicing then only orange will as it is created first
	// when we add delay function, tasks will enter into sleep mode(block mode) one by one. Because of this as scheduler is idle, remaining tasks will get the processor

typedef uint32_t TaskProfiler_t;

const TickType_t _50ms = pdMS_To_TICKS(50);

TaskProfiler_t Orange_TaskProfiler,Red_TaskProfiler, Green_TaskProfiler,Blue_TaskProfiler;

void vGreenLedControllerTask(void *pvParameters);
void vBlueLedControllerTask(void *pvParameters);
void vRedLedControllerTask(void *pvParameters);
void vOrangeLedControllerTask(void *pvParameters);

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void)
{

  HAL_Init();// initialises hardware stuff.It also initialise the timer that we want to use for delay function

  /* Configure the system clock */
  SystemClock_Config(); // configures main system clock and other buses

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  USART2_UART_TX_Init();

  //blue and red have higher priority

  xTaskCreate(vOrangeLedControllerTask, "Orange Led Controller", 100, NULL, 1, NULL);
  xTaskCreate(vBlueLedControllerTask, "Blue Led Controller", 100, NULL, 1, NULL);
  xTaskCreate(vRedLedControllerTask, "Red Led Controller", 100, NULL, 1, NULL);
  xTaskCreate(vGreenLedControllerTask, "Green Led Controller", 100, NULL, 1, NULL);

  /* Init scheduler */
  vTaskStartScheduler();

  while(1)
  {

  }
}

void vGreenLedControllerTask(void *pvParameters)
{
	while(1)
	{
		Green_TaskProfiler++;
		for(int i=0; i < 700000; i++)
		{

		}
		vTaskDelay(_50ms); //task will enter blocked state(sleep state)
	}
}

void vOrangeLedControllerTask(void *pvParameters)
{
	while(1)
	{
		Orange_TaskProfiler++;
		for(int i=0; i < 700000; i++)
		{

		}
		vTaskDelay(_50ms); //task will enter blocked state

	}
}

void vBlueLedControllerTask(void *pvParameters)
{
	while(1)
	{
		Blue_TaskProfiler++;
		for(int i=0; i < 700000; i++)
		{

		}
		vTaskDelay(_50ms); //task will enter blocked state
	}
}
void vRedLedControllerTask(void *pvParameters)
{
	while(1)
	{
		Red_TaskProfiler++;
		for(int i=0; i < 700000; i++)
		{

		}
		vTaskDelay(_50ms); //task will enter blocked state(sleep state)

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

