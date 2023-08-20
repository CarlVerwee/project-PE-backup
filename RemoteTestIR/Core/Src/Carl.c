/*
 * Carl.c
 *
 *  Created on: May 11, 2023
 *      Author: Carl Verwee
 */

#include "Carl.h"

/* Some Extern Private variables -------------------------------*/
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim15;

/* Some Application defines -------------------------------*/
#define TIM_1us htim15
#define TIM_PWM_CARRIER htim1
#define TIM_PWM_CARRIER_CHANNEL TIM_CHANNEL_1

/* Some Application variables -------------------------------*/
//uint16_t (*tijd)[500]; //60 verschillende commando's met elk 500 tijseenheden die het commando(signaal) beschrijven

uint16_t tijd[2][500];
uint16_t pulsecounter = 1 ;
uint8_t commando = 0;
uint8_t IR_READ_flag = 0;
uint16_t lengte;
uint8_t NoSignal = 1;
uint8_t lengte_signaal_gevonden = 0;
uint16_t lengte_signaal[60];

void InitTouch()
{
//	uint16_t device_address_write = 0xBA;
//	uint16_t device_address_read = 0xBB;
//	uint8_t dataWrite[] = {0x81,0x40};
//	uint8_t ontvangen[11];

}
void All_Carl_Inits()
{
	//tijd = (uint16_t (*)[500])calloc(60, sizeof(uint16_t[500]));
	//tijd = (uint16_t(*)[500])malloc(60 * sizeof(uint16_t[500]));
/*	if (tijd == NULL) {
		printf("memory allocation failure\r\n");
	    }
	else{
		printf("memory allocation complete\r\n");
	}*/
/*	for(uint8_t x=0; x<60 ; x++)
	{
		for(uint16_t y=0; y<500; y++)
		{
		tijd[x][y]=0;
		}
	}*/
	InitTouch();
	IR_Carrier_OFF();
}
void Touch()
{
//	uint16_t device_address_write = 0xBA;
//	uint16_t device_address_read = 0xBB;
//	uint8_t nulladdress = 0x00;
//	uint8_t buffer[12];
//	uint8_t buffer1[0];
//	uint16_t X_waarde;
//	uint16_t Y_waarde;
}
void Interrupt_flank()
{

	if(IR_READ_flag == 1)
	{
		HAL_TIM_Base_Stop(&TIM_1us); //timer stoppen
		//timer in het begin is steeds nul maar deze hebben we niet nodig

		tijd[commando][pulsecounter] = TIM_1us.Instance -> CNT; //timer opslaan
		lengte = pulsecounter;
		pulsecounter++;
		TIM_1us.Instance -> CNT = 0;
		HAL_TIM_Base_Start(&TIM_1us);


	}
	else
	{
		printf("ik kan niet werken, aangezien ik uit sta\r\n");
	}

}
void IR_Carrier_ON()
{
	HAL_TIM_PWM_Start(&TIM_PWM_CARRIER, TIM_PWM_CARRIER_CHANNEL);
}
void IR_Carrier_OFF()
{
	HAL_TIM_PWM_Stop(&TIM_PWM_CARRIER, TIM_PWM_CARRIER_CHANNEL);
}
void IR_Read()
{
	IR_READ_flag = 1 ;
}

void IR_UNDERSTAND()
{
	//Is er wel iets gemeten?
	//5 metingen test
	uint8_t measurement = 0;
	for(uint8_t five_measurements = 0; five_measurements < 5; five_measurements++)
	{

		if(tijd[commando][five_measurements] == 0)
		{
			measurement++;
		}
	}
	if(measurement == 5)
		{
			printf("Er is niks gemeten \r\n");
			NoSignal = 1;
		}
	else
	{
		printf("Er is iets gemeten %d \r\n", lengte_signaal[commando]);
		NoSignal = 0;
	}
	if(NoSignal == 0) //Enkel als er een signaal is
	{

		IR_Debug();
		//IR_READ_flag = 0; //stoppen met leesInterrupt
		//Hoe lang is het signaal?
		if(lengte_signaal_gevonden == 0)
		{
			uint16_t waarde = 499;
			while(tijd[commando][waarde] == 0)
			{
				waarde--;
			}
			lengte_signaal[commando] = waarde;
			printf("lengte signaal = %d\r\n", lengte_signaal[commando]);
			lengte_signaal_gevonden = 1;
			IR_Debug();
		}

	}


}



void IR_sent()
{
	if(NoSignal == 0)
	{
    IR_READ_flag = 0; //er kan niks meer ingelezen worden
   for(uint8_t i = 0; i <= lengte_signaal[commando]; i++)
   {

	   if((i%2)==0)
	   {
		   HAL_TIM_Base_Stop(&TIM_1us);
		   TIM_1us.Instance->CNT = 0;
		   HAL_TIM_Base_Start(&TIM_1us);
		   IR_Carrier_ON();
		   while((TIM_1us.Instance->CNT) <= tijd[commando][i]){}
	   }
	   else
	   {
		   HAL_TIM_Base_Stop(&TIM_1us);
		   TIM_1us.Instance->CNT = 0;
		   HAL_TIM_Base_Start(&TIM_1us);
		   IR_Carrier_OFF();
		   while((TIM_1us.Instance->CNT) <= tijd[commando][i]){}

	   }
   }
   IR_Carrier_OFF();


}
}
void IR_Debug() //hier lees ik de waarde uit, om te zien wat er effectief aan het gebeuren is
{
	if(lengte_signaal_gevonden == 0)  //als de lengte van het signaal nog niet bepaald is
	{
		printf("Commando %d\r\n", commando);

		for(uint16_t debugwaarde = 0; debugwaarde < 500; debugwaarde++)
		{
		printf("tijd[%d][%d] = %d \r\n", commando, debugwaarde, tijd[commando][debugwaarde]);
		}
	}
	else //als de lengte van het signaal bepaalt is
	{
		for(uint16_t debugwaarde = 0; debugwaarde <= lengte_signaal[commando] ; debugwaarde++)
			{
			printf("tijd[%d][%d] = %d \r\n", commando, debugwaarde, tijd[commando][debugwaarde]);
			}
	}

}
void whilelus(){

/*	if(HAL_GPIO_ReadPin(Button_GPIO_Port, Button_Pin)==0)
	{
		printf("ja\r\n");
		//IR_Read();
		IR_UNDERSTAND();
	}
	else
	{
		printf("no\r\n");
	}*/


	 	 if(NoSignal == 1)
	 	 {
		 IR_Read();
		 IR_UNDERSTAND();
	 	 }
	 	 else
	 	 {


	 		 if(HAL_GPIO_ReadPin(Button_GPIO_Port, Button_Pin)==0)
	 		 {
	 			IR_sent();
	 			HAL_Delay(100);
	 		 }
	 		 else
	 		 {
	 			IR_Carrier_OFF();
	 		 }

	 	 }

}


// Old Nota

///* DFR0669 */
///*GT911*/
///* GT911 interfaces with the host via 6 pins: VDD, GND, SCL, SDA, INT and RESET. */ //IMPORTANT INT AND RESET
//
////Backlight
//void DFR0669_Backlight_OFF(){
//	//HAL_GPIO_WritePin(Backlight_GPIO_Port, Backlight_Pin, 0);
//}
//
//void DFR0669_Backlight_ON(){
//	//HAL_GPIO_WritePin(Backlight_GPIO_Port, Backlight_Pin, 1);
//}
////Touch
//void CARL_DFR0669_Init_Touch(){
////	  HAL_GPIO_WritePin(Touch_int_GPIO_Port, Touch_int_Pin, 1);
////	  HAL_Delay(1); //>100µs
////	  HAL_GPIO_WritePin(RESET_Scherm_GPIO_Port, RESET_Scherm_Pin, 1);
////	  HAL_Delay(5); //5ms erna
////	  HAL_GPIO_WritePin(Touch_int_GPIO_Port, Touch_int_Pin, 0);
//}
