/*
 * CarlDisplay.h
 *
 *  Created on: Jul 27, 2023
 *      Author: Carl
 */

#ifndef INC_CARLDISPLAY_H_
#define INC_CARLDISPLAY_H_

#include "main.h"

/* Some Easy Functions */
void CS_HIGH();
void CS_LOW();
void DC_HIGH();
void DC_LOW();
void RST_HIGH();
void RST_LOW();

void DrawImage(uint16_t x, uint16_t y , uint16_t w , uint16_t h, uint16_t *Image);
void Starting(); //Just a beautifel printf
void DisplayBegin(void);
void lcd_init_command_list(void);
void setAddrWindow(uint16_t,uint16_t,uint16_t,uint16_t);
void setScrollArea(uint16_t,uint16_t);
void scroll(uint16_t);
void pushColor(uint16_t);
void pushColors(uint16_t*,uint8_t,uint8_t);


void fillScreen(uint16_t);
void drawPixel(uint16_t, uint16_t, uint16_t);
void drawFastHLine(uint16_t, uint16_t, uint16_t, uint16_t);
void drawFastVLine(uint16_t, uint16_t, uint16_t, uint16_t);
void drawLine(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
void writeLine(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
void fillRect(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
void setRotation(uint8_t);
void invertDisplay(uint8_t);
void color565(uint8_t, uint8_t, uint8_t);
void spiwrite(uint8_t);
void writecommand(uint8_t);
void lcd_write_data(uint8_t);
void lcd_write_command(uint8_t);
void write16BitColor(uint16_t);
void writedata(uint8_t);
void testlines(uint8_t);
void sendasIO(uint8_t);

#endif /* INC_CARLDISPLAY_H_ */
