/*
 * CarlDisplay.c
 *
 *  Created on: Jul 27, 2023
 *      Author: Carl Verwee
 *
 * Just some notes:
 * CS is not active HIGH
 * RST is not active LOW
 * DC is not active LOW
 * het werkt tussen de 10 - 20MHz
 * kheb 100MHz met prescaler van 8 = (technisch gzn 9)
 * 0 tot 8 = 9
 * 100/9= 11,111... Mhz
 */

/* Some Includes */
#include "CarlDisplay.h"
#include "Carl.h"
/* Some Extern variables -------------------------------*/
extern SPI_HandleTypeDef hspi2;
extern uint8_t UartScreen;

extern uint16_t lengte_signaal[48];
extern uint8_t IR_READ_flag;
extern uint16_t tijd[500];
/* Some Variables */
uint16_t Display_width = 320;
uint16_t Display_height = 480;
/* Some Defines */
#define CS_Port DISPLAY_CS_GPIO_Port
#define DC_Port DISPLAY_DC_GPIO_Port
#define RST_Port DISPLAY_RESET_GPIO_Port
#define CS_Pin DISPLAY_CS_Pin
#define DC_Pin DISPLAY_DC_Pin
#define RST_Pin DISPLAY_RESET_Pin

#define mySPI hspi2


//pg 87
#define Display_Software_Reset 0x01
//#define Display_Sleep 0x10
#define Display_ExitSleep 0x11
#define Display_On 0x29
#define Display_Off 0x28
//#define Display_Partial_Mode_On 0x12
//#define Display_Normal_Mode_On 0x13
//#define Display_Tearing_Effect_Line_Off 0x34
//#define Display_Idle_Mode_Off 0x38
#define Display_Write_To_RAM 0x2C

/* Some Colors */
#define ILI9488_BLACK      			0x0000      /*   0,   0,   0 */
#define ILI9488_NAVY				0x000F      /*   0,   0, 128 */
#define ILI9488_DARKGREEN   		0x03E0      /*   0, 128,   0 */
#define ILI9488_DARKCYAN    		0x03EF      /*   0, 128, 128 */
#define ILI9488_MAROON      		0x7800      /* 128,   0,   0 */
#define ILI9488_PURPLE      		0x780F      /* 128,   0, 128 */
#define ILI9488_OLIVE       			0x7BE0      /* 128, 128,   0 */
#define ILI9488_LIGHTGREY   		0xC618      /* 192, 192, 192 */
#define ILI9488_DARKGREY    		0x7BEF      /* 128, 128, 128 */
#define ILI9488_BLUE        			0x001F      /*   0,   0, 255 */
#define ILI9488_GREEN       		0x07E0      /*   0, 255,   0 */
#define ILI9488_CYAN        			0x07FF      /*   0, 255, 255 */
#define ILI9488_RED         			0xF800      /* 255,   0,   0 */
#define ILI9488_MAGENTA     		0xF81F      /* 255,   0, 255 */
#define ILI9488_YELLOW      		0xFFE0      /* 255, 255,   0 */
#define ILI9488_WHITE       			0xFFFF      /* 255, 255, 255 */
#define ILI9488_ORANGE      		0xFD20      /* 255, 165,   0 */
#define ILI9488_GREENYELLOW 	0xAFE5      /* 173, 255,  47 */
#define ILI9488_PINK        			0xF81F

/* Some Easy Functions */ 
void CS_HIGH() //High = The chip is not selected and not accessible pg23
{HAL_GPIO_WritePin(CS_Port, CS_Pin, 1);}
void CS_LOW()  //Low = The chip is selected and accessible pg23
{HAL_GPIO_WritePin(CS_Port, CS_Pin, 0);}
void DC_HIGH() //High = Parameter pg23
{HAL_GPIO_WritePin(DC_Port, DC_Pin, 1);}
void DC_LOW()  //Low = command pg23
{HAL_GPIO_WritePin(DC_Port, DC_Pin, 0);}
void RST_HIGH() 
{HAL_GPIO_WritePin(RST_Port, RST_Pin, 1);}
void RST_LOW() //initialize with a low input pg23
{HAL_GPIO_WritePin(RST_Port, RST_Pin, 0);}

//#define TIM_1us htim2

void DisplayBegin(void)
{
	RST_LOW(); //initialize with a low input pg23
	HAL_Delay(500);
	RST_HIGH();
	HAL_Delay(500);
	writecommand(0xE0);
	writedata(0x00);
	writedata(0x03);
	writedata(0x09);
	writedata(0x08);
	writedata(0x16);
	writedata(0x0A);
	writedata(0x3F);
	writedata(0x78);
	writedata(0x4C);
	writedata(0x09);
	writedata(0x0A);
	writedata(0x08);
	writedata(0x16);
	writedata(0x1A);
	writedata(0x0F);

	writecommand(0XE1);
	writedata(0x00);
	writedata(0x16);
	writedata(0x19);
	writedata(0x03);
	writedata(0x0F);
	writedata(0x05);
	writedata(0x32);
	writedata(0x45);
	writedata(0x46);
	writedata(0x04);
	writedata(0x0E);
	writedata(0x0D);
	writedata(0x35);
	writedata(0x37);
	writedata(0x0F);

	writecommand(0XC0);      //Power Control 1 (pg 237)
							
	writedata(0x17);    //Vreg1out =  5V zie pg 237
	writedata(0x15);    //Verg2out = -4.87V zie pg 237
							//pg 25:
							//DDVDH = Power supply for the source driver and VCOM driver. 
							//	Input voltage from the set-up circuit (4.5 to 6V). 
							//DDVDL = Power supply for the source driver and VCOM driver. 
							//	Input voltage from the set-up circuit (-6 to -4.5V).
							
							//Op de DFR0669 staat een LevelShifter, zodat het op 5V werkt.
							

	writecommand(0xC1);      //Power Control 2 (pg 239)
	writedata(0x41);    //VGH,VGL
						//VCI = 2.5 to 3.3V
						//Stel VCI = 3v
						//VGH = 3v x 6 = 18v 
						//VGL = -3v x 4 = -12V
						//VGH - VGL = 18V -(-)12V = 30V 
						//30V < 32v = ok
						
						//(!To reduce power set a small factor!)
						// = 3'h1 = 001
						// 0100 0XXX 
						// 0100 0001 = 41h  (65 decimaal)
						

	writecommand(0xC5);      //VCOM control pg243
	writedata(0x00);		 //1e parameter standaard 00h
	writedata(0x12);    	 //VCOM = -1.71    |VCOM| x 2 = VREG = 3.42V
	writedata(0x80);		// ? 

	writecommand(0x36);      //Memory Access pg192
	writedata(0x48);		//MY = 0				//in vb 0x48
							//MX = 0
							//MV = 0
							//ML = 0
							//BGR = 0
							//MH = 0
							//=0h

	writecommand(0x3A);		// Interface Pixel Format (RGB 565 standaard 3Ah pg123
	writedata(0x66); 	  	//pg 200 --> 16bits/pixel = 55 (decimal = 85)
							//55 = 0101 0101

	writecommand(0XB0);      // Interface Mode Control pg219
	writedata(0x80);     			 //SDO NOT USE
									//1000 0000
									//High enable for RGB interface,
									//data fetched at the rising time,
									//and low level sync clock
									
	writecommand(0xB1);      //Frame rate pg 221
	writedata(0xA0);    //60Hz      NOG UITREKENEN

	writecommand(0xB4);      //Display Inversion Control  pg 225
	writedata(0x02);    //2-dot inversion (standard)

	writecommand(0XB6); //Display Function Control  RGB/MCU Interface Control pg228

	writedata(0x02);    //MCU // 1e parameter standard    question Bypass= Memory or Shift register? 
	writedata(0x02);    //Source,Gate scan direction so frames at 84ms

	writecommand(0XE9);      // Set Image Function pg271 
	writedata(0x00);    // Disable 24 bit data

	writecommand(0xF7);      // Adjust Control   pg 276
	writedata(0xA9);  //standard 1e parameter
	writedata(0x51);  //standard 2e parameter
	writedata(0x2C);  //standard 3e parameter
	writedata(0x82);    // D7 stream, loose   //standard 4e parameter

	writecommand(Display_ExitSleep);    //Exit Sleep

	HAL_Delay(120);

	writecommand(Display_On);    //Display on


	
}

void writecommand(uint8_t cmdo)
{
	uint8_t commando = cmdo;
	DC_LOW(); //Low = command pg23
	CS_LOW(); //The chip is selected and accessible pg23
	HAL_SPI_Transmit(&mySPI, &commando, 1, HAL_MAX_DELAY); //Size = bytes
	CS_HIGH(); //The chip is not selected and not accessible pg23
	


}
void writedata(uint8_t dta )
{
	uint8_t data = dta;
	DC_HIGH(); //High = Parameter pg23
	CS_LOW(); //The chip is selected and accessible pg23
	HAL_SPI_Transmit(&mySPI, &data, 1, HAL_MAX_DELAY); //Size = bytes
	CS_HIGH(); //The chip is not selected and not accessible pg23
	

	//onderstaande is een mopje, aangezien ik een transmitOnly gebruik, kan ik het onmogelijk nakijken
	//als ik het scherm uitschakel, krijg ik ook geen errors
	//wel leuk om optimistisch te zijn, maar dit doet niks
	//uint8_t StateSpiHulp = HAL_SPI_GetState(&hspi2);
	//uint8_t ErrorSpiHulp = HAL_SPI_GetError(&hspi2);
	//printf("    data: 0x %x Spi_State:%d Error: %d\r\n", dta, StateSpiHulp, ErrorSpiHulp);
}
void setAddrWindow(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1)
{
	//pg 175 -> Column Address Set = 2Ah
	writecommand(0x2A);
	
	writedata(x0 >> 8); //?
	writedata(x0 & 0xFF);     // XSTART ?
	writedata(x1 >> 8); //?
	writedata(x1 & 0xFF);     // XEND //?
	
	//pg 177 -> Page Address Set = 2Bh = Row Address Set 
	writecommand(0x2B); // Row addr set
	writedata(y0 >> 8); //?
	writedata(y0 & 0xff);     // YSTART   ?
	writedata(y1 >> 8); // ?
	writedata(y1 & 0xff);     // YEND //?
	writecommand(Display_Write_To_RAM);
	
}
void drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	setAddrWindow(x, y, x + 1, y + 1);
	DC_HIGH();
	CS_LOW();
	write16BitColor(color);
	CS_HIGH();
}

/*
void DrawCircle(uint16_t x, uint16_t y, uint16_t diameter, uint8_t border, uint16_t bordercolor, uint16_t contentcolor)
{
	/*
	 * xy 				pixelT
	 * 				A1			B1
	 * 			An					Bn
	 * 	pixelL							pixelR
	 * 			Cn					Dn
	 *				C1			D1
	 * 					pixelB
	 */
/*Diameter moet even zijn!!!*/
	/* Possible with %2 for security and also check if diameter is more then 1, so we draw a circle and not just a pixel
	 * But all this checks are not important, cause the programmer Carl knows.
	 */
/*
uint16_t pixelTx =  x + (diameter/2);
uint16_t pixelTy= 	y;
uint16_t pixelLx =  x;
uint16_t pixelLy= 	y + (diameter/2);
uint16_t pixelRx =  x + diameter;
uint16_t pixelRy= 	y+ (diameter/2);
uint16_t pixelBx =  x + (diameter/2);
uint16_t pixelBy= 	y + diameter;

drawPixel(pixelTx, pixelTy, bordercolor);
for(uint16_t i = 1; i < ; i++)

}*/
void DrawVolumebuttonUp()
{

}
void DrawVolumebuttonDown()
{

}

void write16BitColor(uint16_t color){
	//565  = rrrr rggg gggb bbbb
	//F800 = 1111 1000 0000 0000 //11 nullekes
	//07E0 = 0000 0111 1110 0000 //5 nulleks
	//001F = 0000 0000 0001 1111
	 uint8_t r = (color >> 8) & 0Xf8; //& 0xF800) >> 11;
	 uint8_t g = (color >> 3) & 0Xfc; //& 0x07E0) >> 5;
	 uint8_t b = (color <<3); //& 0x001F;
//uint8_t r = 0xff;
//uint8_t g = 0x00;
//uint8_t b = 0x00;
	 //2^5 = 32  (0 tot 31)
	 //2^6 = 64  (0 tot 63)
	 //2^5 = 32 (0 tot 31)
	  //r = (r * 255) / 31;
	  //g = (g * 255) / 63;
	  //b = (b * 255) / 31;
	 writedata(r);
	 writedata(g);
	 writedata(b);
	  //HAL_SPI_Transmit(&mySPI, &r, 1, HAL_MAX_DELAY); //Size = bytes
	  //HAL_SPI_Transmit(&mySPI, &g, 1, HAL_MAX_DELAY); //Size = bytes
	  //HAL_SPI_Transmit(&mySPI, &b, 1, HAL_MAX_DELAY); //Size = bytes
}

void DrawImage(uint16_t x, uint16_t y , uint16_t w , uint16_t h, uint16_t *Image)
{
	uint32_t PixelIndex = 0;

	if ((x >= Display_width) || (y >= Display_height))
		return;
	if ((x + w - 1) >= Display_width)
		w = Display_width - x;
	if ((y + h - 1) >= Display_height)
		h = Display_height - y;

	setAddrWindow(x, y, x + w - 1, y + h - 1);

	DC_HIGH();
	CS_LOW();

	for (y = h; y > 0; y--) {
		for (x = w; x > 0; x--) {

			write16BitColor(Image[PixelIndex]);
			PixelIndex++;
		}
	}

	CS_HIGH();



}
void DrawRectangle(uint16_t x, uint16_t y, uint16_t h, uint16_t w, uint16_t color, uint16_t contentcolor, uint8_t border)
{


	//lijntje horizontaal 1
	for(uint8_t j = 0; j <= border; j++)
	{
		for(uint16_t i = x; i <= (x+w); i++)
		{
			drawPixel(i, y+j, color);
		}
	}

//lijntje verticaal 1
for(uint8_t j = 0; j <= border; j++)
{
	for(uint16_t i = y; i <= (y+h); i++)
	{
		drawPixel(x+j, i, color);
	}
}
//lijntje verticaal 2
for(uint8_t j = 0; j <= border; j++)
{
	for(uint16_t i = y; i <= (y+h); i++)
	{
		drawPixel((x+w)-j, i, color);
	}
}

//lijntje horizontaal 2
for(uint8_t j = 0; j <= border; j++)
{
	for(uint16_t i = x; i <= (x+w); i++)
	{
		drawPixel(i, (y+h)-j, color);
	}
}
//contentcolor
fillRect(x+border, y+border, w-(border*2), h-(border*2), contentcolor);



}
void fillRect(uint16_t x, uint16_t y , uint16_t w , uint16_t h, uint16_t color)
{
	if ((x >= Display_width) || (y >= Display_height))
		return;
	if ((x + w - 1) >= Display_width)
		w = Display_width - x;
	if ((y + h - 1) >= Display_height)
		h = Display_height - y;

	setAddrWindow(x, y, x + w - 1, y + h - 1);

	DC_HIGH();
	CS_LOW();

	for (y = h; y > 0; y--) {
		for (x = w; x > 0; x--) {

			write16BitColor(color);
		}
	}

	CS_HIGH();



}


void DevicePressed(uint8_t *apparaat)
{
	uint16_t color;
	uint16_t contentcolor;
	uint8_t device = *apparaat;
	switch(device)
	{
	case 1:
		color = ILI9488_BLUE;
		break;
	case 2:
		color = ILI9488_YELLOW;
		break;
	case 3:
		color = ILI9488_GREEN;
		break;
	case 4:
		color = ILI9488_WHITE;
		break;
	default:
		break;
	}
	for(uint8_t knop = 1; knop <= 14; knop++)
	{
		if(knop <= 12)
		{
			uint8_t berekening = ((device-1) * 12) + knop-1 ;
			if(lengte_signaal[berekening] != 0)
			{
				if(knop == 1)
				{
					contentcolor = ILI9488_RED;
				}
				else
				{
					contentcolor = ILI9488_DARKGREY;
				}

			}
			else
			{
			contentcolor = ILI9488_BLACK;
			}
		}
		switch(knop)
		{
		case 1:
			//Power
			DrawRectangle(23, 50, 75, 75, color, contentcolor, 5);
			break;
		case 2:
			//Src
			DrawRectangle(219, 50, 75, 75, color, contentcolor, 5);
			break;
		case 3:
			//^
			DrawRectangle(121, 80, 75, 75, color, contentcolor, 5);
			break;
		case 4:
			//<
			DrawRectangle(23, 165, 75, 75, color, contentcolor, 5);
			break;
		case 5:
			//OK
			DrawRectangle(121, 165, 75, 75, color, contentcolor, 5);
			break;
		case 6:
			//>
			DrawRectangle(219, 165, 75, 75, color, contentcolor, 5);
			break;
		case 7:
			//V
			DrawRectangle(121, 250, 75, 75, color, contentcolor, 5);
			break;
		case 8:
			//volume up
				DrawRectangle(23, 298, 75, 75, color, contentcolor, 5);
			break;
		case 9:
			//volume down
				DrawRectangle(23, 393, 75, 75, color, contentcolor, 5);
			break;
		case 10:
			//mute
				DrawRectangle(121, 393, 75, 75, color, contentcolor, 5);
			break;
		case 11:
			//channel up
				DrawRectangle(219, 298, 75, 75, color, contentcolor, 5);
			break;
		case 12:
			//channel down
				DrawRectangle(219, 393, 75, 75, color, contentcolor, 5);
			break;
		case 13:
			//Add
				DrawRectangle(23, 245, 48, 75, ILI9488_GREENYELLOW, ILI9488_GREENYELLOW, 0);
			break;
		case 14:
			//Delete
					DrawRectangle(219, 245, 48, 75, ILI9488_RED, ILI9488_RED, 0);
			break;
		default:
			break;
		}
	}





}
void buttonPressed(uint8_t *dev, uint8_t *knop, uint8_t *Sensingtouch, uint16_t *X_w, uint16_t *Y_w)
{
uint8_t device = *dev;
uint8_t button = *knop;
uint8_t aanraak = *Sensingtouch;
uint16_t X_waarde = *X_w;
uint16_t Y_waarde = *Y_w;

uint8_t controle = 0;
uint8_t berekening = ((device-1) * 12) + button-1 ;
	 if(button >= 13) //Is de knop een ADD/Delete button?
	 {
		 switch(button)
		 {
		 case 13: //Add
			 //Add
			 button = 0;
			 cleartijd();
			 X_waarde = 160; Y_waarde = 363;

			 printf("button %d , duw op een knop van de andere afstandsbediening, die je wilt instellen\r\n", button);
			 while(button == 0)
			 {
				 Touch(&aanraak, &X_waarde, &Y_waarde);
				 ReadButton(&button, &X_waarde, &Y_waarde);
				 berekening = ((device-1) * 12) + button-1;
				 //ReadDevice(&device, &X_waarde, &Y_waarde);
				 if(button > 12 || lengte_signaal[berekening] != 0 ) //als add/delete knop ingesteld is, of als knop al ingesteld is
				 {
					 button=0;
				 }
			 }
			 berekening = ((device-1) * 12) + button-1;
			 ColorToSelectedbutton(&button, &device, 0x07E0);
			 IR_Read();
			 uint8_t nosignal = 0; //In het begin gaan we er vanuit dat er geen signaal is
			 uint8_t lengte_signaal_gevonden = 0; //In het begin gaan we er vanuit dat er nog geen lengte van het signaal gevonden is
			 while((nosignal != 2) && (nosignal != 4))
			 {
			 IR_UNDERSTAND(&berekening, &nosignal, &lengte_signaal_gevonden);
			 }
			 IR_READ_flag = 0;
			 if(nosignal == 2)
			 {
				 WriteToGeheugen(&device,&button);
			 	 ColorToSelectedbutton(&button, &device, 0x7BEF);
			 }
			 else  //nosignal == 4
			 {
				 ColorToSelectedbutton(&button, &device, 0x0000);
			 }
			 button=0;
			 break;
		 case 14: //Del
			 //Delete
			 /*
			 button = 0;
			 X_waarde = 160; Y_waarde = 363;
			 while(button == 0)
			 {
				 Touch(&aanraak, &X_waarde, &Y_waarde);
				 ReadButton(&button, &X_waarde, &Y_waarde);
				// ReadDevice(&device, &X_waarde, &Y_waarde);
				if(button > 12)
				{
					button=0;
				}
			 }
			 	 ColorToSelectedbutton(&button, 0xF800);
			 	 printf("button %d , duw op een knop van de andere afstandsbediening, die je wilt instellen\r\n", button);
			 	 IR_Read();
*/
			 break;
		 default:
			 break;
		 }
	 }
	 else
	 {
		 //Heeft ingeduwde knop tijdswaarden? Ja dan is controle = 1
		 uint16_t tijd_geheugen[500];
		 TakefromGeheugen(&device, &button, &tijd_geheugen, &controle);
		 if(controle == 0)
		 {
			 printf("knop is niet ingesteld\r\n");

		 }
		 else
		 {


			 	 	 uint16_t lensignaal = lengte_signaal[berekening];
			 	 	 printf("berekening is %d lengte is %d\r\n", berekening, lensignaal);
					 for(uint16_t i = 0; i < lengte_signaal[berekening]; i ++)
					 {
					 printf("ik werk %d  controle= %d\r\n ", tijd_geheugen[i], controle);
					 }
					 IR_sent(&tijd_geheugen, lensignaal);
					 *knop = 0;

		 }


	 }
}
void ColorToSelectedbutton(uint8_t *button, uint8_t *device, uint16_t color)
{
	uint8_t knop = *button;
	uint16_t bordercolor;
	uint8_t apparaat = *device;
	switch(apparaat)
			{
	case 1:
		bordercolor = ILI9488_BLUE;
		break;
	case 2:
		bordercolor = ILI9488_YELLOW;
		break;
	case 3:
		bordercolor = ILI9488_GREEN;
		break;
	case 4:
		bordercolor = ILI9488_WHITE;
		break;
	default:
		break;
	}

	switch(knop)
			{
			case 1:
				//Power
				DrawRectangle(23, 50, 75, 75, bordercolor, color, 5);
				break;
			case 2:
				//Src
				DrawRectangle(219, 50, 75, 75, bordercolor, color, 5);
				break;
			case 3:
				//^
				DrawRectangle(121, 80, 75, 75, bordercolor, color, 5);
				break;
			case 4:
				//<
				DrawRectangle(23, 165, 75, 75, bordercolor, color, 5);
				break;
			case 5:
				//OK
				DrawRectangle(121, 165, 75, 75, bordercolor, color, 5);
				break;
			case 6:
				//>
				DrawRectangle(219, 165, 75, 75, bordercolor, color, 5);
				break;
			case 7:
				//V
				DrawRectangle(121, 250, 75, 75, bordercolor, color, 5);
				break;
			case 8:
				//volume up
					DrawRectangle(23, 298, 75, 75, bordercolor, color, 5);
				break;
			case 9:
				//volume down
					DrawRectangle(23, 393, 75, 75, bordercolor, color, 5);
				break;
			case 10:
				//mute
					DrawRectangle(121, 393, 75, 75, bordercolor, color, 5);
				break;
			case 11:
				//channel up
					DrawRectangle(219, 298, 75, 75, bordercolor, color, 5);
				break;
			case 12:
				//channel down
					DrawRectangle(219, 393, 75, 75, bordercolor, color, 5);
				break;
			default:
				break;
			}
}
void mainscreen()
{
	if(UartScreen == 1)
		{
		printf("Even geduld...\r\n");
		}
	fillRect(0, 0, 320, 480, 0x0000);
	//DrawImage(0, 0, POWERBUTTON_DATA_X_PIXEL, POWERBUTTON_DATA_Y_PIXEL, POWERBUTTON_DATA);
	//DrawRectangle(x, y, h, w, color, contentcolor, border)
	DrawRectangle(0, 0, 40, 79, ILI9488_BLUE, ILI9488_BLUE, 0);
	DrawRectangle(80, 0, 40, 79, ILI9488_YELLOW, ILI9488_YELLOW, 0);
	DrawRectangle(160, 0, 40, 79, ILI9488_GREEN, ILI9488_GREEN, 0);
	DrawRectangle(240, 0, 40, 79, ILI9488_WHITE, ILI9488_WHITE, 0);

	if(UartScreen == 1)
	{
	printf("MainScreen ingeladen\r\n");
	}
}

