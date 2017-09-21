/*
 * ADC.c
 * 
 * Created: 27.05.2017 21:38:57
 *  Author: Andris
 *
 * ADC.c module initializes two channels:
 *		ch0 - temperature reading. Performs exponential moving average 
 *			and stores in temperature structure.
 *		ch1 - -5V reading. PErforms exponential moving average and stores
 *			ing global variable
 *
 */ 
#include <avr/io.h>
#include "ADC.h"
#include "temperature.h"
#include <avr/interrupt.h>
#include <avr/iom32a.h>
#include <util/atomic.h>
#include "uart.h"

extern volatile struct TEMPERATURE temperature; 
extern volatile uint16_t PWM_Value;

/*
 * ADC_Init()
 * Method to initialize ADC module
 *
 */ 

void ADC_Init() {
	ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)); 
	ADMUX = (1<<REFS0);                         // For Aref=AVcc;
	ADCSRA |= (1<<ADEN)|(1<<ADIE);				//Rrescalar div factor =128
	ADCSRA |= (1<<ADSC);
}


/*
 * ADC_ vect IST
 * Interrupt method to read values from ADC module
 *
 */ 

ISR(ADC_vect)
{
	uint16_t ADCValue;;
	ATOMIC_BLOCK ( ATOMIC_RESTORESTATE ){
		ADCValue = ADCL | (ADCH << 8);
	}	
	switch (ADMUX){
	case 0x40:
		ADMUX = 0x41;
		temperature.temperatur = (1-0.2)*temperature.temperatur + 0.2*ADCValue;   //exponential moving average
		break;
	case 0x41:
		ADMUX = 0x40;
		PWM_Value = 0.5*PWM_Value + 0.5*ADCValue;  
		break;
	default:
		//Default code
		break;
	}
	ADCSRA |= 1<<ADSC;
}
