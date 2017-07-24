



#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <avr/delay.h>
#include <stdio.h> 
#include "uart.h" 
#include <avr/interrupt.h>
volatile extern char ReceivedByte;

void UART_Init(void) 
{
  //UART Init 
  UBRRH = (BAUD_PRESCALE >> 8); 
  UBRRL = BAUD_PRESCALE;// Load lower 8-bits into the low byte of the UBRR register
    UCSRB|= (1<<TXEN)|(1<<RXEN);                // enable receiver and transmitter
	UCSRB|= (1<<RXCIE);
    UCSRC|= (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);   // 8bit data format
 fdevopen(&UART_Put_Char, NULL);
}
  
 // function to send data
void UART_Transmit (unsigned char data)
{
    while (!(UCSRA & (1<<UDRE)));                // wait while register is free
    UDR = data;                                   // load data in the register
}
  
// function to receive data
unsigned char UART_Recieve (void)
{

    while(!(UCSRA) & (1<<RXC));                   // wait while data is being received
    return UDR;                               // return 8-bit data
}
 
int UART_Put_Char(char c, FILE *fd) 
{
  while (!(UCSRA & (1<<UDRE)));  // Wait for the transmit buffer to become empty. 
  UDR = c;                       // Write the character to the UART. 
  return 0; 
}



ISR(USART__RXC_vect)
{
   ReceivedByte = UDR; // Fetch the received byte value into the variable "ByteReceived"
}
