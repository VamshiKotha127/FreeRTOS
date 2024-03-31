#include "main.h"
#include "cmsis_os.h"
#include "uart.h"
#include "adc.h"
#include "exti.h"
#include <stdio.h>

//Software timers are used to schedule the execution of a function at a set time in the future or periodically with a fixed frequency
//
//the function executed by the software timer is called the software timer's callback function
//
//They are implemented under the control of the RTOS kernel
//
//They dont require hardware support and are not related to hardware timers
//
//To enable timer we need set configUSE_TIMERS in FreeRTOSConfig.h
//
//Two types of timers :
//1. Auto reload timer : once started, it will restart itself each time it expires resulting in periodic exdecution of its callback function
//2. one-shot timer : once started, it will execute its callback function once only. It can be restarted manually, but will not restart itself.
//
//
//The"period" is the time between the software timer being started and the software timer's callback function executing
//
//There are two states of timers:
//
//1. Dormant : callback function doesnot execute
//2. Running : executes callback function
//
//
//In case of auto reload timer:
//
//By default when we create timer using xTimerCreate(), it will be in dormant state.
//After using xTimerStart() or xTimerReset() or xTimerChangePeriod() timer state gets changed to Running state.
//
//when timer expires it will be again in running state and when xTimerStop() is called timer will be back in dormant state.
//
//In case of one shot timer, when timer expires it will enter dormant state again or when we use xTimerStop() then also it will enter dormant state
//
//TimerHandle_t xTimerCreate( const char* const pcTimerName, TickType_t TimerPeriodInTicks, UBaseType_t uxAutoReload, void *pvTimerID, TimerCallbackFunction_t pxCallbackFunction);
//
//BaseType_t xTimerStart(TimerHandle_t xTimer, TickType_t TicksToWait); Here this function takes timer handle as input and tickstowait means how much we need to wait for this fucntion gets executed
//
//void vTimerSetTimerID(const TimerHandle_t xTimer, void *pvNewID); For setting timer ID
//
//void *pvTimerGetTimerID(TimerHandle_t xTimer); For getting the timer id

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void prvAutoReloadTimerCallback(TimerHandle_t xTimer);
void prvOneShotTimerCallback(TimerHandle_t xTimer);

// The periods assigned to the one-shot and auto reload timers respectively
#define mainONE_SHOT_TIMER_PERIOD				(pdMS_TO_TICKS(4000UL)) //4000ms
#define mainAUTO_RELOAD_TIMER_PERIOD			(pdMS_TO_TICKS(500UL))  //500ms

TimerHandle_t xAutoReloadTimer, xOneShotTimer;
BaseType_t xTimer1Started, xTimer2Started;

int main(void)
{

  HAL_Init();// initialises hardware stuff.It also initialise the timer that we want to use for delay function

  /* Configure the system clock */
  SystemClock_Config(); // configures main system clock and other buses

  /* Initialise all configured peripherals */
  MX_GPIO_Init();
  USART2_UART_TX_Init();

  xOneShotTimer = xTimerCreate("OneShot",mainONE_SHOT_TIMER_PERIOD,pdFALSE, 0, prvOneShotTimerCallback);
  xAutoReloadTimer = xTimerCreate("AutoReload",mainAUTO_RELOAD_TIMER_PERIOD,pdTRUE, 0, prvAutoReloadTimerCallback);

  printf("system initialising ... \n\r");

  /* Init scheduler */
  vTaskStartScheduler();

  while(1)
  {
  }
}

void prvOneShotTimerCallback(TimerHandle_t xTimer)
{
	static TickType_t xTimerNow;

	//obtain the current tick count
	xTimerNow = xTaskGetTickCount();
	printf("One shot timer callback executing: %d \n\r", (int)xTimerNow);

}

uint32_t timeout_count =0 ;
const uint32_t STOP_MARK =10;
void prvAutoReloadTimerCallback(TimerHandle_t xTimer)
{
	static TickType_t xTimerNow;

	//obtain the current tick count
	xTimerNow = xTaskGetTickCount();
	printf("Auto reload timer callback executing: %d \n\r", (int)xTimerNow);
	timeout_count++;
	if(timeout_count == STOP_MARK)
	{
		printf("STOP MARK REACHED... %d \n\r", (int)xTimerNow);
		xTimerStop(xAutoReloadTimer, 0);
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
