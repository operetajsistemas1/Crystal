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
#include "error.h"

volatile STATES State = OFF;
extern volatile RELAY_FLAGS Relay_Flags;
extern volatile uint16_t Recirculation_Period;  //60 minutes
extern volatile uint16_t Recirculation_Time;  //20 minutes
const uint16_t ERROR = 1 * 60;  // check conductivity every 
volatile uint16_t tank_pump_time;
volatile uint32_t phase_timer;
volatile uint32_t transition_timer;
volatile uint32_t error_timer;

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
					printf ("LP:stendby state check \r\n");
			if (Tank_Full()){
				State = TankFull;
				STATE_Set();
			} else if (Low_Pressure()){
								printf ("LP: standby state check \r\n");
				State = LowPress;
				STATE_Set();
			} else {
				State = Running;			
				STATE_Set();
			}			
		break;		
		case Running:
			printf ("LP:running state check \r\n");
			if (Tank_Full()){
				State = TankFull; 
				STATE_Set();
			} else if (Low_Pressure()){
				printf ("LP: running state check \r\n");
				State = LowPress; 
				STATE_Set();
			}
			if (!phase_timer) {
				State = Recirculation;
				STATE_Set();
			}
			if (!error_timer) {
				ERROR_Check_Grade();
			}
		break;
		case Recirculation:
			if (!phase_timer) {
				State = StandBy;
				STATE_Set();
			}
			if (!error_timer) {
				ERROR_Check_Grade();
			}
		break;
		//case Rinsing:
			////check timers
		//break;
		case Dispensing:
	//		printf(" state dispensing \r\n"); 	
		break;
		case LowPress:
			printf ("LP:lowpress state check \r\n");
			if (!Low_Pressure()){
				printf ("LP: low press state check \r\n");
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
			if (!phase_timer){
				State = StandBy;
				STATE_Set();
			}
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
			phase_timer = Recirculation_Period -Recirculation_Time;
			HC595_Clear_Output();	
			Relay_Flags.Input_Valve = 1;   // inpu valve
			Relay_Flags.Boost_Pump = 1;  //need to set RO pump here
			Relay_Flags.Sterilization = 1; //Sterilization lamp
			MENU_Status_Header();	
			error_timer = ERROR;
		break;		
		case Recirculation:
			HC595_Clear_Output();	
			Relay_Flags.Rec_Pump = 1;
			Relay_Flags.Photoxidation = 1;		
			MENU_Status_Header();
			phase_timer = Recirculation_Time;	
			error_timer = ERROR;
		break;
		case Dispensing:
			HC595_Clear_Output();	
			Relay_Flags.Rec_Pump = 1;		
			Relay_Flags.Dispense_Valve = 1;
			MENU_Status_Header();	
		//	printf("set state dispensing \r\n"); 	
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
			phase_timer = tank_pump_time;
		break;		
		default:
		
		break;		
	}
	
};

