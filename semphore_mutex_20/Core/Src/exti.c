/*
 * exti.c
 *
 *  Created on: Mar 30, 2024
 *      Author: Vamshi Reddy Kotha
 */


#include "stm32f4xx_hal.h"


void p13_interrupt_init(void)
{
	//enable GPIOC clock
	RCC->AHB1ENR |= 4;

	//enable sysconfig clock(exti is part of sysconfig module)
	RCC->APB2ENR |= 0x4000;

	//configure PC13 for push button interrupt
	GPIOC->MODER &= ~(0x0C000000);

	//clear port selection for EXTI13
	SYSCFG->EXTICR[3] &= (~0x00F0);

	//select port C for EXTI13
	SYSCFG->EXTICR[3] &= (~0x00F0);

	//unmask EXTI13
	EXTI->IMR |= 0x2000;

	//select falling edge trigger
	EXTI->FTSR |= 0x2000;

	NVIC_SetPriority(EXTI15_10_IRQn, 6);

	NVIC_EnableIRQ(EXTI15_10_IRQn);

}

void gpio_init(void)
{
	//Enable GPIOC clock
	RCC->AHB1ENR |= 4;


}

uint8_t read_digital_sensor(void)
{
	if(GPIOC->IDR & 0x2000)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
