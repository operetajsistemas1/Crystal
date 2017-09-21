/*
 * buttons.c
 *
 * Created: 29.05.2017 18:58:43
 *  Author: Andris
  * buttons.c module checks input button state, performs debounce 
  * and starts a state function
 */ 

#define F_CPU 16000000UL  // 16 MHz
#include "config.h"
#include "buttons.h"
#include <avr/io.h>
#include <util/delay.h>
#include "conductivity.h"
#include "state.h"
#include "glcd.h"
#include "MenuTree.h"

extern volatile STATES State;
extern volatile COND  Conductivity;
volatile uint8_t currentPortState;
extern uint8_t MENU_SCREEN;
volatile BTN buttons = {.ticks=0, .BTN_Active = 0};
extern	volatile uint8_t Calibration_Available;
extern tree_node calibrate;
extern tree_node * tree_node_selected;

/*
 * BTN_Get_Port_State()
 * Method gets current button state from registers.
 * 
 */ 
uint8_t static BTN_Get_Port_State(){
	// save Display data port configurations
	volatile uint8_t portData = PORTC;
	volatile uint8_t portControl = DDRC;
	//set keypad common to input;
	DDRB &= ~(1<<PINB0);
	PORTB &= ~(1<<PINB0);
	//set display data pins to input pullup
	DDRC = 0x00;
	PORTC = 0xFF;
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

/*
 * BTN_Debounce()
 * Method used to debounce buttons. uses const
 * BTN_SAFE_DEBOUNCE for a treshold
 */ 

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



/*
 * BTN_Check()
 * Use this method to perform pressed button check and set state
 *
 */
void BTN_Check(){
	volatile uint8_t active_buttons = 0;
	currentPortState = BTN_Get_Port_State();
	active_buttons = BTN_Debounce(&buttons, currentPortState);
	if (Calibration_Available){
		active_buttons &= SLEEP;
	}
	switch(active_buttons) {
		case 0  :
		break; 
		case RUN  :
			if (State == OFF){
				State = StandBy;
				STATE_Set();			// only state that we set trough button since we check for device status	
			} else {
				State = OFF;
				STATE_Set();
			}

		break; 
		case PUMPOUT  :
#if ( defined(_ULTRAPURE) || defined(_CLINIC))
			if (!MENU_SCREEN){
				if (State == Dispensing) {
					State = StandBy;
					STATE_Set();
				} else if (State != OFF){
					State = Dispensing;
					STATE_Set();
				}
			}
#endif //_ULTRAPURE	_CLINIC
#ifdef _PURE
			if (!MENU_SCREEN){
				if (State == PUMPOUT) {
					State = StandBy;
					STATE_Set();
				} else if (State != OFF){
					State = PUMPOUT;
					STATE_Set();
				}
			}
#endif //_PURE
		break; 
				
		case MENU  :
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
		break; 
				
		case DOWN  :
			if (MENU_SCREEN){
				MENU_Down();
			}
		break; 
				
		case OK  :
			if (MENU_SCREEN){
				MENU_In();
			}		
		break; 	
					
		case UP  :

			if (MENU_SCREEN){
				MENU_Up();
			}	
			
#ifdef _ULTRAPURE
			if (!MENU_SCREEN){
				if (State == PUMPOUT) {
					State = StandBy;
					STATE_Set();
				} else if (State != OFF){
					State = PUMPOUT;
					STATE_Set();
				}
			}
#endif //_ULTRAPURE
		break; 
		default : /* Optional */
		break;
	}




}
		
		


	
	

