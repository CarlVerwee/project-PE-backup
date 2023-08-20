/*
 * CarlIR.h
 *
 *  Created on: 1 aug. 2023
 *      Author: Carl
 */

#ifndef INC_CARLIR_H_
#define INC_CARLIR_H_

#include "main.h"

void cleartijd();
void Interrupt_flank();
void IR_Carrier_ON();
void IR_Carrier_OFF();
void IR_Read();
void IR_UNDERSTAND(uint8_t *berekening, uint8_t *geen_signaal, uint8_t *lengte_signaal_gevonden);
void IR_sent(uint16_t *tijd, uint16_t lengte_sig);
#endif /* INC_CARLIR_H_ */
