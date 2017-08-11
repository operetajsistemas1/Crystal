/*
 * config.h
 *
 * Created: 23.05.2017 01:43:21
 *  Author: Andris
 *
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

#define F_CPU 16000000UL  // 16 MHz
#define TRUE 1
#define FALSE 0
#define DEBUGG

//Uncomment definition for needed device
//1. EX TRACE/HPLC/BIO
#define _ULTRAPURE
//2. EX/7 Pure
//#define _PURE
//3. EX/7 RO,  Sterifeed
//#define _RO
//4. Clinic
//#define _CLINIC
//5. Doubleflow 
//#define _DOUBLEFLOW





#endif /* CONFIG_H_ */