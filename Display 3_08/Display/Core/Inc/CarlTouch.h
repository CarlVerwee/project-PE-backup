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
void Touch();
void ReadButton(uint8_t *button);
void ButtonPressed(uint8_t *button);
void ReadDevice(uint8_t *device);
void DevicePressed(uint8_t *device);


#endif /* SRC_CARLTOUCH_H_ */
