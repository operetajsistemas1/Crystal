/*
 * PWM.c
 *
 * Created: 27.05.2017 21:40:22
 *  Author: Andris
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include "PWM.h"
#include "ADC.h"
#include "stdutils.h"
#include <math.h>
#include <stdlib.h>
#include "uart.h"

volatile uint16_t PWM_Treshhold = 136;
const uint16_t fiveVoltADCValue = 480;
volatile uint16_t PWM_Value = 480;

void PWM_Init()
{
   /*
   TCCR0 - Timer Counter Control Register (TIMER0)
   -----------------------------------------------
   BITS DESCRIPTION
   

   NO:   NAME   DESCRIPTION
   --------------------------
   BIT 7 : FOC0   Force Output Compare [Not used in this example]
   BIT 6 : WGM00  Wave form generartion mode [SET to 1]
   BIT 5 : COM01  Compare Output Mode        [SET to 1]
   BIT 4 : COM00  Compare Output Mode        [SET to 0]

   BIT 3 : WGM01  Wave form generation mode [SET to 1]
   BIT 2 : CS02   Clock Select               [SET to 0]
   BIT 1 : CS01   Clock Select               [SET to 0]
   BIT 0 : CS00   Clock Select               [SET to 1]

   The above settings are for
   --------------------------

   Timer Clock = CPU Clock (No Prescalling)
   Mode        = Fast PWM
   PWM Output  = Non Inverted

   */


   TCCR0|=(1<<WGM00)|(1<<WGM01)|(3<<COM00)|(1<<CS00);

   //Set OC0 PIN as output. It is  PB3 on ATmega16 ATmega32

   DDRB|=(1<<PINB3);
   OCR0=PWM_Treshhold;   // init duty to 117
}

/******************************************************************
Sets the duty cycle of output. 

Arguments
---------
duty: Between 0 - 255

0= 0%

255= 100%

The Function sets the duty cycle of pwm output generated on OC0 PIN
The average voltage on this output pin will be

         duty
 Vout=  ------ x 5v
         255 

This can be used to control the brightness of LED or Speed of Motor.
*********************************************************************/

void PWM_Set()
{	
	static uint8_t NEG_Counter = 0;
	static uint8_t POZ_Counter = 0;
	if (PWM_Value > (fiveVoltADCValue+8)) {
		POZ_Counter++;
		NEG_Counter = 0;
		if (POZ_Counter > 3){
			PWM_Treshhold++;
			POZ_Counter = 0;
			OCR0=PWM_Treshhold;	
		}
	} else if (PWM_Value < (fiveVoltADCValue-8)) {
		POZ_Counter = 0;
		NEG_Counter++;
		if (NEG_Counter > 3){
			PWM_Treshhold--;
			NEG_Counter = 0;
			OCR0=PWM_Treshhold;	
		}
	}		
}

