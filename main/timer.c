/*
 * timer.c
 *
 * Created: 08.06.2017 11:58:34
 *  Author: Andris
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
//

volatile uint32_t countdown_timer = 0;
volatile uint32_t transition_timer = 0;
volatile uint8_t ticker =0;

void TIMER_Init(){
	TCCR2 |= (1<<WGM21)|(1<<CS20)|(1<<CS21)|(1<<CS22);
	OCR2 |= 125;
	TIMSK |= (1<<OCIE2);

}



ISR(TIMER2_COMP_vect){
	static volatile uint8_t soft_prescaler = 0;
	if (soft_prescaler == 125){
		ticker = 1;
		if (countdown_timer) countdown_timer--;
		if (transition_timer) transition_timer--;	
		soft_prescaler =0;
	} else {
		soft_prescaler++;
	}
	

}
	
	
	