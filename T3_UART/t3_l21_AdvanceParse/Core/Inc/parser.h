/*
 * parser.h
 *
 *  Created on: Aug 22, 2020
 *      Author: Mateusz Salamon
 */

#ifndef INC_PARSER_H_
#define INC_PARSER_H_
#include "uartdma.h"

void UART_ParseLine(UARTDMA_HandleTypeDef *huartdma);

#endif /* INC_PARSER_H_ */
