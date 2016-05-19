#ifndef __UART5_H
#define	__UART5_H

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include "NVIC_CONFIG.H"
#include "CommunicationProtocol.h"


void UART5_Config(uint32_t BaudRate);
void SendUART5OneByte(uint8_t byteData);
void SendUART5BytesBuf(uint8_t* bytesBuf, uint16_t bytesBufLength);
void ChangeUART5BaudRate(uint32_t BaudRate, FunctionalState ReceiveITState);

#endif /* __UART5_H */
