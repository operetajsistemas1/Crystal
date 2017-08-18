/*
 * conductivity.c
 *
 * Created: 04.06.2017 17:53:58
 *  Author: Andris
 */ 
#include "conductivity.h"
#include "config.h"
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <avr/delay.h>
#include "stdutils.h"
#include <math.h>
#include "glcd.h"
#include <stdlib.h>
#include "temperature.h"


extern uint8_t MENU_SCREEN;
#ifdef _ULTRAPURE
	const float GRADE1_OFFSET = -26.0f;
	const float GRADE1_SLOPE = 1.73f;
	const float GRADE2_OFFSET = 23.088f;
	const float GRADE2_SLOPE = 1.097f;
#endif //_ULTRAPURE
#ifdef _CLINIC
	const float GRADE1_OFFSET = 23.088f;   
	const float GRADE1_SLOPE = 1.097f;		
	const float GRADE2_OFFSET = 23.088f;	//FIXME
	const float GRADE2_SLOPE = 1.097f;		//FIXME
#endif //_CLINIC
#ifdef _RO
	const float GRADE2_OFFSET = 23.088f;	//FIXME
	const float GRADE2_SLOPE = 1.097f;		//FIXME
#endif //_RO
#if (defined(_PURE) || defined(_DOUBLEFLOW))
	const float GRADE2_OFFSET = 23.088f;	
	const float GRADE2_SLOPE = 1.097f;		
#endif //_PURE _DOUBLEFLOW

volatile uint16_t timer2_counter = 0;
volatile COND  Conductivity ={.Current_Grade = 1, .Timer_Reset_Pending=1, .Grade1 = 0, .Grade2 = 0, .Overflow = 1};
volatile uint16_t timer_temp;
volatile uint8_t COND_Units = 0;
volatile uint8_t dirty_water_counter = 0;

void COND_Init(void){
	timer2_counter = 0;
	// External interrupt 0 for conductivity sensor Grade 2
	// External interrupt 1 for conductivity sensor Grade 1
	COND_Set_Grade2();   //For RO units this is grade 3
	MCUCR |=(1<<ISC11)|(1<<ISC01);
	// Timer 1 noise cancellor
	// Timer 1 owerflow interrupr
	TCCR1A = 0;
	TCCR1B=(1<<CS12)|(0<<CS11)|(1<<CS10);
	TIMSK |= (1<<TOIE1);
}



ISR(TIMER1_OVF_vect){
	Conductivity.Overflow = TRUE;
	//printf("Timer overflow [%"PRIu32"] \r\n",timer2_counter);

}




ISR(INT1_vect) // grade 2
{	
		ATOMIC_BLOCK ( ATOMIC_RESTORESTATE ){
			timer_temp =  TCNT1;
			TCNT1 = 0; 
			
			if (timer_temp > 140){  //ignore events shorter than 100 ticks for OpAmp contact bounce
				//if (Conductivity.Timer_Reset_Pending){
					//Conductivity.Timer_Reset_Pending--;
					//GIFR |= (1<<INT1); // reset interrupt if it got set already
					//return;
				//}
				Conductivity.Grade2 = (0.1)*timer_temp + 0.9*Conductivity.Grade2;   //exponential moving average
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
		Conductivity.Overflow = FALSE;
	GIFR |= (1<<INT1); // reset interrupt if it got set already
}	

ISR(INT0_vect) // grade 1
{	

		ATOMIC_BLOCK ( ATOMIC_RESTORESTATE ){
			timer_temp =  TCNT1;
			TCNT1 = 0; 
			if (timer_temp > 140){  //ignore events shorter than 100 ticks for OpAmp contact bounce
				//if (Conductivity.Timer_Reset_Pending){
					//Conductivity.Timer_Reset_Pending--;
					//GIFR |= (1<<INT0); // reset interrupt if it got set already
					//return;
				//}
			//	printf("Grade1:  [%"PRIu16"] \r\n",timer_temp);
				Conductivity.Grade1 = (0.3)*Conductivity.Grade1 + 0.7*timer_temp;   //exponential moving average
				dirty_water_counter = 0;
			} else {
				dirty_water_counter++;
				if (dirty_water_counter > 100){
					dirty_water_counter = 0;
					Conductivity.Grade1 = 0;
				}
			}
		}
	Conductivity.Overflow = FALSE;
	GIFR |= (1<<INT0); // reset interrupt if it got set already
}	

void COND_Set_Grade1(){
	static uint8_t first_run = 1;
	GICR &= ~(1<<INT1);	 // Disable ext interrupt 0
	GICR |= (1<<INT0); 
	if (!first_run){
		Conductivity.Timer_Reset_Pending =7;
	}	
	first_run = 0;
	Conductivity.Current_Grade = 1;
	if (!MENU_SCREEN) {
#if  defined(_ULTRAPURE) || defined(_PURE)
			GLCD_SetCursor(0,3,33);
			GLCD_DisplayChar('1');	
#endif // _ULTRAPURE +PURE
#ifdef _CLINIC
			GLCD_SetCursor(0,3,33);
			GLCD_DisplayChar('2');	
#endif // _CLINIC
			GLCD_SetCursor(0,4,30);
			GLCD_DisplayChar(' ');	
	}
}

void COND_Set_Grade2(){
	static uint8_t first_run2 = 1;
	GICR &= ~(1<<INT0);	 // Disable ext interrupt 0
	GICR |= (1<<INT1);	
	if (!first_run2){
	Conductivity.Timer_Reset_Pending =7;
	}
	first_run2 = 0;		
	Conductivity.Current_Grade = 0;
	if (!MENU_SCREEN) {	
			GLCD_SetCursor(0,3,33);
			GLCD_DisplayChar(' ');	
#if (defined(_ULTRAPURE) || defined(_PURE) ||defined(_DOUBLEFLOW))
			GLCD_SetCursor(0,4,30);
			GLCD_DisplayChar('2');	
#endif // _ULTRAPURE +PURE
#if defined(_CLINIC) || defined(_RO)
			GLCD_SetCursor(0,4,30);
			GLCD_DisplayChar('3');	
#endif // _CLINIC _RO
	}
}

uint32_t COND_Get_Kohm(){	
	float resist = 0;
	if (Conductivity.Current_Grade == 1){	
		GLCD_SetCursor(0,1,10);
		GLCD_DisplayDecimalNumber(Conductivity.Grade1,6);
#if defined(_ULTRAPURE) || defined(_CLINIC)
		if ((!Conductivity.Timer_Reset_Pending) & ((Conductivity.Grade1/16) < (abs(Conductivity.Grade1 - Conductivity.Grade1_Saved)))){
			resist  = (float)Conductivity.Grade1 * GRADE1_SLOPE + GRADE1_OFFSET;	
			Conductivity.Grade1_Saved = Conductivity.Grade1;
		}			
		else {
			if (Conductivity.Timer_Reset_Pending) Conductivity.Timer_Reset_Pending--;
			resist  = (float)Conductivity.Grade1_Saved * GRADE1_SLOPE + GRADE1_OFFSET;
		}
		resist = resist * 18180 / Temperature_Compensate();   //temperature compensation for grade I
#endif // ULTRAPURE || CLINIC
	} else {
		GLCD_SetCursor(0,1,10);
		GLCD_DisplayDecimalNumber(Conductivity.Grade2,6);
		if ((!Conductivity.Timer_Reset_Pending) & ((Conductivity.Grade2/16) < (abs(Conductivity.Grade2 - Conductivity.Grade2_Saved)))) {
			resist  = (float)Conductivity.Grade2 * GRADE2_SLOPE + GRADE2_OFFSET;
			Conductivity.Grade2_Saved = Conductivity.Grade2;
		}else {
			if (Conductivity.Timer_Reset_Pending) Conductivity.Timer_Reset_Pending--;
			resist  = (float)Conductivity.Grade2_Saved * GRADE2_SLOPE + GRADE2_OFFSET;
		}
	}			

//	printf("comp out %"PRIu32"  \r\n",Temperature_Compensate());
//	printf("resist %"PRIu32"  \r\n",resist);	
	return resist;		
}

uint32_t COND_Get_US(){
		volatile float conduct  = COND_Get_Kohm();
		conduct = 1000000 / conduct;
	//	printf("conduct:%d \r\n",(uint32_t)conduct );	
		return (uint32_t)conduct;		
}