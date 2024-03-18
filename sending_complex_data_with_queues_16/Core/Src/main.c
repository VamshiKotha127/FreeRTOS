#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
int __io_putchar(int ch);

void vSenderTask(void* pvParameters);
void vReceiverTask(void* pvParameters);

typedef enum
{
	//our system has humidity and pressure sensors

	humidity_sensor,
	pressure_sensor

}DataSource_t;

typedef struct
{
	//structure type to be passed to the queue

	uint8_t ucValue; //value of the sensor
	DataSource_t sDataSource; //data source

}Data_t;

//Declare two "Data_t" variables that will be passed to the queue
static const Data_t xStructsToSend[2] =
{
	{77, humidity_sensor}, //used by humidity sensor
	{63, pressure_sensor}  //used by pressure sensor
};

TaskHandle_t	humidity_task_handle,pressure_task_handle,receiver_task_handle;

QueueHandle_t xQueue;

int main(void)
{

  HAL_Init();// initialises hardware stuff.It also initialise the timer that we want to use for delay function

  /* Configure the system clock */
  SystemClock_Config(); // configures main system clock and other buses

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  xQueue = xQueueCreate(3,sizeof(Data_t));

  //create a receiver task with priority 1
  xTaskCreate(vReceiverTask,
		  	  "Receiver Task",
			  100,
			  NULL,
			  1,
			  &receiver_task_handle);

  //Both humidity and pressure sensor data uses same vSenderTask

  //create task to send humidity data with a priority of 2
  xTaskCreate(vSenderTask,
  		  	  "Humidity Task",
  			  100,
  			  (void*)(&xStructsToSend[0]),
  			  2,
  			  &humidity_task_handle);

  //create task to send pressure data with a priority of 2
    xTaskCreate(vSenderTask,
    		  	  "Pressure Task",
    			  100,
    			  (void*)(&xStructsToSend[1]),
    			  2,
    			  &pressure_task_handle);


  /* Init scheduler */
  vTaskStartScheduler();

  while(1)
  {
  }
}

void vSenderTask(void* pvParameters)
{
	BaseType_t queueStatus;

	//enter the blocked state for 200ms for space to become available in the queue each time the queue is full

	const TickType_t wait_time = pdMS_TO_TICKS(200);

	while(1)
	{
		queueStatus = xQueueSend(xQueue, pvParameters,wait_time);

		if(queueStatus != pdPASS)
		{
			//do something..
		}

		//Pseudo delay
		for(int i=0; i < 100000; i++){};
	}
}

void vReceiverTask(void* pvParameters)
{
	BaseType_t queueStatus;

	Data_t xReceivedStructure;

	while(1)
	{
		queueStatus = xQueueReceive(xQueue, &xReceivedStructure,0);

		if(queueStatus == pdPASS)
		{
			//received data
			if(xReceivedStructure.sDataSource == 0)
			{
				printf("Data received from Humidity sensor. Sensor value is : %d ... \n\r",xReceivedStructure.ucValue);
			}
			else
			{
				printf("Data received from Pressure sensor. Sensor value is : %d ... \n\r",xReceivedStructure.ucValue);
			}
		}
		else
		{
			//do something...
		}

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
