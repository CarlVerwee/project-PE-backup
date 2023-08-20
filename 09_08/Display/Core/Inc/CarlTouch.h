/*
 * CarlTouch.h
 *
 *  Created on: Jul 30, 2023
 *      Author: Carl
 */

#ifndef SRC_CARLTOUCH_H_
#define SRC_CARLTOUCH_H_

#include "main.h"


void InitTouch();
void Touch(uint8_t *, uint16_t *X_waarde, uint16_t *Y_waarde);
void ReadButton(uint8_t *button, uint16_t *X_waarde, uint16_t *Y_waarde);
void buttonPressed(uint8_t *dev, uint8_t *knop, uint8_t *Sensingtouch, uint16_t *X_waarde, uint16_t *Y_waarde);
void ReadDevice(uint8_t *device, uint16_t *X_waarde, uint16_t *Y_waarde);
void DevicePressed(uint8_t *device);


#endif /* SRC_CARLTOUCH_H_ */
