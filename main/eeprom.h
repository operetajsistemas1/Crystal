/*
 * eeprom.h
 *
 * Created: 19.06.2017 21:15:19
 *  Author: Andris
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

void EEPROM_Init();
void EEPROM_Write_Defaults();
void EEPROM_Write_Units();
void EEPROM_Write_Filter();
void EEPROM_Write_Temperature();
void EEPROM_Write_Rec_Time();
void EEPROM_Write_Rec_Period();
void EEPROM_Read_Rec_Period();
void EEPROM_Read_Rec_Time();


#endif /* EEPROM_H_ */