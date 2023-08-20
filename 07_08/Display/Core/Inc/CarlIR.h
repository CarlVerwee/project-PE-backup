/*
 * CarlIR.h
 *
 *  Created on: 1 aug. 2023
 *      Author: Carl
 */

#ifndef INC_CARLIR_H_
#define INC_CARLIR_H_

#include "main.h"

void Interrupt_flank();
void IR_Carrier_ON();
void IR_Carrier_OFF();
void IR_Read();
void IR_UNDERSTAND();
void IR_sent(uint16_t *tijd, uint16_t lengte_sig);
void IR_Debug();
#endif /* INC_CARLIR_H_ */
