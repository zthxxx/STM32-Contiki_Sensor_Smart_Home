#ifndef __COMMUNICATION_DEAL_PACKET_H__
#define __COMMUNICATION_DEAL_PACKET_H__
#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "CommunicationProtocol.h"
#include "Uint8PacketQueueManger.h"
#include "CommunicationConfig.h"
#include "bsp_gpio.h"
#include "PWM.h"

typedef struct PacketBlock PacketBlock;
void DealWithReceivePacketBlock(PacketBlock* packetBlock);



#endif






