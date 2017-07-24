/*
 * main.c
 *
 * Created: 23.05.2017 01:34:50
 *  Author: Andris
 */ 

#define F_CPU 16000000UL  // 16 MHz

#include "gpio.h"
#include "config.h"
//#include <avr/io.h>
#include <util/delay.h>
#include "SNx4HC595.h"
#include "glcd.h"	//User defined LCD library which conatins the lcd routines
#include <avr/iom32a.h>
//#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC.h"
#include "PWM.h"
#include "temperature.h"
#include "buttons.h"
#include "uart.h"
#include "conductivity.h"
#include "timer.h"
#include "state.h"
#include "MenuTree.h"
#include "eeprom.h"
#include <util/atomic.h>







volatile RELAY_FLAGS Relay_Flags = {.flags = 0b0000000000000010};  /* Allocation for the Flags */
volatile char ReceivedByte = '0';
extern volatile TEMPERATURE temperature; 
extern volatile uint32_t timer2_counter;
volatile uint32_t	tick_count;
extern volatile COND Conductivity;
extern volatile uint8_t ticker; 
extern volatile uint32_t countdown_timer;
extern volatile uint32_t transition_timer;
extern volatile STATES State;
uint8_t MENU_SCREEN = 0;
volatile uint8_t Use_MOhm = 1;
extern volatile uint16_t timer_temp;
volatile uint8_t Calibration_Available = 1;
volatile uint8_t Calibration_Running = 0;


int main(){

	//Initialize HC595 system 
	//	Relay_Flags.Cond1_Relay =0;
	//Relay_Flags.flags = 0b1111110000000111;
	sei();	
	HC595_Init();
	GLCD_Init();
	PWM_Init();
	ADC_Init();
	UART_Init();	
	COND_Init();
	TIMER_Init();
	STATE_Init();
	EEPROM_Init();
	HC595_Write(Relay_Flags.flags);	
	
	
	/*
		Loop for temperature calibration
	*/
	for (uint16_t iter = 0; iter < 500; iter++ ){
		BTN_Check();
	}	
	Calibration_Available = 0;	
	while(Calibration_Running) {   // another thread for calibration procedure
			BTN_Check();
		if (ticker) {	
			ticker = 0;					
			MENU_Process(0);
		}		
	}		
	
	
						
	while(1) {
		BTN_Check(); //Check button input
		//this will happen every second
		if (ticker) {
			ticker = 0;
			STATE_Check();    //check state
			if (!MENU_SCREEN) {
				MENU_Status_Header2();    
				MENU_Status();
			}	
			// LED heartbeat		
			Relay_Flags.Relay220_2 ^= 1;
			HC595_Write(Relay_Flags.flags);	
			

			
		}



	}
	
return 0;
}


