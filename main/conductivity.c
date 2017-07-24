/*
 * conductivity.c
 *
 * Created: 04.06.2017 17:53:58
 *  Author: Andris
 */ 
#include "conductivity.h"
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <avr/delay.h>
#include "stdutils.h"
#include <math.h>
volatile uint16_t timer2_counter = 0;
volatile COND  Conductivity ={.Current_Grade = 0, .Timer_Reset_Pending=1, .Grade1 = 6271000, .Grade2 = 6271000};
volatile uint16_t timer_temp;
volatile uint8_t COND_Units = 0;

void COND_Init(void){
	timer2_counter = 0;
	// External interrupt 0 for conductivity sensor Grade 2
	// External interrupt 1 for conductivity sensor Grade 1
	//Enable both edge detection for both interrupts
	if (Conductivity.Current_Grade = 0)	{
		GICR |= (1<<INT0);
		GICR &= ~(1<<INT1);
	} 
	else{
		GICR |= (1<<INT1);
		GICR &= ~(1<<INT0);
	}
	MCUCR |=(1<<ISC11)|(1<<ISC01);
	// Timer 1 noise candellor
	// Timer 1 owerflow interrupr
	TCCR1A = 0;
	TCCR1B=(1<<CS12)|(0<<CS10);
	TIMSK |= (1<<TOIE1);
}



ISR(TIMER1_OVF_vect){

	printf("Timer overflow [%"PRIu32"] \r\n",timer2_counter);

}




ISR(INT1_vect) // grade 1
{	
	cli();
	if (Conductivity.Timer_Reset_Pending == 1){
		Conductivity.Timer_Reset_Pending =0;
		TCNT1 = 0;		
	} else {	
		timer_temp =  TCNT1  ;
		TCNT1 = 0; //we do this first so we do not get race conditions
//		printf("c1 %"PRIu16" \r\n",timer_temp);	
		Conductivity.Grade1 = (1-0.5)*Conductivity.Grade1 + 0.5*timer_temp;   //exponential moving average
//		printf("e1 %"PRIu16" \r\n",Conductivity.Grade1);	
	}	
	GIFR |= (1<<INT1);
	sei();
}	

ISR(INT0_vect) // grade 2
{
	if (Conductivity.Timer_Reset_Pending == 1){
		Conductivity.Timer_Reset_Pending =0;
		timer2_counter = 0;	
		TCNT1 = 0;
	} else {	
		ATOMIC_BLOCK ( ATOMIC_RESTORESTATE ){
			timer_temp =  TCNT1  ;
			TCNT1 = 0; //we do this first so we do not get race conditions
			timer_temp += timer2_counter;
	//		printf("[%"PRIu32" ] \r\n",timer2_counter);	
			timer2_counter = 0;
//					printf("c2 %"PRIu16" \r\n",timer_temp);	
		}
		Conductivity.Grade2 = (1-0.1)*Conductivity.Grade2 + 0.1*timer_temp;   //exponential moving average
	}	
	GIFR |= (1<<INT1);
}	

void COND_Set_Grade1(COND *Conductivity){
	GICR &= ~(1<<INT0);	 // Disable ext interrupt 0
	Conductivity->Timer_Reset_Pending =1;
	Conductivity->Current_Grade = 1;
	GICR |= (1<<INT1);
}

void COND_Set_Grade2(COND *Conductivity){
	GICR &= ~(1<<INT1);	 // Disable ext interrupt 0
	Conductivity->Timer_Reset_Pending =1;
	Conductivity->Current_Grade = 0;
	GICR |= (1<<INT0);
}