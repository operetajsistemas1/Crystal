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

extern volatile struct TEMPERATURE temperature; 
extern volatile uint16_t PWM_Value;

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
	switch (ADMUX){
	case 0x40:
		ADMUX = 0x41;
		temperature.temperatur = (1-0.1)*temperature.temperatur + 0.1*ADCValue;   //exponential moving average
		break;
	case 0x41:
		ADMUX = 0x40;
		PWM_Value = 0.5*PWM_Value + 0.5*ADCValue;    //(1-0.2)*temperature.temperatur + 0.2*ADCValue;
		break;
	default:
		//Default code
		break;
	}

	ADCSRA |= 1<<ADSC;
}

uint16_t ADC_Read(uint8_t ch)
{
   ADMUX = (ADMUX & 0xE0) | (ch & 0x1F);   //select channel (MUX0-4 bits)
  
   //Start Single conversion
   ADCSRA|=(1<<ADSC);

   //Wait for conversion to complete
   while(!(ADCSRA & (1<<ADIF)));

   //Clear ADIF by writing one to it
   //Note you may be wondering why we have write one to clear it
   //This is standard way of clearing bits in io as said in datasheets.
   //The code writes '1' but it result in setting bit to '0' !!!

   ADCSRA|=(1<<ADIF);

   return(ADC);
}