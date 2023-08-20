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


/* Some Extern Private variables -------------------------------*/
extern SPI_HandleTypeDef hspi2;

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
/*void DisplayBegin(void)
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
*/

void DisplayBegin(void)		//DRFRobot
{
	RST_HIGH(); //init
	CS_HIGH();  //init
	RST_HIGH(); //init
	HAL_Delay(100);
	RST_LOW(); //initialize with a low input pg23
	HAL_Delay(500); //langere delay
	RST_HIGH();
	HAL_Delay(500);  //langere delay
	writecommand(0xF7);
		writedata(0xA9);
		writedata(0x51);
		writedata(0x2C);
		writedata(0x82);
	writecommand(0xC0);
		writedata(0x11);
		writedata(0x09);
	writecommand(0xC1);
		writedata(0x41);
	writecommand(0xC5);
		writedata(0x00);
		writedata(0x0A);
		writedata(0x80);
	writecommand(0xB1);
		writedata(0xB0);
		writedata(0x11);
	writecommand(0xB4);
		writedata(0x02);
	writecommand(0xB6);
		writedata(0x02);
		writedata(0x22);
	writecommand(0xB7);
		writedata(0xc6);
	writecommand(0xBE);
		writedata(0x00);
		writedata(0x04);
	writecommand(0xE9);
		writedata(0x00);
	writecommand(0x36);
		writedata(0x08);
	writecommand(0x3A);
		writedata(0x66);
	writecommand(0xE0);
		writedata(0x00);
		writedata(0x07);
		writedata(0x10);
		writedata(0x09);
		writedata(0x17);
		writedata(0x0B);
		writedata(0x41);
		writedata(0x89);
		writedata(0x4B);
		writedata(0x0A);
		writedata(0x0C);
		writedata(0x0E);
		writedata(0x18);
		writedata(0x1B);
		writedata(0x0F);

	writecommand(0XE1);
		writedata(0x00);
		writedata(0x17);
		writedata(0x1A);
		writedata(0x04);
		writedata(0x0E);
		writedata(0x06);
		writedata(0x2F);
		writedata(0x45);
		writedata(0x43);
		writedata(0x02);
		writedata(0x0A);
		writedata(0x09);
		writedata(0x32);
		writedata(0x36);
		writedata(0x0F);

	writecommand(Display_ExitSleep);    //Exit Sleep

	HAL_Delay(120);
	writecommand(0x20);
	writecommand(Display_On);    //Display on
		writedata(0xB0);


}

/*
  HAL_SPI_STATE_RESET      = 0x00U,    //!< Peripheral not Initialized
  HAL_SPI_STATE_READY      = 0x01U,    //!< Peripheral Initialized and ready for use
  HAL_SPI_STATE_BUSY       = 0x02U,    //!< an internal process is ongoing
  HAL_SPI_STATE_BUSY_TX    = 0x03U,    //!< Data Transmission process is ongoing
  HAL_SPI_STATE_BUSY_RX    = 0x04U,    //!< Data Reception process is ongoing
  HAL_SPI_STATE_BUSY_TX_RX = 0x05U,    //!< Data Transmission and Reception process is ongoing
  HAL_SPI_STATE_ERROR      = 0x06U,    //!< SPI error state
  HAL_SPI_STATE_ABORT      = 0x07U     //!< SPI abort is ongoing
  */
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
	  lcd_write_data(r);
	 lcd_write_data(g);
	 lcd_write_data(b);
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
/*void setScrollArea(uint16_t,uint16_t){}
void scroll(uint16_t){}
void pushColor(uint16_t){}
void pushColors(uint16_t*,uint8_t,uint8_t){}
void drawImage(const uint8_t*,uint16_t, uint16_t, uint16_t, uint16_t){}
void fillScreen(uint16_t){}
void drawPixel(uint16_t, uint16_t, uint16_t){}
void drawFastHLine(uint16_t, uint16_t, uint16_t, uint16_t){}
void drawFastVLine(uint16_t, uint16_t, uint16_t, uint16_t){}
void drawLine(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t){}
void writeLine(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t){}
void setRotation(uint8_t){}
void invertDisplay(uint8_t){}
void color565(uint8_t, uint8_t, uint8_t){}
void spiwrite(uint8_t){}
void write16BitColor(uint16_t){}
void testlines(uint8_t){}
void sendasIO(uint8_t){}*/
void lcd_init_command_list(void)
{

	//********Start Initial Sequence*******//
	lcd_write_command(0xE0); //P-Gamma
	lcd_write_data(0x00);
	lcd_write_data(0x13);
	lcd_write_data(0x18);
	lcd_write_data(0x04);
	lcd_write_data(0x0F);
	lcd_write_data(0x06);
	lcd_write_data(0x3A);
	lcd_write_data(0x56);
	lcd_write_data(0x4D);
	lcd_write_data(0x03);
	lcd_write_data(0x0A);
	lcd_write_data(0x06);
	lcd_write_data(0x30);
	lcd_write_data(0x3E);
	lcd_write_data(0x0F);
	lcd_write_command(0XE1); //N-Gamma
	lcd_write_data(0x00);
	lcd_write_data(0x13);
	lcd_write_data(0x18);
	lcd_write_data(0x01);
	lcd_write_data(0x11);
	lcd_write_data(0x06);
	lcd_write_data(0x38);
	lcd_write_data(0x34);
	lcd_write_data(0x4D);
	lcd_write_data(0x06);
	lcd_write_data(0x0D);
	lcd_write_data(0x0B);
	lcd_write_data(0x31);
	lcd_write_data(0x37);
	lcd_write_data(0x0F);
	lcd_write_command(0xC0);
	lcd_write_data(0x18);
	lcd_write_data(0x16);
	lcd_write_command(0xC1);
	lcd_write_data(0x45);
	lcd_write_command(0xC5); //VCOM
	lcd_write_data(0x00);
	lcd_write_data(0x63);
	lcd_write_data(0x01);

	lcd_write_command(0x36); //RAM address mode
	//0xF8 and 0x3C are landscape mode. 0x5C and 0x9C for portrait mode.
	//if(LANDSCAPE)
		lcd_write_data(0xF8);
	//else
	//	lcd_write_data(0x5C);

	lcd_write_command(0x3A); //Interface Mode Control
	lcd_write_data(0x66); //16-bit serial mode
	lcd_write_command(0xB0); //Interface Mode Control
	lcd_write_data(0x80); //SDO not in use
	lcd_write_command(0xB1); //Frame rate 70HZ
	lcd_write_data(0x00); //
	lcd_write_data(0x10);
	lcd_write_command(0xB4);
	lcd_write_data(0x02);

	lcd_write_command(0xB6); //RGB/MCU Interface Control
	lcd_write_data(0x02);
	//lcd_write_data(0x22);

	lcd_write_command(0xE9);
	lcd_write_data(0x00);
	lcd_write_command(0xF7);
	lcd_write_data(0xA9);
	lcd_write_data(0x51);
	lcd_write_data(0x2C);
	lcd_write_data(0x82);
	lcd_write_command(0x11);
	HAL_Delay(120);
	lcd_write_command(0x21);


	HAL_Delay(120);
	lcd_write_command(0x29);
}
void lcd_write_data(uint8_t dta )
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
void lcd_write_command(uint8_t cmdo)  //https://github.com/BasicCode/ILI9488-STM32/blob/main/ILI9488.c
{
	uint8_t commando = cmdo;
	DC_LOW(); //Low = command pg23
	CS_LOW(); //The chip is selected and accessible pg23
	HAL_SPI_Transmit(&mySPI, &commando, 1, HAL_MAX_DELAY); //Size = bytes
	CS_HIGH(); //The chip is not selected and not accessible pg23
}
