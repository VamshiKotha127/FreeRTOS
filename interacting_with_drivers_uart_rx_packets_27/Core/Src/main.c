#include "main.h"
#include "cmsis_os.h"
#include "uart.h"
#include "adc.h"
#include "exti.h"

#include <stdio.h>


//here we dont use polling. we use interrupt method for getting the data from usart
//transferring packets instead of single byte

#define STACK_SIZE 128// 128*4 bytes

#define EXPECTED_PKT_LENGTH 5

static QueueHandle_t uart2_BytesReceived = NULL;
static int rxInProgress=0; //is reception is int progress
static uint16_t rxLen=0;//length that we want to receive in bytes
static uint8_t* rxBuff = NULL;//pointer to the buffer when we are gonna store the received data
static uint16_t rxItr = 0;//itertor
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void vHandlerTask(void* pvParameters);
int start_rx_interrupt(uint8_t* buffer, uint16_t len);
static SemaphoreHandle_t rxDone=NULL;

uint8_t btn_state;
uint32_t sensor_value;

int main(void)
{

  HAL_Init();// initialises hardware stuff.It also initialise the timer that we want to use for delay function

  /* Configure the system clock */
  SystemClock_Config(); // configures main system clock and other buses

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  rxDone = xSemaphoreCreateBinary();
  xTaskCreate(vHandlerTask, "uartPrintTask", STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);

  uart2_BytesReceived = xQueueCreate(10, sizeof(char));

  /* Init scheduler */
  vTaskStartScheduler();

  while(1)
  {
  }
}


//sets up an interrupt rx for usart2
int start_rx_interrupt(uint8_t* buffer, uint16_t len)
{
	if(!rxInProgress && buffer != NULL)
	{
		rxInProgress = 1;
		rxLen=len;
		rxBuff = buffer;
		rxItr =0;

		USART2->CR1 |= 0x0020; //Enable RX Interrupt
		NVIC_SetPriority(USART2_IRQn, 6);
		NVIC_EnableIRQ(USART2_IRQn);
		return 0;
	}

	return -1;

}

void USART2_IRQHandler(void)
{
	//ISR
	//In freertos we can wake up high priority task after completion of interrupt instead of interrupted task
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if(USART2->SR & 0x0020)
	{
		uint8_t tempVal = (uint8_t) USART2->DR;

		if(rxInProgress)
		{
			rxBuff[rxItr++] = tempVal;
			if(rxItr == rxLen)
			{
				rxInProgress = 0;

				//here ISR is needed at the end
				xSemaphoreGiveFromISR(rxDone,&xHigherPriorityTaskWoken);
			}
		}
	}

	//yield from isr -->give up processor from ISR

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //saying that we don't need to bring the high priority task
}


uint8_t rxData[EXPECTED_PKT_LENGTH];

char rxCode[50] = {0};

void vHandlerTask(void* pvParameters)
{
	USART2_UART_RX_Init();	//initialise usart

	for(int i=0 ; i < EXPECTED_PKT_LENGTH ; i++)
	{
		rxData[i] = 0; //initialise buffer
	}
	const TickType_t timeout = pdMS_TO_TICKS(8000); //8seconds

	//handle the received bytes
	while(1)
	{
		start_rx_interrupt(rxBuff, EXPECTED_PKT_LENGTH); //initialising interrupt settings

		if(xSemaphoreTake(rxDone,timeout) == pdPASS)
		{
			//means if we are able to receive the semaphore within the timeout
			if(EXPECTED_PKT_LENGTH == rxItr)
			{
				sprintf(rxCode, "received"); //if we use printf it will again usart transmit instead we are using sprintf which will place the characters in buffer
			}
			else
			{
				sprintf(rxCode, "Length Mismatch");
			}
		}
		else
		{
			sprintf(rxCode, "timeout");
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

