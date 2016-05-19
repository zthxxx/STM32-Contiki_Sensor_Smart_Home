#include "CommunicationDealPacket.h"



/*对数据块进行处理
*
*
*/
void DealWithReceivePacketBlock(PacketBlock* packetBlock)
{
    if(!packetBlock)return;
    switch(packetBlock->functionWord)
    {
        case FunctionWord_Data:
        {
            uint8_t* assembledPacketBuf;
            printf("%d\t%d\r\n",Protocol_PacketSendIndex++,packetBlock->messageDataLength);
            assembledPacketBuf = ResolvePacketStructIntoBytes(packetBlock);
            Uint8PacketQueuePushStreamData(UnsentPacketQueueHandle,assembledPacketBuf,packetBlock->messageDataLength + PROTOCOL_PACKET_CONSISTENT_LENGTH);
        }
        break;
        
        default:
            free(packetBlock->messageData);
            packetBlock->messageData = NULL;
            free(packetBlock);
        break;
    }
}












