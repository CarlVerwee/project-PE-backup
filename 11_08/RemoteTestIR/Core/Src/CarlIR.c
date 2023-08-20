/*
 * CarlIR.c
 *
 *  Created on: 1 aug. 2023
 *      Author: Carl
 */
/* Some Extern Private variables -------------------------------*/

#include "CarlIR.h"
#include <stdio.h>
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim15;

/* Some Application defines -------------------------------*/
#define TIM_1us htim15
#define TIM_PWM_CARRIER htim1
#define TIM_PWM_CARRIER_CHANNEL TIM_CHANNEL_1

//Variables for IR

uint16_t geheugen[5000] = {150,69,69,69};
uint16_t geheugenplaatsen = 4;
uint16_t pulsecounter = 0;
uint8_t IR_READ_flag = 0;
uint16_t lengte;
uint16_t lengte_signaal[48] = {0,1,0,3};
uint16_t tijd[500];
uint8_t commando = 0;
uint16_t eerste_flank = 0;
uint32_t eerste_flank_plus_2sec = 0;
uint8_t IR_Done = 0;
uint8_t IR_ReadMark = 0;
uint32_t IR_ReadMark_First_time = 0;

void cleartijd()
{
	eerste_flank = 0;
	eerste_flank_plus_2sec = 0;
	IR_Done = 0;
	pulsecounter = 0;
	IR_ReadMark = 0;
	IR_ReadMark_First_time = 0;
	for(uint16_t i = 0; i < 500; i++)
	{
		tijd[i]=0;
	}
	HAL_TIM_Base_Stop(&TIM_1us); //timer stoppen
	TIM_1us.Instance -> CNT = 0;

}

void Interrupt_flank()
{

	if(IR_READ_flag == 1)
	{
		eerste_flank++;
		if(eerste_flank == 1) //eerste flank
			{
					HAL_TIM_Base_Start(&TIM_1us);
					eerste_flank_plus_2sec = HAL_GetTick() + 2000;
			}
		if(eerste_flank >= 2) // alle andere flanken
			{
				HAL_TIM_Base_Stop(&TIM_1us); //timer stoppen
				tijd[pulsecounter] = TIM_1us.Instance -> CNT; //timer opslaan
				lengte = pulsecounter;
				pulsecounter++;
				TIM_1us.Instance -> CNT = 0;
				HAL_TIM_Base_Start(&TIM_1us);
			}

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

void IR_UNDERSTAND(uint8_t *berekening, uint8_t *geen_signaal, uint8_t *lengte_signaal_gevonden)
{
	if(HAL_GetTick() >= eerste_flank_plus_2sec)
	{
		if(*geen_signaal == 0)
		{
	//geen_signaal in het begin op 0 zetten, geen signaal =0, wel een signaal = 1, lengte gevonden = 2
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
			*geen_signaal = 0;
		}
		else
		{
			printf("Er is iets gemeten %d \r\n", lengte_signaal[commando]);
			*geen_signaal = 1;
		}

		if(IR_ReadMark == 0)
		{
			IR_ReadMark_First_time = HAL_GetTick();
			IR_ReadMark++;
		}
		else
		{
		if(HAL_GetTick() >= IR_ReadMark_First_time + 5000)
		{
			printf("Geen signaal gevonden in 5 sec tijd\r\n");
			*geen_signaal = 4; //ga uit de whilelus

		}
		}
		}
	if(*geen_signaal == 1)   //als er een signaal gevonden is
		{

			if(*lengte_signaal_gevonden == 0)
			{

				/*for(uint16_t debugwaarde = 0; debugwaarde < 500 ; debugwaarde++)
				{
				 printf("tijd[%d] = %d \r\n", debugwaarde, tijd[debugwaarde]);
				}*/
				uint16_t waarde = 499;
				while(tijd[waarde] == 0)
				{
				waarde--;
				}
				lengte_signaal[*berekening] = waarde+1;//new +1
				printf("lengte signaal = %d\r\n", lengte_signaal[*berekening]);
				printf("trouwens berekening= %d\r\n", *berekening);
				*lengte_signaal_gevonden = 1;
				*geen_signaal=2; //ga uit de whilelus
			}
		}
	}

}



void IR_sent(uint16_t *tijd, uint16_t lengte_sig)
{
	printf("lengte in IR_sent is %d en eerste tijdswaarden is %d en de tweede %d \r\n", lengte_sig, tijd[0], tijd[1]);
   IR_READ_flag = 0; //er kan niks meer ingelezen worden
   for(uint8_t i = 0; i < lengte_sig; i++)
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
