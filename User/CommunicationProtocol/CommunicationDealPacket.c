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
        
        case FunctionWord_SetProperty:
        {
            cJSON * root;
            cJSON * SwitchSet; 
            char* infoType;
            char* onwer;
            uint16_t address;
            uint16_t switchIndex;
            uint16_t statusSet;
            
//            printf("Receive set command.\r\n");
            root = cJSON_Parse((char*)(packetBlock->messageData));
            infoType = cJSON_GetObjectItem(root,"InfoType")->valuestring;
            onwer = cJSON_GetObjectItem(root,"Owner")->valuestring;
            address = cJSON_GetObjectItem(root,"Address")->valueint;
            SwitchSet = cJSON_GetObjectItem(root,"SwitchSet");
            switchIndex = cJSON_GetObjectItem(SwitchSet,"SwitchIndex")->valueint;
            statusSet = cJSON_GetObjectItem(SwitchSet,"StatusSet")->valueint;
//            printf("%s - %s - %d - %d - %d \r\n", infoType, onwer, address, switchIndex, statusSet);
            if(address == Protocol_LocalhostAddress)
            {
                if(statusSet)
                {
                    lamps[switchIndex].LightOn();
                }else
                {
                    lamps[switchIndex].LightOff();
                }
            }
            cJSON_Delete(root);        
        }
        break;
        
        default:
            free(packetBlock->messageData);
            packetBlock->messageData = NULL;
            free(packetBlock);
        break;
    }
}












