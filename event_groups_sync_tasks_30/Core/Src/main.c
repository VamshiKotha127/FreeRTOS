#include "main.h"
#include "cmsis_os.h"
#include "uart.h"
#include "adc.h"
#include "exti.h"
#include <stdio.h>

#define TASK1_BIT (1UL << 0UL)
#define TASK2_BIT (1UL << 1UL)
#define TASK3_BIT (1UL << 1UL)

static void vTask3( void *pvParameters);
static void vTask2( void *pvParameters);
static void vTask1( void *pvParameters);

EventGroupHandle_t xEventGroup;

EventBits_t uxAllSyncBits = (TASK1_BIT | TASK2_BIT | TASK3_BIT );
const TickType_t xDelay500ms = pdMS_TO_TICKS(500UL);

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
  printf("Initialising system ... \n\r");

  xEventGroup = xEventGroupCreate();

  xTaskCreate(vTask1, "Task 1", 100, NULL, 1, NULL);
  xTaskCreate(vTask2, "Task 2", 100, NULL, 1, NULL);
  xTaskCreate(vTask3, "Task 3", 100, NULL, 1, NULL);



  /* Init scheduler */
  vTaskStartScheduler();

  while(1)
  {

  }
}

static void vTask1( void *pvParameters)
{
	EventBits_t uxReturn;

	while(1)
	{
		//each task set its specific bits and wait for all other bits in uxALLSyncBits. we will proceed only when all bits are set
		uxReturn = xEventGroupSync(xEventGroup, TASK1_BIT, uxAllSyncBits, portMAX_DELAY);

		if((uxReturn & uxAllSyncBits) == uxAllSyncBits)
		{
			//do something. All tasks are synced
		}
	}
}

static void vTask2( void *pvParameters)
{
	EventBits_t uxReturn;
	while(1)
	{
		//each task set its specific bits and wait for all other bits in uxALLSyncBits. we will proceed only when all bits are set
		uxReturn = xEventGroupSync(xEventGroup, TASK2_BIT, uxAllSyncBits, portMAX_DELAY);

		if((uxReturn & uxAllSyncBits) == uxAllSyncBits)
		{
			//do something. All tasks are synced
		}

	}
}

static void vTask3( void *pvParameters)
{
	EventBits_t uxReturn;

	while(1)
	{
		//each task set its specific bits and wait for all other bits in uxALLSyncBits. we will proceed only when all bits are set
		uxReturn = xEventGroupSync(xEventGroup, TASK3_BIT, uxAllSyncBits, portMAX_DELAY);

		if((uxReturn & uxAllSyncBits) == uxAllSyncBits)
		{
			//do something. All tasks are synced
			//if we remove xEventGroupSync function from task2, then task2 bit wont be set and we will never reach sync stage. task1 and task3 will be waiting for task2 to set its bit in event group
			printf("ALL TASKS SET\r\n");
			vTaskDelay(xDelay500ms);
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

