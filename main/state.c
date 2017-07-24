/*
 * state.c
 *
 * Created: 07.06.2017 17:40:19
 *  Author: Andris
 */ 
#include "state.h"
#include "glcd.h"
#include "SNx4HC595.h"
#include "MenuTree.h"

volatile STATES State = OFF;
extern volatile RELAY_FLAGS Relay_Flags;

void STATE_Init(){
	GLCD_Clear();
	State = OFF;
	MENU_Status_Header();
	
}

void STATE_Check(){
	//printf("%u \r\n", State); 		
	switch (State){
		case OFF:
			//do nothing
		break;
		case StandBy:
			if (Tank_Full()){
				State = TankFull;
				STATE_Set();
			} else if (Low_Pressure()){
				State = LowPress;
				STATE_Set();
			} else {
				State = Running;			
				STATE_Set();
			}			
		break;		
		case Running:
			if (Tank_Full()){
				State = TankFull; 
				STATE_Set();
			} else if (Low_Pressure()){
				State = LowPress; 
				STATE_Set();
			}
			//check timers
		break;
		case Recirculation:
			//check timers
		break;
		case Rinsing:
			//check timers
		break;
		case Dispensing:
			
		break;
		case LowPress:
			if (!Low_Pressure()){
				State = Running;			
				STATE_Set();
			}		
		break;
		case TankFull:
			//probably have to add transition timer 
			if (!Tank_Full()){
				State = Running;			
				STATE_Set();
			}			
		break;		
		case TankPump:

		break;		
		default:
		
		break;		
	}
	
};



void STATE_Set(){
	//printf("%u \r\n", State); 		
	switch (State){
		case OFF:
			HC595_Clear_Output();
			#ifdef DEBUGG
				printf("OFF \r\n"); 		
			#endif	
			MENU_Status_Header();
		break;
		case StandBy:
			MENU_Status_Header();		
			HC595_Clear_Output();	

		break;		
		case Running:
			HC595_Clear_Output();	
			Relay_Flags.Input_Valve = 1;   // inpu valve
			Relay_Flags.Boost_Pump = 1;  //need to set RO pump here
			Relay_Flags.Sterilization = 1; //Sterilization lamp
			MENU_Status_Header();	
		break;
		case Rinsing:
			HC595_Clear_Output();	
			Relay_Flags.Rec_Pump = 1;
			Relay_Flags.Photoxidation = 1;		
			MENU_Status_Header();	
			// set timers
		break;		
		case Recirculation:
			HC595_Clear_Output();	
			Relay_Flags.Rec_Pump = 1;
			Relay_Flags.Photoxidation = 1;		
			MENU_Status_Header();	
			//set timers	
		break;
		case Dispensing:
			HC595_Clear_Output();	
			Relay_Flags.Rec_Pump = 1;		
			Relay_Flags.Dispense_Valve = 1;
			MENU_Status_Header();		
		break;
		case LowPress:
			HC595_Clear_Output();	
			Relay_Flags.Input_Valve = 1;   // inpu valve						
			MENU_Status_Header();
		break;
		case TankFull:
			HC595_Clear_Output();	
			MENU_Status_Header();
		break;	
		case TankPump:
			HC595_Clear_Output();	
			Relay_Flags.Tank_Pump = 10;				
			MENU_Status_Header();
		break;		
		default:
		
		break;		
	}
	
};

