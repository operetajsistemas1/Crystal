/*
 * MenuTree.c
 *
 * Created: 11.06.2017 12:41:25
 *  Author: Andris
 */ 

#include <avr/io.h>
#include "MenuTree.h"
#include "glcd.h"
#define NULL 0
#include "uart.h"
#include "state.h"
#include "gpio.h"
#include "eeprom.h"
#include "buttons.h"
#include "temperature.h"
#include "Conductivity.h"
#include "error.h"
char LCD_Crystal[10] = {'C','r','y','s','t','a','l',' ',' ',0};
char LCD_Running[10] = {'R','u','n','n','i','n','g',' ',' ',0};
char LCD_OFF[10] = {'O','f','f',' ',' ',' ',' ',' ',' ',0};
char LCD_StandBy[10] = {'S','t','a','n','d','b','y',' ',' ',0};
char LCD_Recirc[10] = {'R','e','c','i','r','c','.',' ',' ',0};
char LCD_Dispensing[10] = {'D','i','s','p','e','n','s',' ',' ',0};
char LCD_LowPressure[10] = {'L','o','w','p','r','e','s',' ',' ',0};
char LCD_TankFull[10] = {'T','a','n','k','F','u','l','l',' ',0};
char LCD_Blank[10] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',0};
char LCD_TankPump[10] = {'P','u','m','p',' ',' ',' ',' ',' ',0};
uint16_t temper = 0;
extern volatile uint8_t Error_Flag;
extern volatile STATES State;
extern volatile uint8_t COND_Units;

extern uint8_t MENU_SCREEN;
extern volatile uint32_t FILTER_Time_Left;
extern volatile uint8_t Use_MOhm;
volatile uint8_t process = 0;
extern volatile uint16_t Recirculation_Period;  //60 minutes
extern volatile uint16_t Recirculation_Time;  //20 minutes
extern volatile BTN buttons;
extern volatile uint8_t Calibration_Running;
extern volatile TEMPERATURE temperature;
extern volatile COND  Conductivity;


extern tree_node resetfilter;
tree_node reset = {
	.parent = &resetfilter,
	.text = "Reset\0",
	.selected = 1,
	.child =
	{ NULL, NULL, NULL, NULL, NULL}
};


// third and last level nodes, we have to define behaviour on last press
extern tree_node units;
tree_node usiemens = {
	.parent = &units,
	.text = "uS\0",
	.selected = 1,
	.child =
	{ NULL, NULL, NULL, NULL, NULL}
};
tree_node mohm = {
	.parent = &units,
	.text = "MOhm\0",
	.selected = 0,
	.child =
	{ NULL, NULL, NULL, NULL, NULL}
};



extern tree_node params;
tree_node recper = {
	.parent = &params,
	.text = "Recirculation period\0",
	.selected = 1,
	.child =
	{ NULL, NULL, NULL, NULL, NULL}
};
tree_node rectime = {
	.parent = &params,
	.text = "Recirculation time\0",
	.selected = 0,
	.child =
	{ NULL, NULL, NULL, NULL, NULL}
};
tree_node units = {
	.parent = &params,
	.text = "Measurement units\0",
	.selected = 0,
	.child =
	{ &usiemens, &mohm, NULL, NULL, NULL}
};



extern tree_node parent; // define that we will have such parent reference
tree_node params = {
	.parent = &parent,
	.text = "Options \0",
	.selected = 0,
	.child =
	{ &units, &recper, &rectime, NULL}//  { &units, &timedate, &recirc, &logint, NULL}
};


tree_node resetfilter = {
	.parent = &parent,
	.text = "Reset filter counter \0",
	.selected = 0,
	.child =
	{ &reset, NULL, NULL, NULL, NULL}
};


tree_node parent = {
	.parent = NULL,
	.text = "Main menu \0",
	.selected = 0,
	.child =
	{&resetfilter, &params, 0, 0, 0} //{ &voldis, &params, &clerr, &sval, &resetfilter} //{ &voldis, &params, &clerr, &sval, &service}
};

tree_node calibrate = {
	.parent = NULL,
	.text = "Calibrate Temperature",
	.selected = 1,
	.child =
	{ NULL, NULL, NULL, NULL, NULL}
};



tree_node * tree_node_selected = &parent;



void MENU_Draw(){
	GLCD_Clear();
	GLCD_GoToLine(0);
	GLCD_DisplayString(tree_node_selected->text);
	for (volatile uint8_t iter = 0; iter<= 4; iter++ ) {
		if (tree_node_selected->child[iter] == NULL) continue;
			if (iter == tree_node_selected->selected){
				GLCD_EnableDisplayInversion();
				GLCD_GoToLine(iter+2);
				GLCD_DisplayString(tree_node_selected->child[iter]->text);
				GLCD_DisableDisplayInversion();
			} else {
				GLCD_GoToLine(iter+2);
				GLCD_DisplayString(tree_node_selected->child[iter]->text);
			}			
		}
};	



void MENU_Down(){
	if (process){
		MENU_Process(2);
		return;
	}	
	char select = ++tree_node_selected->selected;
	if (tree_node_selected->child[select] == NULL) {
		tree_node_selected->selected = 0;
	}
	printf("qwe");
	MENU_Draw();
}

void MENU_Up(){
	if (process){
		MENU_Process(1);
		return;
	}
	char select = tree_node_selected->selected;
	UART_Transmit(select);
	if (select == 0){
		for (char iter = 0; iter < 5; iter++){
			if (tree_node_selected->child[iter] == NULL){
				tree_node_selected->selected = iter-1;
				UART_Transmit(tree_node_selected->selected);
				MENU_Draw();
				return;
			}
		}		
	} else {
		tree_node_selected->selected = select - 1;
		MENU_Draw();
		
	}

}

void MENU_Out(){
	if (process){
		MENU_Process(4);
		return;
	}
	if (tree_node_selected->parent != NULL){
		tree_node_selected = tree_node_selected->parent;
		MENU_Draw();		
//		UART_Transmit(1);
		
	} else {
//		UART_Transmit(0);
		MENU_SCREEN = 0;
		GLCD_Clear();
		MENU_Status();
		MENU_Status_Header();
		MENU_Status_Header2();
	}
}	


void MENU_In(){
	if (process){
		MENU_Process(3);
		return;
	}
		char select = tree_node_selected->selected;
	if (tree_node_selected->child[select]->child[0] != NULL) {
		tree_node_selected = tree_node_selected->child[select];
		MENU_Draw();
	}else {
		MENU_Process(0);
	}
}


void MENU_Process(int button){
	printf("%s \r\n",tree_node_selected->text);	
	if (tree_node_selected == &calibrate){
		switch(process){
			case 0:
			printf("0 \r\n");				
			MENU_SCREEN = 1;
			GLCD_Clear();				
			GLCD_GoToLine(0);
			GLCD_DisplayString("Calibrate temperature");	
			GLCD_GoToLine(6);
			GLCD_DisplayString("Wait for stable reading");
			GLCD_GoToLine(7);
			GLCD_DisplayString("Press OK");	
			GLCD_SetCursor(0,3,40);	
			TEMPERATURE_Display();
			process =1;	
			Calibration_Running = 1;
			break;	
			
			case 1:
				printf("1 \r\n");	
	
				GLCD_SetCursor(0,3,40);	
				TEMPERATURE_Display();
						
				if (button == 3) {
					temper = TEMPERATURE_Calculate();
					GLCD_Clear();				
					GLCD_GoToLine(0);
					GLCD_DisplayString("Calibrate temperature");	
					GLCD_GoToLine(6);
					GLCD_DisplayString("Use Up and Down buttons");
					GLCD_GoToLine(7);
					GLCD_DisplayString("to set temperature");				
				//	GLCD_SetCursor(1,3,5);
					//GLCD_Printf("oC");
					GLCD_SetCursor(0,3,40);
					TEMPERATURE_Read(temper);
					process = 2;
				} 
			break;
			
			case 2:
			switch (button){
				case 1:
						temper += 1;
						GLCD_SetCursor(0,3,40);		
						TEMPERATURE_Read(temper);													
				break;
				case 2:
					temper -= 1;
					GLCD_SetCursor(0,3,40);	
					TEMPERATURE_Read(temper);													
				break;	
				case 3:
					temperature.offset = temperature.offset - (TEMPERATURE_Calculate() - temper);
					void EEPROM_Write_Temperature();
					process =0;
					MENU_SCREEN = 0;	
					Calibration_Running = 0;
					tree_node_selected = &parent;	
					GLCD_Clear();
		//			MENU_Status();
		//			MENU_Status_Header();
		//			MENU_Status_Header2();	
				break;		
			}
		}

	
	} else if (tree_node_selected == &resetfilter){
			FILTER_Time_Left = 723600;
			MENU_SCREEN = 0;
			tree_node_selected = &parent;					
			GLCD_Clear();
			MENU_Status();
			MENU_Status_Header();
			MENU_Status_Header2();
			printf("behav reset filt\r\n");	
	} else if (tree_node_selected == &units){
		if (tree_node_selected->selected == 0){
			MENU_SCREEN = 0;			
			COND_Units = 0;
			tree_node_selected = &parent;					
			EEPROM_Write_Units();
			GLCD_Clear();
			MENU_Status();
			MENU_Status_Header();
			MENU_Status_Header2();			
		}else if (tree_node_selected->selected == 1){
			COND_Units = 1;
			MENU_SCREEN = 0;	
			tree_node_selected = &parent;		
			EEPROM_Write_Units();
			GLCD_Clear();
			MENU_Status();
			MENU_Status_Header();
			MENU_Status_Header2();	
		}		
		if (Conductivity.Current_Grade == 1) {
			GLCD_SetCursor(0,3,33);
			GLCD_DisplayChar('1');	
			GLCD_SetCursor(0,4,30);
			GLCD_DisplayChar(' ');	
		} else {
			GLCD_SetCursor(0,3,33);
			GLCD_DisplayChar(' ');	
			GLCD_SetCursor(0,4,30);
			GLCD_DisplayChar('2');	
		}	 	
	} else if (tree_node_selected == &params){
		if (tree_node_selected->selected == 1){
			switch(process){
				case 0:
				GLCD_Clear();				
				GLCD_GoToLine(0);
				GLCD_DisplayString(tree_node_selected->child[1]->text);	
				GLCD_GoToLine(6);
				GLCD_DisplayString("Use Up and Down buttons");
				GLCD_GoToLine(7);
				GLCD_DisplayString("Maximum period: 4:00");				
				GLCD_SetCursor(1,3,5);
				GLCD_Printf("[H:MM]");
				GLCD_SetCursor(0,3,40);
				MENU_Print_Time(Recirculation_Period);
				process =1;	
				break;	
				
				case 1:
				switch (button){
					case 1:
					if (Recirculation_Period < 60*4*60) {
							Recirculation_Period += 6;
							GLCD_SetCursor(0,3,40);	
							buttons.BTN_Active = 0; // override btn to enable multiple number scroll with one click		
							MENU_Print_Time(Recirculation_Period);									
							
					}					
					break;
					case 2:
					if (Recirculation_Period >= 60) {
						Recirculation_Period -= 10;	
						GLCD_SetCursor(0,3,40);	
						buttons.BTN_Active = 0; // override btn to enable multiple number scroll with one click		
						MENU_Print_Time(Recirculation_Period);								
						
					}						
					break;	
					case 3:
						EEPROM_Write_Rec_Period();
						process =0;
						MENU_SCREEN = 0;	
						tree_node_selected = &parent;	
						GLCD_Clear();
						MENU_Status();
						MENU_Status_Header();
						MENU_Status_Header2();	
					break;		
					case 4:
						EEPROM_Read_Rec_Period();
						process =0;
						MENU_SCREEN = 0;	
						tree_node_selected = &parent;	
						GLCD_Clear();
						MENU_Status();
						MENU_Status_Header();
						MENU_Status_Header2();	
					break;	
				}
			}
		}else if (tree_node_selected->selected == 2){
			switch(process){
				case 0:
				GLCD_Clear();				
				GLCD_GoToLine(0);
				GLCD_DisplayString(tree_node_selected->child[1]->text);	
				GLCD_GoToLine(6);
				GLCD_DisplayString("Use Up and Down buttons");
				GLCD_GoToLine(7);
				GLCD_DisplayString("Maximum time:");	
				GLCD_SetCursor(1,7,20);
				MENU_Print_Time(Recirculation_Period);											
				GLCD_SetCursor(1,3,5);
				GLCD_Printf("[H:MM]");
				GLCD_SetCursor(0,3,40);
				MENU_Print_Time(Recirculation_Time);
				process =1;	
				break;	
				
				case 1:
				switch (button){
					case 1:
					if (Recirculation_Time < (Recirculation_Period-6)) {
						Recirculation_Time += 6;
						GLCD_SetCursor(0,3,40);	
						buttons.BTN_Active = 0; // override btn to enable multiple number scroll with one click		
						MENU_Print_Time(Recirculation_Time);				
					}					
					break;
					case 2:
					if (Recirculation_Time >= 60) {
						Recirculation_Time -= 10;	
						GLCD_SetCursor(0,3,40);	
						buttons.BTN_Active = 0; // override btn to enable multiple number scroll with one click		
						MENU_Print_Time(Recirculation_Time);						
					}						
					break;	
					case 3:
						EEPROM_Write_Rec_Time();
						process =0;
						MENU_SCREEN = 0;	
						tree_node_selected = &parent;	
						GLCD_Clear();
						MENU_Status();
						MENU_Status_Header();
						MENU_Status_Header2();	
					break;		
					case 4:
						EEPROM_Read_Rec_Time();
						process =0;
						MENU_SCREEN = 0;	
						tree_node_selected = &parent;	
						GLCD_Clear();
						MENU_Status();
						MENU_Status_Header();
						MENU_Status_Header2();	
					break;	
				}

			}	
		}
	}
}

void MENU_Print_Time(uint16_t value){
	uint8_t    msd, nsd; /// Tree values to display time on the screen
	msd = value / 3600U;
	value -= msd * 3600U;
	nsd = value / 60U;
	if (msd == 0){
		GLCD_Printf("0",msd);		
	} else {
		GLCD_Printf("%d",msd);							
	}
	if (nsd == 0){
		GLCD_Printf(":00");		
	} else if (nsd <10){
		GLCD_Printf(":0%d",nsd);					
	} else {
		GLCD_Printf(":%d",nsd);							
	}
	
}



void MENU_Status(){
				
	GLCD_SetCursor(0,7,10);
	TEMPERATURE_Display();			
			
		if (FILTER_Time_Left>3600){ 
			if (State==Running) FILTER_Time_Left--;
			GLCD_SetCursor(1,7,20);
			GLCD_Printf("%u h   ",(FILTER_Time_Left/3600));	
			EEPROM_Write_Filter();
		} else {
			GLCD_SetCursor(1,7,20);
			GLCD_Printf("0 h   ");	
			Error_Flag |= (1 << Filter_Error);   //Ser DI error
		}			
				
			if (!Conductivity.Overflow)	{
			if (COND_Units == 1){
				volatile uint32_t resistivity =  COND_Get_Kohm();
				if (resistivity > 18200) resistivity = 18200;		
				if (resistivity < 100) {
					resistivity = 100;	
					GLCD_SetCursor(0,2,42);
					GLCD_DisplayChar32(16);						
					GLCD_SetCursor(0,2,60);
					GLCD_DisplayChar32(14);	  
					GLCD_SetCursor(1,2,12);
					GLCD_DisplayChar32(0);	  
					GLCD_SetCursor(1,2,28);
					GLCD_DisplayChar32(10);	
					GLCD_SetCursor(1,2,32);
					GLCD_DisplayChar32(1);	  
							
				} else {	
		
					GLCD_SetCursor(1,2,10);
					GLCD_DisplayChar32(10);	
					uint8_t digit = resistivity / 10000;
					resistivity = resistivity % 10000;	
					if (digit) {
						GLCD_SetCursor(0,2,42);
						GLCD_DisplayChar32(digit);	
					} else {
						GLCD_SetCursor(0,2,42);
						GLCD_DisplayChar32(16);	
					}
					digit = resistivity / 1000;
					resistivity = resistivity % 1000;
					GLCD_SetCursor(0,2,58);
					GLCD_DisplayChar32(digit);					
					digit = resistivity / 100;
					resistivity = resistivity % 100;
					GLCD_SetCursor(1,2,16);
					GLCD_DisplayChar32(digit);
					digit = resistivity / 10;
					resistivity = resistivity % 10;
					GLCD_SetCursor(1,2,32);
					GLCD_DisplayChar32(digit);				
				}				
										
				
			} else {
				volatile uint32_t conductivity =  COND_Get_US();	
			//	printf("%d",conductivity );	
				if (conductivity < 55) conductivity = 55;	
				if (conductivity > 9999) {
					GLCD_SetCursor(0,2,52);
					GLCD_DisplayChar32(16);	  // 					
					GLCD_SetCursor(0,2,41);
					GLCD_DisplayChar32(16);	  // 					
					GLCD_SetCursor(1,2,1);
					GLCD_DisplayChar32(13);	  // 
					GLCD_SetCursor(1,2,16);
					GLCD_DisplayChar32(1);	  //
					GLCD_SetCursor(1,2,32);
					GLCD_DisplayChar32(0);	  //										
				} else {			
				GLCD_SetCursor(0,2,58);
				GLCD_DisplayChar32(10);		
				uint8_t digit = conductivity / 1000;
				conductivity = conductivity % 1000;
				GLCD_SetCursor(0,2,41);
				GLCD_DisplayChar32(digit);	  // change back to digit			
				digit = conductivity / 100;
				conductivity = conductivity % 100;	
				GLCD_SetCursor(1,2,0);
				GLCD_DisplayChar32(digit);										
				digit = conductivity / 10;
				conductivity = conductivity % 10;				
				GLCD_SetCursor(1,2,16);
				GLCD_DisplayChar32(digit);				
				digit = conductivity;			
				GLCD_SetCursor(1,2,32);
				GLCD_DisplayChar32(digit);						
				}
			} 
			}else {
				GLCD_SetCursor(1,2,32);
				GLCD_DisplayChar32(15);	
				GLCD_SetCursor(1,2,16);
				GLCD_DisplayChar32(15);	
				GLCD_SetCursor(1,2,0);
				GLCD_DisplayChar32(15);			
				GLCD_SetCursor(0,2,50);	
				GLCD_DisplayChar32(16);			
				GLCD_SetCursor(0,2,39);	
				GLCD_DisplayChar32(16);	//white space				
			}			
}

void MENU_Status_Header(){
	switch (State){
		case OFF:
			GLCD_GoToLine(0);
			GLCD_DisplayString(LCD_OFF);

		break;
		case StandBy:
			GLCD_GoToLine(0);
			GLCD_DisplayString(LCD_StandBy);

		
		break;		
		case Running:
			GLCD_GoToLine(0);
			GLCD_DisplayString(LCD_Running);
		 

		break;
		case Recirculation:
			GLCD_GoToLine(0);
			GLCD_DisplayString(LCD_Recirc);

		
		break;
		case Dispensing:
			GLCD_GoToLine(0);
			GLCD_DisplayString(LCD_Dispensing);

		
		break;
		case LowPress:
			GLCD_GoToLine(0);
			GLCD_DisplayString(LCD_StandBy);		
		break;
		case TankFull:
			GLCD_GoToLine(0);
			GLCD_DisplayString(LCD_StandBy);
		break;	
		case TankPump:
			GLCD_GoToLine(0);
			GLCD_DisplayString(LCD_TankPump);
		break;		
		default:
		
		break;		
	}
	
	
}

void MENU_Status_Header2(){
			//	printf ("LP: status header before \r\n");
			if (Tank_Full()){
				GLCD_SetCursor(1,0,18);
				GLCD_DisplayString(LCD_TankFull);
			} else if (State == LowPress){
		//		printf ("LP: status header2 \r\n");
				GLCD_SetCursor(1,0,18);
				GLCD_DisplayString(LCD_LowPressure);	
		
			} else {
				GLCD_SetCursor(1,0,18);
				GLCD_DisplayString(LCD_Blank);					
			}
			if (COND_Units) {
				GLCD_ShowMOhm();	
			}else {
				GLCD_ShowUS();
			}
			
}