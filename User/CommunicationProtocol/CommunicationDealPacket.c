#include "CommunicationDealPacket.h"

void ControlSwitch(char* switchType, uint16_t switchIndex, uint16_t statusSet)
{
    static uint16_t count = 0;
    if(strcmp(switchType,"Light")==0)
    {
        if(statusSet)
        {
            lamps[switchIndex].LightOn();
        }else
        {
            lamps[switchIndex].LightOff();
        }
    }
    else if(strcmp(switchType,"Slider")==0)
    {
        printf("Slider %d\r\n", count++);
        Set_TIM1_CH1_Percentage(statusSet);
    }
}

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
            char* switchType;
            uint16_t address;
            uint16_t switchIndex;
            uint16_t statusSet;
            
//            printf("Receive set command.\r\n");
            root = cJSON_Parse((char*)(packetBlock->messageData));
            infoType = cJSON_GetObjectItem(root,"InfoType")->valuestring;
            onwer = cJSON_GetObjectItem(root,"Owner")->valuestring;
            address = cJSON_GetObjectItem(root,"Address")->valueint;
            SwitchSet = cJSON_GetObjectItem(root,"SwitchSet");
            switchType = cJSON_GetObjectItem(SwitchSet,"SwitchType")->valuestring;
            switchIndex = cJSON_GetObjectItem(SwitchSet,"SwitchIndex")->valueint;
            statusSet = cJSON_GetObjectItem(SwitchSet,"StatusSet")->valueint;
            if((strcmp(infoType,"Setting")==0) && (address == Protocol_LocalhostAddress))
            {
//                printf("%s - %s - %d - %s - %d - %d \r\n", infoType, onwer, address, switchType, switchIndex, statusSet);
                ControlSwitch(switchType, switchIndex, statusSet);
            }
            cJSON_Delete(root);        
        }
        break;
        
        default:
            free(packetBlock->messageData);
            packetBlock->messageData = NULL;
//            free(packetBlock);
        break;
    }
}












