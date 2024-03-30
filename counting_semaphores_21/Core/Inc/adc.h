/*
 * adc.h
 *
 *  Created on: Mar 30, 2024
 *      Author: Vamshi Reddy Kotha
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include <stdint.h>

uint32_t read_analog_sensor(void);
void adc_init(void);


#endif /* INC_ADC_H_ */
