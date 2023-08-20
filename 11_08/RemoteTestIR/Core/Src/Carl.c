/*
 * Carl.c
 *
 *  Created on: Jul 30, 2023
 *      Author: Carl
 */

#include "Carl.h"
//#include <stdio.h>


uint8_t UartScreen = 1; //Als je de Serial Output wilt zien voor te debuggen
						//Zet je deze op 1
						//Indien niet; op 0

//Variables for IR
extern uint16_t ruimte;
extern uint16_t geheugen[10000]; // {8, 4, 4, 6, 6, 8,...}
extern uint16_t geheugenplaatsen;
extern uint16_t lengte_signaal[48]; //{1, 2, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0};
extern uint16_t tijd[500];

void Starting()
{
	  printf("\r\n-----------------------------------------------------------------------\r\n");
	  printf("  ######  ########    ###    ########  ######## #### ##    ##  ######\r\n");
	  printf(" ##    ##    ##      ## ##   ##     ##    ##     ##  ###   ## ##    ##\r\n");
	  printf(" ##          ##     ##   ##  ##     ##    ##     ##  ####  ## ## \r\n");
	  printf("  ######     ##    ##     ## ########     ##     ##  ## ## ## ##   #### \r\n");
	  printf("       ##    ##    ######### ##   ##      ##     ##  ##  #### ##    ##\r\n");
	  printf(" ##    ##    ##    ##     ## ##    ##     ##     ##  ##   ### ##    ##  \r\n");
	  printf("  ######     ##    ##     ## ##     ##    ##    #### ##    ##  ######\r\n");
	  printf("-----------------------------------------------------------------------\r\n\r\n");
}
void All_Carl_Inits()
{
	if(UartScreen == 1)
	{
	Starting();
	}
	InitTouch();
	DisplayBegin();
	IR_Carrier_OFF();

	//logo();
	mainscreen();
//	WriteToGeheugen(3, 12);
//	DeleteFromGeheugen(3, 11);
//	DeleteFromGeheugen(1, 1);
	//Wat zit er eigenlijk in mijn geheugen?
	for(uint8_t i = 1; i <= 4; i++)
	{
		uint16_t *tijd;
		for(uint8_t j = 1; j <= 12; j ++)
		{
			//tijd = TakefromGeheugen(i, j);
		}
	}
}

void WriteToGeheugen(uint8_t *dev, uint8_t *kn)
{
	uint8_t device = *dev;
	uint8_t knop = *kn;
	//Elke knop heeft een lengte van IR signaal
	//Device1:   knop 1 t.e.m. 12    lengte_signaal[0] t.e.m. lengte_signaal[11]
	//Device2:   knop 1 t.e.m. 12    lengte_signaal[12] t.e.m. lengte_signaal[23]
	//Device3:   knop 1 t.e.m. 12    lengte_signaal[24] t.e.m. lengte_signaal[35]
	//Device4:   knop 1 t.e.m. 12    lengte_signaal[36] t.e.m. lengte_signaal[47]
	//stel singaal1 is 12 groot


	//stel Device 3 knop 4
	//berekening = 2*12 + 3 = 27
	uint8_t berekening = ((device-1) * 12) + knop-1 ; //zoek de plaats van lengte_signaal voor het knopje van het apparaat

	printf("%d\r\n", berekening);
	for(uint16_t i = 0; i < lengte_signaal[berekening] ; i++)
	{
		geheugen[geheugenplaatsen+i] = tijd[i];
	}
	geheugenplaatsen += lengte_signaal[berekening];

	printf("geheugenplaatsen %d\r\n", geheugenplaatsen);
}
/*
void DeleteFromGeheugen(uint8_t *dev, uint8_t *kn)
{
	uint8_t device = *dev;
	uint8_t knop = *kn;
	uint16_t plaatservoor = 0;
	uint8_t berekening = ((device-1) * 12) + knop-1 ;
	//Hoeveel plaats neemt de eventuele voorgaande knoppen in?
	for(uint8_t i = 0; i < berekening; i++)
	{
		plaatservoor += lengte_signaal[i];
	}
	uint16_t plaatserna = plaatservoor + lengte_signaal[berekening];
	printf("plaatservoor %d plaatserna %d\r\n", plaatservoor, plaatserna);
	//nu gaan we de geheugenplaats dat we moeten verwijderen, met nullen opvullen
	for(uint16_t i = plaatservoor; i < plaatserna; i ++)
	{
		geheugen[i]=0;
	}
	uint16_t index = 0;
	//vervolgens gaan we het geheugen optimaliseren,
	for(uint16_t i = plaatservoor; i < geheugenplaatsen - plaatservoor; i++)
	{

		uint16_t a = geheugen[plaatserna + index];
		geheugen[i]= a;
		index++;
	}
	lengte_signaal[berekening] = 0;
	geheugenplaatsen -= (plaatserna - plaatservoor);
	//En alles achter het geheugen opvullen met 0
	for(uint16_t i = geheugenplaatsen; i < ruimte; i++ )
	{
		geheugen[i] = 0;
	}
	//vervolgens gaan we geheugenplaatsen controleren, dat het wel goed gewerkt heeft
	uint8_t controle = 0;
	while(geheugen[controle] != 0)
	{
		controle++;
	}
	if(controle == geheugenplaatsen)
	{
	printf("geheugenplaatsen = %d geheugen = %d\n\r", geheugenplaatsen, controle);
	}
}
*/
void TakefromGeheugen(uint8_t *dev, uint8_t *kn, uint16_t *DataFromGeheugen, uint8_t *controle)
{
		uint8_t device = *dev;
		uint8_t knop = *kn;
		uint16_t plaatservoor = 0;
		uint8_t berekening = ((device-1) * 12) + knop-1 ;
		if(lengte_signaal[berekening] == 0)
		{
			printf("device: %d knop: %d is niet ingesteld!\r\n", device, knop);
			*controle=0;
		}
		else
		{
			*controle=1;
			//Hoeveel plaats neemt de eventuele voorgaande knoppen in?
			for(uint8_t i = 0; i < berekening; i++)
			{
				plaatservoor += lengte_signaal[i];
			}
			uint16_t plaatserna = plaatservoor + lengte_signaal[berekening];

			//printf("device: %d knop: %d = ", device, knop);
			uint16_t index = 0;
			for(uint16_t i = plaatservoor; i < plaatserna; i++)
			{

				DataFromGeheugen[index] = geheugen[i];
				//printf("%d ", DataFromGeheugen[index]);
				index++;
			}
			//printf("\r\n");
		}
}

uint8_t device = 0;
uint8_t OldDevice = 0;
uint8_t button = 0;
uint8_t OldButton = 0;
uint8_t aanraak = 0;


uint16_t Xvalue = 160;
uint16_t Yvalue = 363;
uint16_t OldX = 0;
uint16_t OldY = 0;

uint8_t tijden = 0;
void whilelus(){

	Touch(&aanraak, &Xvalue, &Yvalue);

	if((OldX != Xvalue && OldY != Yvalue))
	{
		OldX = Xvalue;
		OldY = Yvalue;
		ReadDevice(&device, &Xvalue, &Yvalue);
		if(device != OldDevice)
		{
			OldDevice=device;
			DevicePressed(&device);
			Xvalue = 160; Yvalue = 363; //randomwaarde waar geen knop op staat
		}
		if(device != 0)
		{
			ReadButton(&button, &Xvalue, &Yvalue);
			if(button != 0 && aanraak != 0)
			{
			buttonPressed(&device, &button, &aanraak, &Xvalue, &Yvalue);
			Xvalue = 160; Yvalue = 363; //randomwaarde waar geen knop op staat
			button=0;
			}
		}
	}/*
	else
	{

	}*/
	// else , halgettick , nmeet hoelang, want om de seconde send ik


//printf("device:%d button: %d   aanraak %d\r\n", device, button, aanraak);
//printf("X:%d Y:%d ", Xvalue, Yvalue);
}






//	Touch();
//	for(uint8_t i = 1; i <= 12 ; i++ )
//	{
//	ReadButton(i);
//	}



//	if(device1 == 1 && olddevice1 == 0)
//	{
//		printf("Knop1 UITGEVOERD\r\n");
//		device1screen();
//		device1 = 0;
//
//	}
//	if(device2 == 1 && olddevice2 == 0)
//	{
//		printf("Knop2 UITGEVOERD\r\n");
//		device2screen();
//		device2 = 0;
//
//	}
//	if(device3 == 1 && olddevice3 == 0)
//	{
//		printf("Knop3 UITGEVOERD\r\n");
//		device3screen();
//		device3 = 0;
//
//	}
//	if(device4 == 1 && olddevice4 == 0)
//	{
//		printf("Device4 UITGEVOERD\r\n");
//		device4screen();
//		device4 = 0;
//
//	}
/*


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
*/


