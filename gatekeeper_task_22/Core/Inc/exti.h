/*
 * exti.h
 *
 *  Created on: Mar 30, 2024
 *      Author: Vamshi Reddy Kotha
 */

#ifndef INC_EXTI_H_
#define INC_EXTI_H_

void p13_interrupt_init(void);
uint8_t read_digital_sensor(void);
void gpio_init(void);

#endif /* INC_EXTI_H_ */
