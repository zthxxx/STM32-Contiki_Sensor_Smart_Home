#ifndef __COMMUNICATION_DEAL_PACKET_H__
#define __COMMUNICATION_DEAL_PACKET_H__


#include "CommunicationProtocol.h"
#include "Uint8PacketQueueManger.h"
#include "CommunicationConfig.h"

typedef struct PacketBlock PacketBlock;
void DealWithReceivePacketBlock(PacketBlock* packetBlock);



#endif






