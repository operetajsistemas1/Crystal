/*
 * gpio.c
 *
 * Created: 06.06.2017 22:00:53
 *  Author: Andris
 */ 
#include <avr/io.h>


GPIO_Init(){
	//PB6	(MISO)	Tank full
	//PB5	(MOSI)	Low Pressure
	DDRB &= ~((1<<PINB5)|(1<<PINB6));
	PORTB |= (1<<PINB5)|(1<<PINB6);

}

char Low_Pressure(){
//	printf ("LP: %d \r\n",PINB&(1<<5));
	return (PINB&(1<<5));
}

char Tank_Full(){
	return (PINB&(1<<PIND6));
}