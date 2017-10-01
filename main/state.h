/*
 * state.h
 *
 * Created: 07.06.2017 17:40:28
 *  Author: Andris
 */ 


#ifndef STATE_H_
#define STATE_H_

#include "state.h"

typedef enum {
	OFF = 0,
	StandBy = 1,
	Running = 2,
	Recirculation = 3,
	Dispensing = 4,
	Rinsing = 5,
	LowPress = 6,
	TankFull = 7,
	TankPump = 8,
	PostFill = 9
}STATES;

void STATE_Init();
void STATE_Check();
void STATE_Set_On();
void STATE_Set();

#ifdef _CLINIC
void STATE_Grade2Flow();
#endif //_CLINIC

#endif /* STATE_H_ */