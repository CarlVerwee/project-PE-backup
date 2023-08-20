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

void DisplayBegin(void);
void setAddrWindow(uint16_t,uint16_t,uint16_t,uint16_t);
void drawPixel(uint16_t, uint16_t, uint16_t);
void DrawImage(uint16_t x, uint16_t y , uint16_t w , uint16_t h, uint16_t *Image);
void DrawRectangle(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint8_t);
void DrawCircle(uint16_t x, uint16_t y, uint16_t diameter, uint8_t border, uint16_t bordercolor, uint16_t contentcolor);
void DrawVolumebuttonUp();
void DrawVolumebuttonDown();
void fillRect(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
void DevicePressed(uint8_t *apparaat);
void ColorToSelectedbutton(uint8_t *button, uint8_t *device, uint16_t color);
void mainscreen();
void writecommand(uint8_t);
void write16BitColor(uint16_t);
void writedata(uint8_t);
void DevicePressed(uint8_t *device);
void buttonPressed(uint8_t *dev, uint8_t *knop, uint8_t *Sensingtouch, uint16_t *X_w, uint16_t *Y_w);

#endif /* INC_CARLDISPLAY_H_ */
