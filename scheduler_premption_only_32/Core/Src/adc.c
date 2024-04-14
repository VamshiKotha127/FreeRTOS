/*
 * adc.c
 *
 *  Created on: Mar 30, 2024
 *      Author: Vamshi Reddy Kotha
 */

#include "adc.h"
#include "stm32f4xx_hal.h"

void adc_init(void)
{
	//enable clock access
	RCC->AHB1ENR |= (1U << 0);//0x00 00 00 01 (enable clock for GPIOA)
	RCC->APB2ENR |= (1U << 8); //enable clock for ADC1

	GPIOA->MODER |= 0xC; //set PA1 as analog

	ADC1->CR2 = 0; //trigger using software
	ADC1->SQR3 = 1; //conversion sequence starts at ch 1
	ADC1->SQR1 = 0; //conversion sequence length 1
	ADC1->CR2 |= 1; //ADC1
}


uint32_t read_analog_sensor(void)
{
	ADC1->CR2 |= (1U << 30); //start conversion

	while(!(ADC1->SR & 2)){} //wait for conversion to complete

	return ADC1->DR; //return results
}

