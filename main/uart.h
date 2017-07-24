
#ifndef UART_H 
#define UART_H 
#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <stdio.h> 
#include "config.h"

#define F_CPU 16000000UL  //
#define USART_BAUDRATE 9600UL 
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)



//No need to call this function directly, it's just here for printf(). 
//Outputs a single character to the UART when the transmit buffer is empty. 
//This is the function that we bind to printf so it can output the string 
//that printf generates. The FILE parameter is required by printf for behind- 
//the-scenes bookkeeping. *Note that we can write a function that will put a 
//single char anywhere we want -- parallel interface, SPI, I2C, etc.* 
void UART_Transmit (unsigned char);
unsigned char UART_Recieve (void);
void UART_Init(void); 
int UART_Put_Char(char c, FILE *fd);
//Set up the registers that control the UART and bind put_uart_char 
//as our output function. 

#endif