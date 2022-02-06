/*
 * uartdma.c
 *
 *  Created on: Feb 17, 2021
 *      Author: Dominik
 */

#include "main.h"
#include "uartdma.h"

typedef struct{
	__IO uint32_t ISR;
	__IO uint32_t Reserved0;
	__IO uint32_t IFCR;
}DMA_Base_Registers;

void UARTDMA_UartIrqHandler(UARTDMA_HandleTypeDef *huartdma)
{
	if(huartdma -> huart -> Instance -> SR & UART_FLAG_IDLE)
	{
		volatile uint32_t tmp;
		tmp = huartdma -> huart -> Instance -> SR;
		tmp = huartdma -> huart -> Instance -> DR;
		huartdma -> huart -> hdmarx -> Instance -> CR &= ~DMA_SxCR_EN;
		tmp=tmp;
	}
}

void UARTDMA_DmaReceiveIrqHandler(UARTDMA_HandleTypeDef *huartdma)
{
	uint8_t *DmaBufferPointer;
	uint16_t Length,i;

	DMA_Base_Registers *DmaRegisters = (DMA_Base_Registers *) huartdma -> huart -> hdmarx -> StreamBaseAddress;

	if(__HAL_DMA_GET_IT_SOURCE(huartdma -> huart -> hdmarx, DMA_IT_TC) != RESET)
	{
		DmaRegisters -> IFCR = DMA_FLAG_TCIF0_4 << huartdma -> huart -> hdmarx -> StreamIndex;

		Length = DMA_RX_BUFFER_SIZE - huartdma -> huart -> hdmarx -> Instance -> NDTR;

		DmaBufferPointer = huartdma -> DMA_RX_Buffer;

		for(i=0; i < Length; i++)
		{
			RB_Write(&huartdma -> UART_RX_Buffer, DmaBufferPointer[i]);
		}

		DmaRegisters -> IFCR = 0x3FU << huartdma -> huart -> hdmarx -> StreamIndex;
		huartdma -> huart -> hdmarx -> Instance -> M0AR = (uint32_t)huartdma -> DMA_RX_Buffer;
		huartdma -> huart -> hdmarx -> Instance -> NDTR = DMA_RX_BUFFER_SIZE;
		huartdma -> huart -> hdmarx -> Instance -> CR |= DMA_SxCR_EN;
	}
}

void UARTDMA_PutCharToTxBuffer(UARTDMA_HandleTypeDef *huartdma, char c)
{
	RB_Write(&huartdma->UART_TX_Buffer, c);
}

void UARTDMA_Print(UARTDMA_HandleTypeDef *huartdma, const char *Message)
{
	char *MsgPointer;
	char CharToPut;
	MsgPointer = (char*)Message;

	while((CharToPut = *MsgPointer))
	{
	UARTDMA_PutCharToTxBuffer( huartdma, CharToPut);
	MsgPointer++;

		if(CharToPut=='\n')
		{
			huartdma->UartTxBufferLines++;
		}
	}
}

uint8_t UARTDMA_IsDataTransferReady(UARTDMA_HandleTypeDef *huartdma)
{
	if(huartdma->UartTxBufferLines)
		return 1;
	else
		return 0;
}

void UARTDMA_TransmitEvent(UARTDMA_HandleTypeDef *huartdma)
{
	char CharToSend;
	RB_Status Status;
	uint16_t i=0;

	if(UARTDMA_IsDataTransferReady(huartdma))
	{
		if(huartdma->huart->hdmatx->State != HAL_DMA_STATE_BUSY)
		{
			do{
				Status = RB_Read(&huartdma->UART_TX_Buffer, (uint8_t*)&CharToSend);

				if(Status == RB_OK)
				{
					huartdma->DMA_TX_Buffer[i++] = CharToSend;
				}

			}while((Status == RB_OK && CharToSend != '\n'));

			if(CharToSend=='\n')
			{
				huartdma->UartTxBufferLines--;
			}

			HAL_UART_Transmit_DMA(huartdma->huart, huartdma->DMA_TX_Buffer, i);
		}
	}
}

void UARTDMA_Init(UARTDMA_HandleTypeDef *huartdma, UART_HandleTypeDef *huart)
{
	huartdma->huart = huart;

	//IDLE Enable
	__HAL_UART_ENABLE_IT( huartdma->huart, UART_IT_IDLE);
	//DMA TC Enable
	__HAL_DMA_ENABLE_IT( huartdma->huart->hdmarx, DMA_IT_TC);
	__HAL_DMA_ENABLE_IT( huartdma->huart->hdmatx, DMA_IT_TC);
	//Run DMA UART on Buffer RX
	HAL_UART_Receive_DMA(huart, huartdma->DMA_RX_Buffer, DMA_RX_BUFFER_SIZE);
	//DMA HT Disable
	__HAL_DMA_DISABLE_IT( huartdma->huart->hdmarx, DMA_IT_HT);
	__HAL_DMA_DISABLE_IT( huartdma->huart->hdmatx, DMA_IT_HT);

}
