/*
 * eeprom.c
 *
 * Created: 19.06.2017 21:15:06
 *  Author: Andris
 */ 
#include <avr/eeprom.h>
#include "eeprom.h"
#include "temperature.h"
#include "glcd.h"

uint16_t EEMEM BKP_Magic;
uint8_t EEMEM BKP_Units;
uint16_t EEMEM BKP_Filter; 
uint8_t EEMEM BKP_Rec_Time;
uint8_t EEMEM BKP_Rec_Period; 
float EEMEM BKP_C_Offset;        
float EEMEM BKP_C_Slope;   
volatile uint32_t FILTER_Time_Left =   60; //723600;
extern volatile uint8_t COND_Units;
extern volatile TEMPERATURE temperature;
volatile uint16_t Recirculation_Period = 60 * 60;  //60 minutes
volatile uint16_t Recirculation_Time = 15 * 60;  //20 minutes

void EEPROM_Init(){

	if (eeprom_read_word(&BKP_Magic) == 0x0102){
		printf("magic true \r\n");	
		COND_Units = eeprom_read_byte(&BKP_Units);
		temperature.offset = eeprom_read_float(&BKP_C_Offset);
	//	printf("%f",temperature.offset);
	//	GLCD_GoToLine(1);
	//	GLCD_DisplayFloatNumber(temperature.offset);
		temperature.slope = eeprom_read_float(&BKP_C_Slope);
	//	GLCD_GoToLine(5);
	//	GLCD_DisplayFloatNumber(temperature.slope);
	//	printf("%f",temperature.slope);
		FILTER_Time_Left = (uint32_t)eeprom_read_word(&BKP_Filter) * 60;
		Recirculation_Period = eeprom_read_byte(&BKP_Rec_Time) * 60;
		Recirculation_Time = eeprom_read_byte(&BKP_Rec_Period) * 60;
	}else{	
		printf("magic fasle \r\n");	
		EEPROM_Write_Defaults();
	}	
}

void EEPROM_Write_Defaults(){
	
	eeprom_write_word(&BKP_Magic,0x0102);	
	eeprom_write_byte(&BKP_Units,0);	
	eeprom_write_word(&BKP_Filter,FILTER_Time_Left/60);	
	eeprom_write_float(&BKP_C_Offset,0);		
	eeprom_write_float(&BKP_C_Slope,0.75);	
	eeprom_write_byte(&BKP_Rec_Time,Recirculation_Time/60);	
	eeprom_write_byte(&BKP_Rec_Period,Recirculation_Period/60);	
}	

void EEPROM_Write_Units(){
	eeprom_write_byte(&BKP_Units,COND_Units);
}
void EEPROM_Write_Filter(){
	uint16_t temp = FILTER_Time_Left/60;
	eeprom_write_word(&BKP_Filter,temp);	
}
void EEPROM_Write_Temperature(){
	eeprom_write_float(&BKP_C_Offset,temperature.offset);		
	eeprom_write_float(&BKP_C_Slope,temperature.slope);	
}
void EEPROM_Write_Rec_Time(){
	eeprom_write_byte(&BKP_Rec_Time,Recirculation_Time/60);
}
void EEPROM_Write_Rec_Period(){
	eeprom_write_byte(&BKP_Rec_Period,Recirculation_Period/60);
}

void EEPROM_Read_Rec_Period(){
		Recirculation_Time = eeprom_read_byte(&BKP_Rec_Time) * 60;
}		
		
void EEPROM_Read_Rec_Time(){		
		Recirculation_Period = eeprom_read_byte(&BKP_Rec_Period) * 60;
}