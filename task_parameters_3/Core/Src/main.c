#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>

//here we want to enable 4 gpio pins on port D. going to see how these pins toggle when we place them in different tasks

UART_HandleTypeDef huart2;

#define GREEN				GPIO_PIN_12
#define ORANGE				GPIO_PIN_13
#define RED					GPIO_PIN_14
#define BLUE				GPIO_PIN_15

const uint16_t blue_led = (uint16_t)(BLUE);
const uint16_t red_led = (uint16_t)(RED);
const uint16_t green_led = (uint16_t)(GREEN);
const uint16_t orange_led = (uint16_t)(ORANGE);

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
int __io_putchar(int ch);
void vLedControllerTask(void *pvParameters);
//void vBlueLedControllerTask(void *pvParameters);
//void vRedLedControllerTask(void *pvParameters);
//void vGreenLedControllerTask(void *pvParameters);
//void vOrangeLedControllerTask(void *pvParameters);

typedef uint32_t TaskProfiler;
TaskProfiler BlueTaskProfiler,RedTaskProfiler,GreenTaskProfiler,OrangeTaskProfiler;//used for checking whether tasks are getting started or not

int main(void)
{

  HAL_Init();// initialises hardware stuff.It also initialize the timer that we want to use for delay function

  /* Configure the system clock */
  SystemClock_Config(); // configures main system clock and other buses

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  //100 bytes stack size
  //handle is used for read task info and make the task do other functions (here it is NULL)
  //priority is 1. here parameters are NULL
  //if all three tasks have same priority, there will be issues here as uart takes some time for printing the debug statements.
  //and there will be a case where time slice(context switch) occurs before printing. This might happen for all tasks.
  //means no single task has enough time for printing the debug statement using uart2

  xTaskCreate(vLedControllerTask,
		  	  "Blue Led Controller",
			  100,
			  (void*)&blue_led ,
			  1,
			  NULL);

  xTaskCreate(vLedControllerTask,
 		  	  "Red Led Controller",
 			  100,
			  (void*)&red_led,
 			  1,
 			  NULL);

  xTaskCreate(vLedControllerTask,
  		  	  "Green Led Controller",
  			  100,
			  (void*)&green_led,
  			  1,
  			  NULL);
  xTaskCreate(vLedControllerTask,
    		  	  "Orange Led Controller",
    			  100,
				  (void*)&orange_led,
    			  1,
    			  NULL);

  /* Init scheduler */
  vTaskStartScheduler();

  while(1)
  {
	  //this wont get executed because scheduler has already started before this while loop. this while loop is unreachable
	  //printf("Hello World!..... \n\r");
  }
}

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2,(uint8_t* )&ch, 1, 0xFFFF);
	return ch;

}

void vLedControllerTask(void *pvParameters)
{
	while(1)
	{
		HAL_GPIO_TogglePin(GPIOD,*((uint16_t*)(pvParameters)));

		//adding delay
		for(int i=0; i<60000; i++)
		{

		}
	}
}

//void vBlueLedControllerTask(void *pvParameters)
//{
//	while(1)
//	{
//		//BlueTaskProfiler++;
//		HAL_GPIO_TogglePin(GPIOD,BLUE);
//		//printf("vBlueLedControllerTask running \n\r");
//	}
//}
//
//void vRedLedControllerTask(void *pvParameters)
//{
//	while(1)
//	{
//		//RedTaskProfiler++;
//		HAL_GPIO_TogglePin(GPIOD,RED);
//		//printf("vRedLedControllerTask running \n\r");
//	}
//}
//
//void vGreenLedControllerTask(void *pvParameters)
//{
//	while(1)
//	{
//		//GreenTaskProfiler++;
//		HAL_GPIO_TogglePin(GPIOD,GREEN);
//		//printf("vGreenLedControllerTask running \n\r");
//	}
//}
//
//void vOrangeLedControllerTask(void *pvParameters)
//{
//	while(1)
//	{
//		//OrangeTaskProfiler++;
//		HAL_GPIO_TogglePin(GPIOD,ORANGE);
//		//printf("vOrangeLedControllerTask running \n\r");
//	}
//}

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
	GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  //enable clock access to port
  __HAL_RCC_GPIOD_CLK_ENABLE();

  //reset pins
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET);

  //or HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,GPIO_PIN_RESET);//resets all pins at once

  //configure pins
  GPIO_InitStruct.Pin  = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // normal output mode
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  //intialize pins
  HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);
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
