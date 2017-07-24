/*
 * ADC.c
 *
 * Created: 27.05.2017 21:38:57
 *  Author: Andris
 */ 
#include <avr/io.h>
#include "ADC.h"
#include "temperature.h"
//#include "temperature.c"
#include <avr/interrupt.h>
#include <avr/iom32a.h>
#include <util/atomic.h>
#include "uart.h"

extern struct TEMPERATURE temperature; 

void ADC_Init()
{
ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)); 
ADMUX = (1<<REFS0);                         // For Aref=AVcc;
ADCSRA |= (1<<ADEN)|(1<<ADIE); //Rrescalar div factor =128
//DDRA &= ~(1<<PINA0);
//DDRA &= ~(1<<PINA1);
ADCSRA |= (1<<ADSC);
}

ISR(ADC_vect)
{
	uint16_t ADCValue;;
	ATOMIC_BLOCK ( ATOMIC_RESTORESTATE ){
	ADCValue = ADCL | (ADCH << 8);
	}	
	temperature.temperatur = (1-0.1)*temperature.temperatur + 0.1*ADCValue;   //exponential moving average
	ADCSRA |= 1<<ADSC;
}