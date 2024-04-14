/*
 * uart.h
 *
 *  Created on: Mar 30, 2024
 *      Author: Vamshi Reddy Kotha
 */

#ifndef INC_UART_H_
#define INC_UART_H_


int __io_putchar(int ch);
void USART2_UART_TX_Init(void);
void USART2_UART_RX_Init(void);
int USART2_write(int ch);
char USART2_read(void);

#endif /* INC_UART_H_ */
