/*
 * Carl.h
 *
 *  Created on: Jul 30, 2023
 *      Author: Carl
 */

#ifndef SRC_CARL_H_
#define SRC_CARL_H_

#include "main.h"
#include "CarlDisplay.h"
#include "CarlTouch.h"
#include "CarlIR.h"


void Starting(); //Just a beautifel printf
void All_Carl_Inits();
void WriteToGeheugen(uint8_t device, uint8_t knop);
void DeleteFromGeheugen(uint8_t device, uint8_t knop);
void TakefromGeheugen(uint8_t device, uint8_t knop);
void Starting();
void whilelus();

#endif /* SRC_CARL_H_ */
