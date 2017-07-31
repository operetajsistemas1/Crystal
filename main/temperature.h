/*
 * temperature.h
 *
 * Created: 28.05.2017 18:41:28
 *  Author: Andris
 */ 


#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_


typedef struct TEMPERATURE {
	uint16_t temperatur;
	float offset;
	float slope;
} TEMPERATURE; 

uint16_t getTemperature(TEMPERATURE*);
uint16_t TEMPERATURE_Calculate(void);
void setTemperature(TEMPERATURE*, uint16_t);
void TEMPERATURE_Read(uint16_t);
uint32_t Temperature_Compensate (void);


#endif /* TEMPERATURE_H_ */