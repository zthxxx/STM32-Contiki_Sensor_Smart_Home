#include "CommunicationProtocol.h"




const uint8_t Protocol_PacketStratData[2] = {0xAA,0xAA};
uint16_t Protocol_PacketSendIndex = 0;
uint16_t Protocol_PacketAckedIndex = 0;
Uint8PacketQueue UnsentPacketQueue;
Uint8PacketQueue UnackedPacketQueue;
Uint8PacketQueue* UnsentPacketQueueHandle = &UnsentPacketQueue;
Uint8PacketQueue* UnackedPacketQueueHandle = &UnackedPacketQueue;


//uint8_t Protocol_DeviceAddressData[2] = {0x00,0x00};
//FunctionWord_TypeDef Protocol_FunctionWord;
//uint8_t Protocol_DataLengthBytes[2];
//uint8_t* Protocol_JSONMessageData;//此处uint8_t 是因为在MDK中，默认char 是unsigned char
//uint8_t Protocol_PacketCheckSum;




Uint8PacketQueue* CreatUint8PacketQueue(void)
{
    Uint8PacketQueue*  Uint8PacketQueueHandle = NULL;
    Uint8PacketQueueHandle = (Uint8PacketQueue*)malloc(sizeof(Uint8PacketQueue));
    if(Uint8PacketQueueHandle == NULL)
    {
        return NULL;
    }
    Uint8PacketQueueHandle->head = NULL;
    Uint8PacketQueueHandle->last = NULL;
    return Uint8PacketQueueHandle;
}

void Uint8PacketQueuePush(Uint8PacketQueue* Uint8PacketQueueHandle,Uint8PacketNode* uint8PacketNodePointer)
{
    if(Uint8PacketQueueHandle->head == NULL)
    {
        Uint8PacketQueueHandle->head = uint8PacketNodePointer;
    }
    else
    {
        Uint8PacketQueueHandle->last->next = uint8PacketNodePointer;
    }
    uint8PacketNodePointer->next = NULL;
    Uint8PacketQueueHandle->last = uint8PacketNodePointer;
}

void Uint8PacketQueuePushData(Uint8PacketQueue* Uint8PacketQueueHandle,uint8_t* packet)
{
    Uint8PacketNode* uint8PacketNodePointer;
    uint8PacketNodePointer = (Uint8PacketNode*)malloc(sizeof(Uint8PacketNode));
    if(!uint8PacketNodePointer)return;
    
    uint8PacketNodePointer->packet = packet;
    uint8PacketNodePointer->next = NULL;
    uint8PacketNodePointer->index = Protocol_PacketSendIndex++;
    uint8PacketNodePointer->resendCount = 0;
    uint8PacketNodePointer->resendTime = 0;
    Uint8PacketQueuePush(Uint8PacketQueueHandle, uint8PacketNodePointer);
}

Uint8PacketNode* Uint8PacketQueuePop(Uint8PacketQueue* Uint8PacketQueueHandle)
{
    Uint8PacketNode* uint8PacketNodePointer;
    if(Uint8PacketQueueHandle->head == NULL)
    {
        return NULL;
    }
    uint8PacketNodePointer = Uint8PacketQueueHandle->head;
    Uint8PacketQueueHandle->head = Uint8PacketQueueHandle->head->next;
    uint8PacketNodePointer->next = NULL;
    return uint8PacketNodePointer;  
}

void SendOneCommunicationPacket(Uint8PacketNode* uint8PacketNodePointer)
{
    uint16_t protocol_PacketLength;
    uint8_t* packet;
    if(!uint8PacketNodePointer)return;
    packet = uint8PacketNodePointer->packet;
    protocol_PacketLength = packet[5];      //长度所在位置
    protocol_PacketLength += packet[6]<<8;
    
    sendUartByteBuf(packet, protocol_PacketLength + PROTOCOL_PACKET_CONSISTENT_LENGTH);
}


void SendUnsentPacketQueue()
{
    Uint8PacketNode* uint8PacketNodePointer;
    while(UnsentPacketQueueHandle->head)
    {
        uint8PacketNodePointer = Uint8PacketQueuePop(UnsentPacketQueueHandle);
        SendOneCommunicationPacket(uint8PacketNodePointer);
        Uint8PacketQueuePush(UnackedPacketQueueHandle, uint8PacketNodePointer);
    }
}

void DeletPacketQueueCuurentItem(Uint8PacketQueue* PacketQueueHandle, Uint8PacketNode** uint8PacketNodePointer, Uint8PacketNode** uint8PacketNodePreviousPointer)
{
    if(!*uint8PacketNodePointer)return;
    if(!*uint8PacketNodePreviousPointer)         //上一项不存在  相当于为头节点
    {
        *uint8PacketNodePreviousPointer = *uint8PacketNodePointer;
        *uint8PacketNodePointer = (*uint8PacketNodePointer)->next;
        free((*uint8PacketNodePreviousPointer)->packet);
        free(*uint8PacketNodePreviousPointer);
        *uint8PacketNodePreviousPointer = NULL;
        PacketQueueHandle->head = *uint8PacketNodePointer;
    }
    else
    {
        (*uint8PacketNodePreviousPointer)->next = (*uint8PacketNodePointer)->next;
        free((*uint8PacketNodePointer)->packet);
        free(*uint8PacketNodePointer);
        *uint8PacketNodePointer = (*uint8PacketNodePreviousPointer)->next;
        if(!*uint8PacketNodePointer)         //下一项不存在  相当于为尾节点
        {
            PacketQueueHandle->last = *uint8PacketNodePreviousPointer;
        }
    }
}


void DeletPacketQueueConditionalItem(Uint8PacketQueue* PacketQueueHandle, bool (*PacketQueueItemCondition)(Uint8PacketNode* uint8PacketNodePointer))
{
    Uint8PacketNode* uint8PacketNodePointer = NULL;
    Uint8PacketNode* uint8PacketNodePreviousPointer = NULL;
    uint8PacketNodePointer = PacketQueueHandle->head;
    
    while(uint8PacketNodePointer)
    {
        if(PacketQueueItemCondition(uint8PacketNodePointer))
        {
            DeletPacketQueueCuurentItem(PacketQueueHandle, &uint8PacketNodePointer, &uint8PacketNodePreviousPointer);
            continue;
        }
        uint8PacketNodePreviousPointer = uint8PacketNodePointer;
        uint8PacketNodePointer = uint8PacketNodePointer->next;
    }
}

bool UnackedPacketRetimeAndRecountCondition(Uint8PacketNode* uint8PacketNodePointer)
{
    if(!uint8PacketNodePointer)return false;
    if(uint8PacketNodePointer->resendTime >= PROTOCOL_PACKET_RESENT_TIME_MAX)
    {
        uint8PacketNodePointer->resendTime = 0;
        SendOneCommunicationPacket(uint8PacketNodePointer);
        uint8PacketNodePointer->resendCount++;
        if(uint8PacketNodePointer->resendCount >= PROTOCOL_PACKET_RESENT_COUNT_MAX)
        {
            return true;
        }
    }
    return false;
}

bool UnackedPacketAckIndexCondition(Uint8PacketNode* uint8PacketNodePointer)
{
    if(!uint8PacketNodePointer)return false;
    if(uint8PacketNodePointer->index == Protocol_PacketAckedIndex)
    {
        return true;
    }
    return false;
}

void SendUnackedPacketQueue()
{    
    DeletPacketQueueConditionalItem(UnackedPacketQueueHandle, UnackedPacketRetimeAndRecountCondition);
}

void IncreaseUnackedPacketQueueResendTime()
{
    Uint8PacketNode* uint8PacketNodePointer = NULL;
    uint8PacketNodePointer = UnackedPacketQueueHandle->head;
    
    while(uint8PacketNodePointer)
    {
        if(uint8PacketNodePointer->resendTime < PROTOCOL_PACKET_RESENT_TIME_MAX)
        {
            uint8PacketNodePointer->resendTime++;
        }
        uint8PacketNodePointer = uint8PacketNodePointer->next;
    }
}

void DeleteAckedIndexPacket(uint16_t packetAckedIndex)
{
    Protocol_PacketAckedIndex = packetAckedIndex;
    DeletPacketQueueConditionalItem(UnackedPacketQueueHandle, UnackedPacketAckIndexCondition);
}

void AssembleCommunicationPacket(FunctionWord_TypeDef FunctionWord, uint16_t JSONMessageDataLength,uint8_t* JSONMessageData)
{
    uint16_t protocol_PacketLength;
    uint8_t protocol_DataLengthBytes[2];
    uint8_t protocol_PacketSendIndexBytes[2];
    uint32_t packetCheckSum;
    uint16_t bytesCount;
    uint8_t* assembledPacketBuf;
    
    
    protocol_PacketSendIndexBytes[0] = (uint8_t)(Protocol_PacketSendIndex & 0x00FF);
    protocol_PacketSendIndexBytes[1] = (uint8_t)(Protocol_PacketSendIndex >> 8);
    
    
    
    protocol_DataLengthBytes[0] = (uint8_t)(JSONMessageDataLength & 0x00FF);
    protocol_DataLengthBytes[1] = (uint8_t)(JSONMessageDataLength >> 8);  
    protocol_PacketLength = JSONMessageDataLength + PROTOCOL_PACKET_CONSISTENT_LENGTH;
    
    assembledPacketBuf = (uint8_t *)malloc(protocol_PacketLength * sizeof(uint8_t));
    
    memcpy(assembledPacketBuf,Protocol_PacketStratData,2);
    memcpy(assembledPacketBuf + 2,protocol_PacketSendIndexBytes,2);
    memcpy(assembledPacketBuf + 4,&FunctionWord,1);
    memcpy(assembledPacketBuf + 5,protocol_DataLengthBytes,2);
    memcpy(assembledPacketBuf + 7,JSONMessageData,JSONMessageDataLength);
    
    for(bytesCount=0;bytesCount<protocol_PacketLength-1;bytesCount++)
    {
        packetCheckSum += assembledPacketBuf[bytesCount];
    }
    assembledPacketBuf[protocol_PacketLength-1] = (uint8_t)(packetCheckSum & 0x000000FF);
    
    Uint8PacketQueuePushData(UnsentPacketQueueHandle,assembledPacketBuf);
}




