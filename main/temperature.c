/*
 * temperature.c
 *
 * Created: 27.05.2017 21:43:38
 *  Author: Andris
 */ 
#include <avr/io.h>
#include "ADC.h"
#include "temperature.h"
#include "glcd.h"


//temperature is linear kx+b, where k - slope, b offset

volatile TEMPERATURE temperature = {.temperatur = 0,
									 .offset = 0, 
									.slope = 0.75f};

uint16_t getTemperature(TEMPERATURE *temperature){
	return temperature->temperatur;
}

void setTemperature(TEMPERATURE *temperature, uint16_t temp){ //this will calculate actual temperature from ADC reading
	temperature->temperatur = temp;
}

uint16_t TEMPERATURE_Calculate(){
	uint16_t temp = (temperature.temperatur * temperature.slope) + temperature.offset; 
//	printf("e1 %"PRIu16" \r\n",temp);	
	//GLCD_GoToLine(1);
	//GLCD_DisplayFloatNumber(temperature.slope);
	return temp;
}


void TEMPERATURE_Display(){
	uint16_t temper = TEMPERATURE_Calculate();
	unsigned char str[5];
//	printf("e2 %"PRIu16" \r\n",temper);	
	snprintf(str, sizeof str, "%lu", (unsigned long)temper); /* Method 1 */
	GLCD_Printf("%c%c.%c@C   ",str[0],str[1],str[2]);							
}

void TEMPERATURE_Read(uint16_t temper){
	unsigned char str[5];
//	printf("e2 %"PRIu16" \r\n",temper);	
	snprintf(str, sizeof str, "%lu", (unsigned long)temper); /* Method 1 */
	GLCD_Printf("%c%c.%c@C   ",str[0],str[1],str[2]);							
}