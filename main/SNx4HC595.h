/*
 * SNx4HC595.h
 *
 * Created: 23.05.2017 01:34:50
 *  Author: Andris
 */ 
#include <avr/io.h>
#include <util/delay.h>


#ifndef SNX4HC595_H_
#define SNX4HC595_H_

	#define HC595_PORT   PORTD
	#define HC595_DDR    DDRD

	#define HC595_DS_POS PD5		//Data pin (DS) pin location

	#define HC595_SH_CP_POS PD6     //Shift Clock (SH_CP) pin location 
	#define HC595_ST_CP_POS PD7     //Store Clock (ST_CP) pin location
	
	#define HC595_DataHigh() (HC595_PORT|=(1<<HC595_DS_POS))
	#define HC595_DataLow() (HC595_PORT&=(~(1<<HC595_DS_POS)))
	
	void HC595_Init();				//Initialize HC595 System
	void HC595_Pulse();				//Sends a clock pulse on SH_CP line
	void HC595_Latch();				//Sends a clock pulse on ST_CP line
	void HC595_Write(uint16_t);		//Main High level function to write a single byte to Output shift register
	void HC595_Clear_Output();	
	
	
	// P19 Tank Pump
	// P18 Recirculation Pump
	// P17 Boost Pump
	// P16 Input valve
	// P15 Bio Lamp
	// P14 Oxi Lamp
	// P13 Dispense valve
	// P12 Not used
	
	typedef union
	{
	 uint16_t     flags;      /* Allows us to refer to the flags 'en masse' */
	 struct
	 {
	  uint16_t	Photoxidation : 1,     //P14		  
				LCD_Backlight : 1,     //LCD BACKLIGHT	  
				Not_connected : 1,     //??	  
				Sterilization : 1,     //P15	  
				Input_Valve : 1,     //P16
				Boost_Pump : 1,     //P17				
				Rec_Pump : 1,        //P18
				Tank_Pump : 1,        //P19				
				Cond5_Relay : 1,     // Cond5				
				Buzzer_Relay : 1,     // LED3, buzzer
				Cond4_Relay : 1,     //  NC				
				Cond3_Relay : 1,     //  Cond3													  
				Cond2_Relay : 1,     //  NC
				Cond1_Relay: 1,     //  Cond1
				Relay220_2 : 1,        // Controls P12, unused		
				Dispense_Valve : 1;        //Controls P13, REL220_3	  
	 };
	} RELAY_FLAGS;

//	Flags.all_flags = 0U; /* Clear all flags */

	
#endif /* SNX4HC595_H_ */