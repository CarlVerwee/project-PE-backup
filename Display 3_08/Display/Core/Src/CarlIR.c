/*
 * CarlIR.c
 *
 *  Created on: 1 aug. 2023
 *      Author: Carl
 */
/* Some Extern Private variables -------------------------------*/

#include "CarlIR.h"
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

/* Some Application defines -------------------------------*/
#define TIM_1us htim2
#define TIM_PWM_CARRIER htim3
#define TIM_PWM_CARRIER_CHANNEL TIM_CHANNEL_2

//Variables for IR
uint16_t ruimte = 10000;
uint16_t geheugen[10000];
uint16_t geheugenplaatsen = 10;
uint16_t pulsecounter = 1 ;
uint8_t IR_READ_flag = 0;
uint16_t lengte;
uint8_t NoSignal = 1;
uint8_t lengte_signaal_gevonden = 0;
uint16_t lengte_signaal[48];
uint16_t tijd[500];
uint8_t commando = 0;

void Interrupt_flank()
{

	if(IR_READ_flag == 1)
	{
		HAL_TIM_Base_Stop(&TIM_1us); //timer stoppen
		//timer in het begin is steeds nul maar deze hebben we niet nodig

		tijd[pulsecounter] = TIM_1us.Instance -> CNT; //timer opslaan
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

		if(tijd[five_measurements] == 0)
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
			while(tijd[waarde] == 0)
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
		   while((TIM_1us.Instance->CNT) <= tijd[i]){}
	   }
	   else
	   {
		   HAL_TIM_Base_Stop(&TIM_1us);
		   TIM_1us.Instance->CNT = 0;
		   HAL_TIM_Base_Start(&TIM_1us);
		   IR_Carrier_OFF();
		   while((TIM_1us.Instance->CNT) <= tijd[i]){}

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
		printf("tijd[%d][%d] = %d \r\n", commando, debugwaarde, tijd[debugwaarde]);
		}
	}
	else //als de lengte van het signaal bepaalt is
	{
		for(uint16_t debugwaarde = 0; debugwaarde <= lengte_signaal[commando] ; debugwaarde++)
			{
			printf("tijd[%d][%d] = %d \r\n", commando, debugwaarde, tijd[debugwaarde]);
			}
	}

}
