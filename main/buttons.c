/*
 * buttons.c
 *
 * Created: 29.05.2017 18:58:43
 *  Author: Andris
 */ 

#define F_CPU 16000000UL  // 16 MHz
#include "buttons.h"
#include <avr/io.h>
#include <util/delay.h>
#include "conductivity.h"
#include "state.h"
#include "glcd.h"
#include "MenuTree.h"
#include "config.h"

extern volatile STATES State;
extern volatile COND  Conductivity;
volatile uint8_t currentPortState;
extern uint8_t MENU_SCREEN;
volatile BTN buttons = {.ticks=0, .BTN_Active = 0};
extern	volatile uint8_t Calibration_Available;
extern tree_node calibrate;
extern tree_node * tree_node_selected;
//extern volatile uint8_t Calibration_Running;

uint8_t static BTN_Get_Port_State(){
		// save Display data port configurations
	volatile uint8_t portData = PORTC;
	volatile uint8_t portControl = DDRC;
// 	if (portData != 0){
// 		printf("%u",'a'); 
// 		return 0; //if there is data on display port, skip button checking
// 	}	 
// 	printf("%u",PORTC); 
	//set keypad common to input;
	DDRB &= ~(1<<PINB0);
	PORTB &= ~(1<<PINB0);
	//set display data pins to input pullup
	DDRC = 0x00;
	PORTC = 0xFF;
	// if display tries to communicate, restor settings and skip button checking
// 	if (PINC) {
// 		PORTC = portData;
// 		DDRC = portControl;
// 		return; //if there is data on display port, skip button checking
// 	}	
	//set keypad common to zero, output
	DDRB |= (1<<PINB0);
	PORTB &= ~(1<<PINB0);	
	_delay_ms(1); // wait for portc to change state between reading;
	currentPortState = ~PINC;
	//set displey pins to old state
	PORTC = portData;
	DDRC = portControl;
	return currentPortState;

}


uint8_t static BTN_Debounce(BTN *btn, uint8_t currentPortState){
	if ((btn->BTN_Active ^ currentPortState)&&(btn->ticks >= BTN_SAFE_DEBOUNCE)){
		btn->BTN_Active = currentPortState;
		btn->ticks = 0;
		if (btn->BTN_Active) return btn->BTN_Active;
		
	} else if ((btn->BTN_Active ^ currentPortState)&&(btn->ticks < BTN_SAFE_DEBOUNCE)) {
		btn->ticks++;

	} else if (btn->BTN_Active == currentPortState) btn->ticks = 0;
	return 0;
}



		
void BTN_Check(){
	volatile uint8_t active_buttons = 0;
	currentPortState = BTN_Get_Port_State();
	//UART_Transmit(currentPortState);
	active_buttons = BTN_Debounce(&buttons, currentPortState);
//	UART_Transmit(active_buttons);

	switch(active_buttons) {
		case 0  :
	//		printf("%d  1 \r\n",active_buttons); 
		break; 
			
		case RUN  :
			if (State == OFF){
				State = StandBy;
				STATE_Set();			// only state that we set trough button since we check for device status	
			} else {
				State = OFF;
				STATE_Set();		
				//printf("off  \r\n");		
			}

		break; 

		case PUMPOUT  :
			//printf("%d 3  \r\n",active_buttons); 
#ifdef _ULTRAPURE
			 //   printf("ultrapure \r\n"); 
			if (!MENU_SCREEN){
			  //  printf("dispensing \r\n"); 
				if (State == Dispensing) {
					State = StandBy;
					STATE_Set();					
				} else if (State != OFF){
					State = Dispensing;
					STATE_Set();
				}				
			}
#endif //_ULTRAPURE			
#ifdef _PURE
			if (!MENU_SCREEN){
				State = PUMPOUT;
				STATE_Set();
			}
#endif //_PURE		
			
		break; 
				
		case MENU  :
			//printf("%d 4  \r\n",active_buttons); 
			if (MENU_SCREEN){
				MENU_Out();
			} else {
				MENU_SCREEN = 1;
				MENU_Draw();	
			}	

		break; 		
				
		case SLEEP  :
			if (Calibration_Available) {
				tree_node_selected = &calibrate;
				MENU_Process(0);
			}			
			printf("%d 5  \r\n",active_buttons); 
		break; 				
				
		case DOWN  :
			printf("%d  6 \r\n",active_buttons); 
			if (MENU_SCREEN){
				MENU_Down();
			}
		break; 		
				
		case OK  :
			printf("%d  7 \r\n",active_buttons); 
			if (MENU_SCREEN){
				MENU_In();
			}		
		break; 	
					
		case UP  :
//			printf("%d 8  \r\n",active_buttons); 
			if (MENU_SCREEN){
				MENU_Up();
			}			
		break; 			
		  
		default : /* Optional */
		printf("%d 9  \r\n",active_buttons); 
	}




}
		
		


	
	

