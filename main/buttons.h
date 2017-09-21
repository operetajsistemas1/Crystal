/*
 * buttons.h
 *
 * Created: 29.05.2017 18:58:34
 *  Author: Andris
 */ 
#include <avr/io.h>

#ifndef BUTTONS_H_
#define BUTTONS_H_

#define NUMBER_OF_BUTTONS 8
#define BTN_SAFE_DEBOUNCE 2


typedef enum {
	UP = 1,
	OK = 2,
	DOWN = 4,
	SLEEP = 8,
	MENU = 16,
	PUMPOUT = 32,
	RUN = 64
}ButtonTypes;

typedef struct {
	uint8_t ticks;
	uint8_t BTN_Active;
}BTN;


uint8_t static BTN_Get_Port_State(void);
void BTN_Check(void);
uint8_t static BTN_Debounce(BTN *btn,uint8_t currentPortState);

#endif //* BUTTONS_H_ *
