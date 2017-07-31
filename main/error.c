/*
 * error.c
 *
 * Created: 27.07.2017 21:23:02
 *  Author: Andris
 */ 
#include "error.h"
#include <avr/io.h>
#include "Conductivity.h"

extern volatile COND  Conductivity;
volatile uint8_t Error_Flag = 0;

void ERROR_Check(){
	static uint8_t active_error = 0;
	static error_cycle_timer = 0;
	error_cycle_timer++;
	if (error_cycle_timer > 5) {
		for (uint8_t i = active_error; sizeof(uint8_t); i++ )
			if (Error_Flag&&(1<<i)){
				printf("error %d", i);
				active_error = i;
				
			} else {
				active_error = 0;
			}
	error_cycle_timer = 0;	
	}
}

void ERROR_Check_Grade(){
	
	uint32_t resistivity = COND_Get_Kohm();
	if (resistivity < 2000){
		if (Conductivity.Current_Grade == 1){		
			Error_Flag |= (1 << Polishing_Error);   //Set polishing error	
		} else {
			Error_Flag |= (1 << DI_Error);   //Ser DI error
		}	
	}

}