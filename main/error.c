/*
 * error.c
 *
 * Created: 27.07.2017 21:23:02
 *  Author: Andris
 */ 
#include "error.h"
#include <avr/io.h>
#include "Conductivity.h"
#include "config.h"

extern volatile COND  Conductivity;
volatile uint8_t Error_Flag = 0;

const char LCD_Error[3][10] = {
		{'D','I',' ','e','r','r',' ',' ',' ',0},
		{'P','o','l','i','s','h','i','n','g',0},
		{'F','i','l','t','e','r',' ',' ',' ',0}
		};

void ERROR_Check(){
	static uint8_t active_error = 0;
	static error_cycle_timer = 0;
	error_cycle_timer++;
	if (error_cycle_timer > 5) {
		uint8_t i = ++active_error;
		while (!(Error_Flag&(1<<i))) {
			i++;
			if (i>8) i = 0;
		}		
		active_error = i;
		GLCD_GoToLine(1);
		GLCD_DisplayString(LCD_Error[i]);	
		error_cycle_timer = 0;	
	}
}

void ERROR_Check_Grade(){
	
	uint32_t resistivity = COND_Get_Kohm();
	if (resistivity < 2000){
		if (Conductivity.Current_Grade == 1){	
#ifdef _ULTRAPURE
			Error_Flag |= (1 << Polishing_Error);   //Set polishing error	
#endif // _ULTRAPURE
#ifdef _CLINIC
			Error_Flag |= (1 << DI_Error);   //Set polishing error	
#endif // _CLINIC
#if (defined(_PURE)||defined(_ULTRAPURE)||defined(_DOUBLEFLOW))
		} else {
			Error_Flag |= (1 << DI_Error);   //Ser DI error
#endif // _PURE _ULTRAPURE _DOUBLEFLOW
		}	
	}

}