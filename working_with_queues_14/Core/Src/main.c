#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
int __io_putchar(int ch);

TaskHandle_t sender_handle,receiver_handle;

void vSenderTask(void *pvParameters);
void vReceiverTask(void *pvParameters);

QueueHandle_t yearQueue;

int main(void)
{

  HAL_Init();// initialises hardware stuff.It also initialize the timer that we want to use for delay function

  /* Configure the system clock */
  SystemClock_Config(); // configures main system clock and other buses

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  //Queues are used as FIFO buffers, where data is inserted at the back and removed from the front
  //Queues can hold a finite number of fixed data items
  // The maximum number of items a queue can hold is called the length of the queue
   //we can pass data into queue in two ways. 1. pass by value 2. pass by reference
   // pass by reference involves using queues to transfer pointers to the data, rather than copy the data itself into and out of the queue byte by byte
   // pass by reference is the preferred task for passing large queue data. It saves memory

   //Blocking on Queue Reads
   //when a task attempts to read from a queue, it can optionally specify a block time. This is the time the task will be kept in the blocked state to wait for data to be available from the queue,if the queue is empty.
   //As soon as data becomes available the task is automatically moved to the ready state

   //Blocking on Queue Writes
   //Task is placed in blocked state if queue is full, as soon as space becomes available in the queue task is moved to ready state

   //XQueueSend
   //	BaseType_t xQueueSend(QueueHandle_t xQueue, const void* pvItemToQueue, TickType_t xTickToWait);
   //xQueue is the handle of the queue
   //pvItemToQueue -->pointer to the variable/buffer that we want to send
   //xTickToWait --> how much time we want to wait(block time)

   //XQueueReceive
     //	BaseType_t XQueueReceive(QueueHandle_t xQueue,  void* pvBuffer, TickType_t xTickToWait);
     //xQueue is the handle of the queue
     //pvBuffer -->pointer to the variable/buffer where we want to store received data
     //xTickToWait --> how much time we want to wait(block time)

   //XQueueCreate
      //	QueueHandle_t XQueueReceive(UBaseType_t uxQueueLength, UBaseType_t uxItemSize);
      //uxQueueLength is the length of the queue (no. of items)
   	  // uxItemSize --> size of each item

   //Queuesets allow a task to receive data from more than one queue without the task polling each queue in turn to determine which, if any of the queues have new data

   //when we use queuesets, the task will automatically get a notification that one of the queuesets have new data
   //we need to manually set this configuration using configUSE_QUEUE_SETS() in FreeRTOSConfig.h file

   //xQueueCreateSet()
   // QueueSetHandle_t xQueueCreateSet(const UBaseType_t uxEventQueueLength);
   //uxEventQueueLength -->length of the queue sets

   // xQueueAddToSet() --> allows you to add queues to a queueset
   //BaseType_t xQueueAddToSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet)
   //as we can also add semaphores to a queueset the first argument contains semaphore in xQueueOrSemaphore
   //first argument QueueSetMemberHandle_t--> handle of the queue
   //second argument xQueueSet --> handle of the queueset

   //Queues are used to transfer data from one task to another task or from one task to one ISR or from one ISR to one task

  yearQueue = xQueueCreate(5,sizeof(int32_t));

  xTaskCreate(vSenderTask,
		  	  "Sender Task",
			  100,
			  NULL,
			  1,
			  &sender_handle);

  xTaskCreate(vReceiverTask,
  		  	  "Receiver Task",
  			  100,
  			  NULL,
  			  1,
  			  &receiver_handle);

  /* Init scheduler */
  vTaskStartScheduler();

  while(1)
  {
  }
}

void vSenderTask(void *pvParameters)
{
	int32_t value_to_send = 2050;
	BaseType_t queueStatus;

	while(1)
	{
		queueStatus = xQueueSend(yearQueue,&value_to_send,0); //it return queueStatus of BaseType_t
		//here block state wait time is 0. return from xQueueSend without any wait

		if(queueStatus != pdPASS)//pdPASS means true
		{
			printf("Error: Data couldn't be sent .....\n\r");
		}

		for(int i=0;i< 10000; i++){}//here task wont be in the blocked/wait state
	}
}
void vReceiverTask(void *pvParameters)
{
	int32_t value_received;
	const TickType_t wait_time = pdMS_TO_TICKS(100);//100ms
	BaseType_t queueStatus;

	while(1)
	{
		queueStatus = xQueueReceive(yearQueue,&value_received,wait_time);

		if(queueStatus == pdPASS)
		{
			printf("Received Value :   %ld .... \n\r",value_received);
		}
		else
		{
			printf("Error: Data couldn't receive .....\n\r");
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
