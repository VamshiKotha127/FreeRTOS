#include "main.h"
#include "cmsis_os.h"
#include "uart.h"
#include "adc.h"
#include "exti.h"

#include <stdio.h>
//Here we have allocated the uart printf task to only one task(gatekeeper). analog and digital tasks should access usart using gatekeeper

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void analog_sensor_task(void *pvParameters);
void digital_sensor_task(void *pvParameters);
void lcd_task(void *pvParameters);
QueueHandle_t xPrintQueue;

int btn_state;
int sensor_value;

int main(void)
{

  HAL_Init();// initialises hardware stuff.It also initialise the timer that we want to use for delay function

  /* Configure the system clock */
  SystemClock_Config(); // configures main system clock and other buses

  /* Initialise all configured peripherals */
  MX_GPIO_Init();
  USART2_UART_TX_Init();

  printf("system initialising ... \n\r");

  xTaskCreate(digital_sensor_task, "ButtonRead", 128, NULL, 1, NULL);
  xTaskCreate(analog_sensor_task, "SensorRead", 128, NULL, 1, NULL);
  xTaskCreate(lcd_task, "Gatekeeper", 128, NULL, 0, NULL);

  xPrintQueue = xQueueCreate(2, sizeof(int32_t));

  /* Init scheduler */
  vTaskStartScheduler();

  while(1)
  {
  }
}

int value_to_print;

void lcd_task(void *pvParameters)
{
	while(1)
	{
		//wait for the message to arrive
		xQueueReceive(xPrintQueue, &value_to_print, portMAX_DELAY);
		printf("the new value is : %d \r\n", value_to_print);

	}
}


void digital_sensor_task(void *pvParameters)
{
	gpio_init();


	while(1)
	{
		  btn_state = read_digital_sensor();
		  xQueueSendToBack(xPrintQueue, &btn_state,0);
		  vTaskDelay(10); //increased delay because every task should get time
	}
}

void analog_sensor_task(void *pvParameters)
{
	adc_init();
	while(1)
	{
		  sensor_value = read_analog_sensor();
		  xQueueSendToBack(xPrintQueue, &sensor_value,0);
		  vTaskDelay(10);
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

