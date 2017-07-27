/*
 * error.h
 *
 * Created: 27.07.2017 21:23:14
 *  Author: Andris
 */ 


#ifndef ERROR_H_
#define ERROR_H_


typedef enum {
	DI = 0,
	Polishing = 1,
	Filter = 2,
}ERRORS;


void ERROR_Check(void);



#endif /* ERROR_H_ */