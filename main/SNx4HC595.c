/*
 * SNx4HC595.c
 *
 * Created: 23.05.2017 01:39:25
 *  Author: Andris
 */ 

#include "SNx4HC595.h"
extern volatile RELAY_FLAGS Relay_Flags;

//Initialize HC595 System
void HC595_Init()
{
   //Make the Data(DS), Shift clock (SH_CP), Store Clock (ST_CP) lines output
   HC595_DDR|=((1<<HC595_SH_CP_POS)|(1<<HC595_ST_CP_POS)|(1<<HC595_DS_POS));
   HC595_Write(0b0000000000000000);  
}


//Sends a clock pulse on SH_CP line
void HC595_Pulse()
{
   //Pulse the Shift Clock

   HC595_PORT|=(1<<HC595_SH_CP_POS);//HIGH

   HC595_PORT&=(~(1<<HC595_SH_CP_POS));//LOW

}

//Sends a clock pulse on ST_CP line
void HC595_Latch()
{
   //Pulse the Store Clock

   HC595_PORT|=(1<<HC595_ST_CP_POS);//HIGH
   _delay_loop_1(1);

   HC595_PORT&=(~(1<<HC595_ST_CP_POS));//LOW
   _delay_loop_1(1);
}


/*
Main High level function to write a single byte to
Output shift register 74HC595. 

Arguments:
   single byte to write to the 74HC595 IC

Returns:
   NONE

Description:
   The byte is serially transfered to 74HC595
   and then latched. The byte is then available on
   output line Q0 to Q7 of the HC595 IC.

*/
void HC595_Write(uint16_t data)
{
   //Send each 8 bits serially
	
   //Order is MSB first
   for(uint8_t i=0;i<16;i++)
   {
      //Output the data on DS line according to the
      //Value of MSB
      if(data & 0b1000000000000000)
      {
         //MSB is 1 so output high

         HC595_DataHigh();
      }
      else
      {
         //MSB is 0 so output high
         HC595_DataLow();
      }

      HC595_Pulse();  //Pulse the Clock line
      data=data<<1;  //Now bring next bit at MSB position

   }

   //Now all 8 bits have been transferred to shift register
   //Move them to output latch at one
   HC595_Latch();
}

void HC595_Clear_Output(){
	//Relay_Flags.Photoxidation = 0;
	Relay_Flags.Boost_Pump = 0;
	Relay_Flags.Dispense_Valve = 0;
	Relay_Flags.Input_Valve = 0;
	Relay_Flags.Rec_Pump = 0;
	//Relay_Flags.Sterilization = 0;
	Relay_Flags.Tank_Pump = 0;
	
}
