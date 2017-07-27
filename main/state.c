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
const uint32_t minute = 60;    //seconds in minute
const uint32_t recirculation = 30 * minute; // rinsing(fixed 30 sec)
const uint32_t oxidising_time_limit = 1 * minute; // 1 min oxidizing(fixed 1min)
const uint32_t measure_toc_time_limit = 1 * minute; // TOC measurement time(max 1min, or peak detected)
const uint32_t predispense_time_limit = 3 * minute; // delay when starting dispense from standby
const uint32_t tank_pump_limit = 4 * minute; // limit tank pump operation to 4 minutes
extern volatile uint16_t Recirculation_Period;  //60 minutes
extern volatile uint16_t Recirculation_Time;  //20 minutes

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

