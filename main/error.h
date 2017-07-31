/*
 * error.h
 *
 * Created: 27.07.2017 21:23:14
 *  Author: Andris
 */ 


#ifndef ERROR_H_
#define ERROR_H_


typedef enum {
	DI_Error = 0,
	Polishing_Error = 1,
	Filter_Error  = 2,
}ERRORS;


void ERROR_Check(void);
void ERROR_Check_Grade(void);

#endif /* ERROR_H_ */