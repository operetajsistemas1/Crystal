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
#define COMPENSATION_TABLE_SIZE 11
   // t = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50}
uint32_t const t_compensation_table[COMPENSATION_TABLE_SIZE]={86190, 60480, 43430, 31870, 23850, 18180, 14090, 11090, 8849, 7154, 5853};

//temperature is linear kx+b, where k - slope, b offset

volatile TEMPERATURE temperature = {.temperatur = 240,
									 .offset = -40.762, 
									.slope = 0.996f};

uint16_t getTemperature(TEMPERATURE *temperature){
	return temperature->temperatur;
}

void setTemperature(TEMPERATURE *temperature, uint16_t temp){ //this will calculate actual temperature from ADC reading
	temperature->temperatur = temp;
}

uint16_t TEMPERATURE_Calculate(){
	//uint16_t ADCValue;
//	uint16_t ADCValue = ADC_Read(0);
	//printf("TEMP ADC %"PRIu16" \r\n",ADCValue);		
//	temperature.temperatur = (1-0.2)*temperature.temperatur + 0.2*ADCValue;
	
	uint16_t temp = (temperature.temperatur * temperature.slope) + temperature.offset; 
	if (temp> 750) temp = 250;
//	printf("e1 %"PRIu16" \r\n",temp);	

	return temp;
}


void TEMPERATURE_Display(uint16_t temper){
//	uint16_t temper = TEMPERATURE_Calculate();
	unsigned char str[5];
//	printf("temper %"PRIu16" \r\n",temper);	
	snprintf(str, sizeof str, "%lu", (unsigned long)temper); /* Method 1 */
	if (temper >= 100){
		GLCD_Printf("%c%c.%c@C   ",str[0],str[1],str[2]);	
	} else if (temper >= 10){
		GLCD_Printf(" %c.%c@C   ",str[0],str[1]);		
	} else {
		GLCD_Printf(" 0.%c@C   ",str[0]);		
	}							
}

//void TEMPERATURE_Read(uint16_t temper){
	//unsigned char str[5];
////	printf("e2 %"PRIu16" \r\n",temper);	
	//snprintf(str, sizeof str, "%lu", (unsigned long)temper); /* Method 1 */
	//GLCD_Printf("%c%c.%c@C   ",str[0],str[1],str[2]);							
//}

uint32_t Temperature_Compensate (){
	
	uint16_t temper = TEMPERATURE_Calculate();
	if (temper > 500) temper = 500;
	if (temper < 1) temper = 0;
	// temperature is measured in decidegrees
//	printf("temper [%"PRIu16"] \r\n",temper);
	uint8_t table_element = temper/50;  // calculate which element in the table this is. Step size is 5 but we need to multiply with 10 to mach input temperature format
//	printf("table_element [%"PRIu8"] \r\n",table_element);
	uint8_t dx = temper % 50;  // dx for approximation
	//printf("dx [%"PRIu8"] \r\n",dx);
	uint16_t dy = t_compensation_table[table_element] - t_compensation_table[table_element+1];
	//printf("dy [%"PRIu16"] \r\n",((dy / 50)   * dx));
	uint32_t result = t_compensation_table[table_element] - ((dy/50) *dx);
//	printf("comp in [%"PRIu32"] \r\n",result);
	return result;
}