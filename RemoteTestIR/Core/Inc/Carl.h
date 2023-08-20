/*
 * Carl.h
 *
 *  Created on: May 11, 2023
 *      Author: carl_
 */

#ifndef INC_CARL_H_
#define INC_CARL_H_

#include "main.h"

void IR_Read();
void IR_sent();
void InitTouch();
void IR_UNDERSTAND();
void IR_Debug();
void All_Carl_Inits();
void Touch();
void IR_Carrier_ON();
void IR_Carrier_OFF();
void Interrupt_flank();
void whilelus(); //de whilelus

// Old Nota
//
///* DFR0669 */
//
////Turns Backlight off
//void CARL_Backlight_OFF();
////Turns Backlight on
//void CARL_Backlight_ON();

#endif /* INC_CARL_H_ */
