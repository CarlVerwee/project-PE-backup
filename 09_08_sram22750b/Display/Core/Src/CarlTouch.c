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
void Touch(uint8_t *Sensingtouch, uint16_t *X_waarde, uint16_t *Y_waarde)
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
			*Sensingtouch = 1;
			HAL_I2C_Mem_Read(&myI2C, device_address_read, 0x814F, 2, buffer, 12, HAL_MAX_DELAY);
			if(buffer[4] == 1) //als buffer[3] het midden overschrijdt, dan gaat hij van 255 terug naar 0, en wordt buffer[4] op 1 gezet.
			{
				*Y_waarde = 0xFF + buffer[3];
			}
			else
			{
				*Y_waarde = buffer[3];
			}
			if(buffer[2] == 1)
			{
				*X_waarde = 0xFF + buffer[1];
			}
			else
			{
				*X_waarde = buffer[1];
			}
						if(UartScreen == 1)
							{
						printf("X= %d\r\n", *X_waarde);
						printf("Y= %d\r\n", *Y_waarde);
							}

			HAL_I2C_Mem_Write(&myI2C, device_address_write, 0x814E, 2, &nulladdress, 1, HAL_MAX_DELAY); //de flag uitzetten
		}
		else
		{
			*Sensingtouch = 0;
		}
		HAL_Delay(100);
		}

void ReadDevice(uint8_t *device, uint16_t *X_w, uint16_t *Y_w)
{
	uint16_t X_waarde = *X_w;
	uint16_t Y_waarde = *Y_w;
	const uint16_t X_waarde_knoppen[] = {0,80,160,240};
	const uint16_t Y_waarde_knoppen[] = {0,40};
	uint8_t apparaat = 0; //if device = 0 , dan is er geen device geselecteerd

		for(uint8_t i = 1; i<= 4; i++)
		{
		if((X_waarde >= X_waarde_knoppen[i-1])&&(X_waarde <=X_waarde_knoppen[i-1]+79)&&(Y_waarde >= Y_waarde_knoppen[0])&&(Y_waarde <=Y_waarde_knoppen[1]))
		{
			*device = i;
			printf("device %d\r\n", i);

		}
		}


}
void ReadButton(uint8_t *button, uint16_t *X_w, uint16_t *Y_w){
	uint16_t X_waarde = *X_w;
	uint16_t Y_waarde = *Y_w;
	const uint16_t X_waarde_knoppen[] = {23,219,121,23,121,219,121,23,23,121,219,219,23,219};
	const uint16_t Y_waarde_knoppen[] = {50,50,90,175,175,175,260,298,393,393,298,393,245,245};
	 //if button = 0 , dan is er geen knop ingeduwd
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
	 * knop13: Add
	 * knop14: Delete
	 */


		for(uint8_t i = 1; i<= 14; i++)
		{
			if(i <= 12)
			{
				//hier lees ik mijn touch uit
				if((X_waarde >= X_waarde_knoppen[i-1])&&(X_waarde <=X_waarde_knoppen[i-1]+75)&&(Y_waarde >= Y_waarde_knoppen[i-1])&&(Y_waarde <=Y_waarde_knoppen[i-1]+75))
				{

					*button = i;
					printf("geduwd op button %d\r\n", i);
				}
			}
			else
			{
				if((X_waarde >= X_waarde_knoppen[i-1])&&(X_waarde <=X_waarde_knoppen[i-1]+75)&&(Y_waarde >= Y_waarde_knoppen[i-1])&&(Y_waarde <=Y_waarde_knoppen[i-1]+48))
				{

				*button = i;
				printf("geduwd op button %d\r\n", i);
				}
			}
		}

}
