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
#include "glcd.h"
#include <stdlib.h>

const float GRADE1_OFFSET = 38.208f;
const float GRADE1_SLOPE = 0.30501f;
const float GRADE2_OFFSET = 38.208f;
const float GRADE2_SLOPE = 0.30501f;
volatile uint16_t timer2_counter = 0;
volatile COND  Conductivity ={.Current_Grade = 1, .Timer_Reset_Pending=1, .Grade1 = 6271000, .Grade2 = 6271000};
volatile uint16_t timer_temp;
volatile uint8_t COND_Units = 0;
volatile uint8_t dirty_water_counter = 0;

void COND_Init(void){
	timer2_counter = 0;
	// External interrupt 0 for conductivity sensor Grade 2
	// External interrupt 1 for conductivity sensor Grade 1
	//Enable both edge detection for both interrupts
	if (Conductivity.Current_Grade == 0)	{
		GICR |= (1<<INT0);
		GICR &= ~(1<<INT1);
	} 
	else{
		GICR |= (1<<INT1);
		GICR &= ~(1<<INT0);
	}
	MCUCR |=(1<<ISC11)|(1<<ISC01);
	// Timer 1 noise cancellor
	// Timer 1 owerflow interrupr
	TCCR1A = 0;
	TCCR1B=(1<<CS12)|(0<<CS11)|(1<<CS10);
	TIMSK |= (1<<TOIE1);
}



ISR(TIMER1_OVF_vect){

	printf("Timer overflow [%"PRIu32"] \r\n",timer2_counter);

}




ISR(INT1_vect) // grade 1
{	
		ATOMIC_BLOCK ( ATOMIC_RESTORESTATE ){
			timer_temp =  TCNT1;
			TCNT1 = 0; 
			if (timer_temp > 200){  //ignore events shorter than 100 ticks for OpAmp contact bounce
				Conductivity.Grade1 = (0.1)*timer_temp + 0.9*Conductivity.Grade1;   //exponential moving average
				dirty_water_counter = 0;
			} else {
				dirty_water_counter++;
				if (dirty_water_counter > 100){
				//	printf("Dirty water [%"PRIu8"] \r\n",dirty_water_counter);
					dirty_water_counter = 0;
					Conductivity.Grade1 = 0;
				}
			}
		}
	GIFR |= (1<<INT1); // reset interrupt if it got set already
}	

ISR(INT0_vect) // grade 2
{	
		ATOMIC_BLOCK ( ATOMIC_RESTORESTATE ){
			timer_temp =  TCNT1;
			TCNT1 = 0; 
			if (timer_temp > 200){  //ignore events shorter than 100 ticks for OpAmp contact bounce
				Conductivity.Grade2 = (1-0.5)*Conductivity.Grade2 + 0.5*timer_temp;   //exponential moving average
				dirty_water_counter = 0;
			} else {
				dirty_water_counter++;
				if (dirty_water_counter > 100){
				//	printf("Dirty water [%"PRIu8"] \r\n",dirty_water_counter);
					dirty_water_counter = 0;
					Conductivity.Grade2 = 0;
				}
			}
		}
	GIFR |= (1<<INT0); // reset interrupt if it got set already
}	

void COND_Set_Grade1(){
	GICR &= ~(1<<INT0);	 // Disable ext interrupt 0
	Conductivity.Timer_Reset_Pending =1;
	Conductivity.Current_Grade = 1;
	GICR |= (1<<INT1); 
}

void COND_Set_Grade2(){
	GICR &= ~(1<<INT1);	 // Disable ext interrupt 0
	Conductivity.Timer_Reset_Pending =1;
	Conductivity.Current_Grade = 0;
	GICR |= (1<<INT0);
}

uint32_t COND_Get_Kohm(){
//		printf("G1_out %"PRIu16"  \r\n",Conductivity.Grade1);		
//	printf("Cur_grade %d \r\n",Conductivity.Current_Grade);	
	if (Conductivity.Current_Grade == 1){		
		float resist  = (float)Conductivity.Grade1 * GRADE1_SLOPE + GRADE1_OFFSET;	
		//GLCD_SetCursor(0,1,10);
		//GLCD_DisplayFloatNumber(resist);
		return resist;
	} else {
		float resist  = (float)Conductivity.Grade2 * GRADE2_SLOPE + GRADE2_OFFSET;	
	//	GLCD_SetCursor(0,1,10);
	//	GLCD_DisplayFloatNumber(resist);
		return (uint32_t)resist;		
	}		
}

uint32_t COND_Get_US(){
		volatile float conduct  = COND_Get_Kohm();
		conduct = 1000000 / conduct;
	//	GLCD_SetCursor(0,6,10);
	//	GLCD_DisplayFloatNumber(conduct);
		//printf("conduct:%d \r\n",(uint32_t)conduct );	
		return (uint32_t)conduct;		
}