/*
 * CarlTouch.c
 *
 *      Author: Carl Verwee
 */
//extern uint32_t tijd[500];
//extern uint32_t pulsecounter;
//uint32_t geheugen;

#include "CarlTouch.h"

extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim10;
extern uint8_t UartScreen;

#define myI2C hi2c1
#define Tim_Backlight htim10

uint8_t button = 0; //if button = 0 , dan is er geen knop ingeduwd
uint8_t device1 = 0;
uint8_t device2 = 0;
uint8_t device3 = 0;
uint8_t device4 = 0;
uint16_t Y_waarde;
uint16_t X_waarde;
uint16_t OldX;
uint16_t OldY;

void InitTouch()
{
	uint16_t device_address_write = 0xBA;
	uint16_t device_address_read = 0xBB;
	uint8_t dataWrite[] = {0x81,0x40};
	uint8_t ontvangen[11];
	HAL_I2C_Master_Transmit(&hi2c1, device_address_write, 0, 0 , HAL_MAX_DELAY); //new
	HAL_I2C_Master_Transmit(&hi2c1, device_address_write, dataWrite, 2 , HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, device_address_read, ontvangen, 11, HAL_MAX_DELAY); //
}
void Touch()
{
	uint16_t device_address_write = 0xBA;
	uint16_t device_address_read = 0xBB;
	uint8_t nulladdress = 0x00;
	uint8_t buffer[12];
	uint8_t buffer1[0];





		HAL_Delay(5);
		HAL_I2C_Mem_Read(&myI2C, device_address_read, 0x814E, 2, buffer1, 1, HAL_MAX_DELAY);
		if(buffer1[0] != 0x00)
		{
			HAL_I2C_Mem_Read(&myI2C, device_address_read, 0x814F, 2, buffer, 12, HAL_MAX_DELAY);
			if(buffer[4] == 1) //als buffer[3] het midden overschrijdt, dan gaat hij van 255 terug naar 0, en wordt buffer[4] op 1 gezet.
			{
				Y_waarde = 0xFF + buffer[3];
			}
			else
			{
				Y_waarde = buffer[3];
			}
			if(buffer[2] == 1)
			{
				X_waarde = 0xFF + buffer[1];
			}
			else
			{
				X_waarde = buffer[1];
			}
						if(UartScreen == 1)
							{
						printf("X= %d\r\n", X_waarde);
						printf("Y= %d\r\n", Y_waarde);
							}

			HAL_I2C_Mem_Write(&myI2C, device_address_write, 0x814E, 2, &nulladdress, 1, HAL_MAX_DELAY); //de flag uitzetten
		}


		//knop 1: x: 40 - 145
		//     	  y: 122 - 253
		//knop 2: x: 172 - 283
		//     	  y: 122 - 253
		//slidebar: x: 82 - 231
		// 			y: 333 - 372

		if((OldX != X_waarde)||(OldY != Y_waarde))  //is de X of Y dezelfde als de vorige waarde X/Y?
		{
			if(UartScreen == 1)
			{
			printf("OldX %d   xwaarde  %d\r\n", OldX, X_waarde);
			}
		if((X_waarde >= 0)&&(X_waarde <=79)&&(Y_waarde>=0)&&(Y_waarde <=40))
		{
			if(device1 == 0)
				{
					if(UartScreen == 1)
						{
						printf("device1\r\n");
						OldX = X_waarde;
						OldY = Y_waarde;
						device1=1;device2=0;device3=0;device4=0;
						}
				}
		}
		if((X_waarde >= 80)&&(X_waarde <=159)&&(Y_waarde>=0)&&(Y_waarde <=40))
		{
			if(device2 == 0)
				{
					if(UartScreen == 1)
						{
						printf("device2\r\n");
						OldX = X_waarde;
						OldY = Y_waarde;
						device1=0;device2=1;device3=0;device4=0;
						}
				}
		}
		if((X_waarde >= 160)&&(X_waarde <=239)&&(Y_waarde>=0)&&(Y_waarde <=40))
			{
			if(device3 == 0)
				{
					if(UartScreen == 1)
						{
						printf("device3\r\n");
						OldX = X_waarde;
						OldY = Y_waarde;
						device1=0;device2=0;device3=1;device4=0;
						}
				}
			}
		if((X_waarde >= 240)&&(X_waarde <=320)&&(Y_waarde>=0)&&(Y_waarde <=40))
			{
			if(device4 == 0)
				{
					if(UartScreen == 1)
						{
						OldX = X_waarde;
						OldY = Y_waarde;
						printf("device4\r\n");
						device1=0;device2=0;device3=0;device4=1;
						}
				}

			}
//		if((X_waarde >= 82)&&(X_waarde <=231)&&(Y_waarde>=360)&&(Y_waarde <=400))//slidebar //tim3 PWM
//		{
//
//			uint32_t helderheid = 231 - X_waarde;
//			Tim_Backlight.Instance->CCR1 = helderheid;
//
//
//		}


		//HAL_GPIO_TogglePin(Backlight_GPIO_Port, Backlight_Pin);
		HAL_Delay(100);
		}
}

void ReadButton(uint8_t knop){
	const uint16_t X_waarde_knoppen[] = {23,219,121,23,121,219,121,23,23,121,219,219};
	const uint16_t Y_waarde_knoppen[] = {50,50,90,175,175,175,260,298,393,393,298,393};
	/*
	 * knop1: Power
	 * knop2: Source
	 * knop3: ^
	 * knop4: <
	 * knop5: OK
	 * knop6: >
	 * knop7: v
	 * knop8: Volume+
	 * knop9: Volume-
	 * knop10: Mute
	 * knop11: Channel+
	 * knop12: Channel-
	 */
	if((OldX != X_waarde)||(OldY != Y_waarde))  //is de X of Y dezelfde als de vorige waarde X/Y?
	{
		if((X_waarde >= X_waarde_knoppen[knop-1])&&(X_waarde <=X_waarde_knoppen[knop-1]+75)&&(Y_waarde >= Y_waarde_knoppen[knop-1])&&(Y_waarde <=Y_waarde_knoppen[knop-1]+75))
		{
			button = knop;
			printf("button %d\r\n", button);
		}

	}



}
//if((X_waarde >= 80)&&(X_waarde <=159)&&(Y_waarde>=0)&&(Y_waarde <=40))
//		{
//			if(knop2 == 0)
//				{
//					if(UartScreen == 1)
//						{
//						printf("knop2\r\n");
//						OldX = X_waarde;
//						OldY = Y_waarde;
//						knop1=0;knop2=1;knop3=0;knop4=0;
//						}
//				}
//		}
//uint16_t color= ILI9488_WHITE;
////Power
//DrawRectangle(23, 50, 75, 75, color, ILI9488_RED, 5);
////Src
//DrawRectangle(219, 50, 75, 75, color, ILI9488_BLACK, 5);
//
////^
//DrawRectangle(121, 90, 75, 75, color, ILI9488_DARKGREY, 5);
////<
//DrawRectangle(23, 175, 75, 75, color, ILI9488_DARKGREY, 5);
////OK
//DrawRectangle(121, 175, 75, 75, color, ILI9488_DARKGREY, 5);
////>
//DrawRectangle(219, 175, 75, 75, color, ILI9488_DARKGREY, 5);
////V
//DrawRectangle(121, 260, 75, 75, color, ILI9488_DARKGREY, 5);
//
////volume up
//DrawRectangle(23, 298, 75, 75, color, ILI9488_DARKGREY, 5);
////volume down
//DrawRectangle(23, 393, 75, 75, color, ILI9488_DARKGREY, 5);
////mute
//DrawRectangle(121, 393, 75, 75, color, ILI9488_DARKGREY, 5);
////channel up
//DrawRectangle(219, 298, 75, 75, color, ILI9488_DARKGREY, 5);
////channel down
//DrawRectangle(219, 393, 75, 75, color, ILI9488_DARKGREY, 5);



