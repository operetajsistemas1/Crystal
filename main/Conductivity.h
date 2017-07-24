/*
 * conductivity.h
 *
 * Created: 04.06.2017 17:53:27
 *  Author: Andris
 */ 
#ifndef CONDUCTIVITY_H_
#define CONDUCTIVITY_H_

#include <avr/io.h>

typedef struct {
	uint8_t Current_Grade;// 0 = grade 2 sensor, 1 = grade 1 sensor
	volatile uint16_t Grade1;// 0 = grade 2 sensor, 1 = grade 1 sensor  
	volatile uint16_t Grade2;// 0 = grade 2 sensor, 1 = grade 1 sensor  
	uint32_t Timer_Reset_Pending;	
	uint16_t slope;
	uint16_t offset;
}COND;

void COND_Init(void);
uint32_t COND_Get_Ticks(void);
void COND_Set_Grade1(COND *Conductivity);
void COND_Set_Grade2(COND *Conductivity);


#endif /* CONDUC TIVITY_H_ */