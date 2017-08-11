

#ifndef __GPIO_H
	#define __GPIO_H
	
	#define HC595_PORT   PORTD
	#define HC595_DDR    DDRD

	#define HC595_DS_POS PD5      //Data pin (DS) pin location

	#define HC595_SH_CP_POS PD6     //Shift Clock (SH_CP) pin location 
	#define HC595_ST_CP_POS PD7      //Store Clock (ST_CP) pin location
	
	char Tank_Full();
	char Low_Pressure();
#ifdef _CLINIC
	char Grade2Flow();
#endif //_CLINIC
	
#endif //__GPIO_H
