/*
 * state.c
 *
 * Created: 07.06.2017 17:40:19
 *  Author: Andris
 */ 
#include "state.h"
#include "config.h"
#include "glcd.h"
#include "SNx4HC595.h"
#include "MenuTree.h"
#include "error.h"
#include "gpio.h"

volatile STATES State = OFF;
extern volatile RELAY_FLAGS Relay_Flags;
#ifdef _ULTRAPURE
extern volatile uint16_t Recirculation_Period;  //60 minutes
extern volatile uint16_t Recirculation_Time;  //20 minutes
#endif //_ULTRAPURE
const uint16_t ERROR = 5 * 60;  // check conductivity every 
const uint16_t OVERFILL = 30;  // check conductivity every 
volatile uint16_t tank_pump_time;
volatile uint32_t phase_timer;
volatile uint32_t transition_timer;
volatile uint32_t error_timer;

void STATE_Init(){
	GLCD_Clear();
	State = OFF;
	MENU_Status_Header(State);
	
}

void STATE_Check(){	
	switch (State){
		case OFF:
		break;
		case StandBy:
			if (Tank_Full()){
#ifdef _ULTRAPURE
				phase_timer = Recirculation_Period - Recirculation_Time;
				State = TankFull;
				STATE_Set();
#endif //_ULTRAPURE
			// I dont think low pressure check in standby is neccassary
			//} else if (Low_Pressure()){
				//State = LowPress;
				//STATE_Set();
			} else {
				State = Running;			
				STATE_Set();
			}	
#ifdef _CLINIC
			if (Grade2Flow()){
				COND_Set_Grade1();
				MENU_Status_Header(Dispensing);
			} else {
				COND_Set_Grade2();
				MENU_Status_Header(StandBy);				
			}	
#endif //_CLINIC
		break;		
		case Running:
			if (Tank_Full()){
				transition_timer = OVERFILL;
				State = PostFill; 
				STATE_Set();
			} else if (Low_Pressure()){
				State = LowPress; 
				STATE_Set();
			}
#ifdef _ULTRAPURE
			if (!phase_timer) {
				State = Recirculation;
				STATE_Set();
			}
#endif //_ULTRAPURE
			if (!error_timer) {
				ERROR_Check_Grade();
			}
#ifdef _CLINIC
			if (Grade2Flow()){
				COND_Set_Grade1();
				MENU_Status_Header(Dispensing);
			} else {
				COND_Set_Grade2();
				MENU_Status_Header(Running);				
			}	
#endif //_CLINIC
		break;
#ifdef _ULTRAPURE
		case Recirculation:
			if (!phase_timer) {
				State = StandBy;
				STATE_Set();
			}
			if (!error_timer) {
				ERROR_Check_Grade();
			}
		break;
#endif //_ULTRAPURE
		case Dispensing:
		break;
		case LowPress:
			if (!Low_Pressure()){
				State = Running;			
				STATE_Set();
			}	
#ifdef _ULTRAPURE
			if (Tank_Full()){
				phase_timer = Recirculation_Period - Recirculation_Time;
				State = TankFull;
				STATE_Set();
			}			
#endif //_ULTRAPURE
#ifdef _CLINIC
			if (Grade2Flow()){
				COND_Set_Grade1();
				MENU_Status_Header(Dispensing);
			} else {
				COND_Set_Grade2();
				MENU_Status_Header(LowPress);				
			}	
#endif //_CLINIC	
		break;
		case TankFull:
			if (!Tank_Full()){
				if (!transition_timer) {
					State = Running;			
					STATE_Set();
				}				
			} else {
				transition_timer = 10;
			}
#ifdef _ULTRAPURE
			if (!phase_timer) {
				State = Recirculation;
				STATE_Set();
			}
#endif //_ULTRAPURE	
#ifdef _CLINIC
			if (Grade2Flow()){
				COND_Set_Grade1();
				MENU_Status_Header(Dispensing);
			} else {
				COND_Set_Grade2();
				MENU_Status_Header(TankFull);				
			}	
#endif //_CLINIC		
		break;		
		case TankPump:
			if (!phase_timer){
				State = StandBy;
				STATE_Set();
			}
		break;		
		case PostFill:
			if (!transition_timer){
				if (Tank_Full()) {
					State = TankFull;
				} else {
					State = Running;
				}
				STATE_Set();
			}
#ifdef _CLINIC
			if (Grade2Flow()){
				COND_Set_Grade1();
				MENU_Status_Header(Dispensing);
			} else {
				COND_Set_Grade2();
				MENU_Status_Header(Running);				
			}	
#endif //_CLINIC
		break;
		default:
		
		break;		
	}
	
};



void STATE_Set(){ 		
	switch (State){
		case OFF:
			HC595_Clear_Output();
			MENU_Status_Header(State);
		break;
		case StandBy:
			phase_timer = 0;
			transition_timer = 0;
			MENU_Status_Header(State);		
			HC595_Clear_Output();	

		break;		
		case Running:
			if (Tank_Full()){
				State = TankFull; 
				STATE_Set();
				break;
			} else if (Low_Pressure()){
				State = LowPress; 
				STATE_Set();
				break;
			}
			COND_Set_Grade2();
#ifdef _ULTRAPURE
			phase_timer = Recirculation_Period -Recirculation_Time;
#endif //_ULTRAPURE
			HC595_Clear_Output();	
			Relay_Flags.Input_Valve = 1;   // inpu valve
			Relay_Flags.Boost_Pump = 1;  //need to set RO pump here
#if (defined(_DOUBLEFLOW) || defined(_CLINIC))
			Relay_Flags.Rec_Pump = 1;
#endif //_DOUBLEFLOW _CLINIC
			Relay_Flags.Sterilization = 1; //Sterilization lamp
			MENU_Status_Header(State);	
			error_timer = ERROR;
		break;	
#ifdef _ULTRAPURE	
		case Recirculation:
			COND_Set_Grade1();
			HC595_Clear_Output();	
			Relay_Flags.Rec_Pump = 1;
			Relay_Flags.Photoxidation = 1;		
			MENU_Status_Header(State);
			phase_timer = Recirculation_Time;	
			error_timer = ERROR;
		break;
#endif //_ULTRAPURE
		case Dispensing:
			COND_Set_Grade1();
#ifndef _CLINIC
			HC595_Clear_Output();
#endif //_CLINIC	
#ifdef _ULTRAPURE
			Relay_Flags.Rec_Pump = 1;	
#endif //_ULTRAPURE	
			Relay_Flags.Dispense_Valve = 1;
			MENU_Status_Header(State);	
		//	printf("set state dispensing \r\n"); 	
		break;
		case LowPress:
			HC595_Clear_Output();	
			Relay_Flags.Input_Valve = 1;   // inpu valve						
			MENU_Status_Header(State);
		break;
		case TankFull:
				HC595_Clear_Output();	
				MENU_Status_Header(State);
		break;	
		case TankPump:
			HC595_Clear_Output();	
			Relay_Flags.Tank_Pump = 10;				
			MENU_Status_Header(State);
			phase_timer = tank_pump_time;
		break;		
		case PostFill:
			//do nothing
		break;				
		default:
		
		break;		
	}
	
};

