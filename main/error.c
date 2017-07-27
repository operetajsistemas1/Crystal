/*
 * error.c
 *
 * Created: 27.07.2017 21:23:02
 *  Author: Andris
 */ 
#include "error.h"
#include <avr/io.h>

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

void ERROR_Grade1(){
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