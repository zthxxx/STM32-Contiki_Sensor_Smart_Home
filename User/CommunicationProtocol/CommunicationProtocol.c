#include "CommunicationProtocol.h"


#define AT_LEAST_PACKET_BYTES_LENGTH 8

const uint8_t Protocol_PacketHeadData[2] = {0xAA,0xAA};
uint16_t Protocol_PacketSendIndex = 0;//连续增加的发送序号
uint16_t Protocol_PacketAckedIndex = 0;//作为缓存中转的接收序号
Uint8PacketQueue UnsentPacketQueue;
Uint8PacketQueue UnackedPacketQueue;
Uint8PacketQueue ReceivedPacketQueue;
Uint8FIFOQueue ReceiveBytesFIFOQueue;

Uint8PacketQueue* UnsentPacketQueueHandle = &UnsentPacketQueue;
Uint8PacketQueue* UnackedPacketQueueHandle = &UnackedPacketQueue;
Uint8PacketQueue* ReceivedPacketBlockQueueHandle = &ReceivedPacketQueue;
Uint8FIFOQueue* ReceiveBytesFIFOQueueHandle = &ReceiveBytesFIFOQueue;

//uint8_t Protocol_DeviceAddressData[2] = {0x00,0x00};
//FunctionWord_TypeDef Protocol_FunctionWord;
//uint8_t Protocol_DataLengthBytes[2];
//uint8_t* Protocol_JSONMessageData;//此处uint8_t 是因为在MDK中，默认char 是unsigned char
//uint8_t Protocol_PacketCheckSum;



/*创建一个发送包的队列
*返回一个管理包的指针
*
*
*
*/
Uint8PacketQueue* CreatUint8PacketQueue(void)
{
    Uint8PacketQueue*  Uint8PacketQueueHandle = NULL;
    Uint8PacketQueueHandle = (Uint8PacketQueue*)malloc(sizeof(Uint8PacketQueue));//作为全局的管理队列对象，只生成一次，不释放
    if(Uint8PacketQueueHandle == NULL)
    {
        return NULL;
    }
    Uint8PacketQueueHandle->head = NULL;
    Uint8PacketQueueHandle->last = NULL;
    return Uint8PacketQueueHandle;
}

/*将一个发送包节点压入包队列中
*Uint8PacketQueueHandle:要压入的管理包指针
*uint8PacketNodePointer:发送包节点
*
*
*/
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

Uint8PacketNode* CreatUint8PacketNode(uint8_t* packet, PacketBlock* packetBlock)
{
    Uint8PacketNode* uint8PacketNodePointer;
    uint8_t packetIndexPosition;
    packetIndexPosition = sizeof(((PacketBlock*)0)->PacketHead);
    
    uint8PacketNodePointer = (Uint8PacketNode*)malloc(sizeof(Uint8PacketNode));
    if(!uint8PacketNodePointer)return NULL;
    
    uint8PacketNodePointer->packet = packet;
    uint8PacketNodePointer->packetBlock = packetBlock;
    uint8PacketNodePointer->next = NULL;
    if(packet)
    {
        uint8PacketNodePointer->index = packet[packetIndexPosition];
        uint8PacketNodePointer->index = packet[packetIndexPosition+1]<<8; 
    }
    else if(packetBlock)
    {
        uint8PacketNodePointer->index = packetBlock->PacketIndex;
    }
    else
    {
        uint8PacketNodePointer->index = 0;
    }
    
    uint8PacketNodePointer->resendCount = 0;
    uint8PacketNodePointer->resendTime = 0;
    return uint8PacketNodePointer;
}


/*将一个数据包结构体节点压入包队列中
*Uint8PacketQueueHandle:要压入的管理包指针
*packetBlock:数据包指针
*
*
*/
void Uint8PacketQueuePushStruct(Uint8PacketQueue* Uint8PacketQueueHandle,PacketBlock* packetBlock)
{
    Uint8PacketNode* uint8PacketNodePointer;
    if(!Uint8PacketQueueHandle)return;
    uint8PacketNodePointer = CreatUint8PacketNode(NULL, packetBlock);
    Uint8PacketQueuePush(Uint8PacketQueueHandle, uint8PacketNodePointer);
}

/*将一个数据包节点压入包队列中
*Uint8PacketQueueHandle:要压入的管理包指针
*packet:数据包指针
*
*
*/
void Uint8PacketQueuePushData(Uint8PacketQueue* Uint8PacketQueueHandle,uint8_t* packet)
{
    Uint8PacketNode* uint8PacketNodePointer;
    if(!Uint8PacketQueueHandle)return;
    uint8PacketNodePointer = CreatUint8PacketNode(packet, NULL);
    Uint8PacketQueuePush(Uint8PacketQueueHandle, uint8PacketNodePointer);
}

/*从发送包队列中弹出一个节点
*Uint8PacketQueueHandle:要弹出的管理包指针。
*返回值为节点指针。
*
*
*/
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


/*发送一个节点数据
*uint8PacketNodePointer:要发送的节点指针。
*
*
*/
void SendOneCommunicationPacket(Uint8PacketNode* uint8PacketNodePointer)
{
    uint16_t protocol_PacketLength;
    uint8_t* packet;
    uint8_t packetLengthPosition;
    packetLengthPosition = sizeof(((PacketBlock*)0)->PacketHead)+sizeof(((PacketBlock*)0)->PacketIndex)+sizeof((uint8_t)(((PacketBlock*)0)->PacketFunctionWord));
    if(!uint8PacketNodePointer)return;
    packet = uint8PacketNodePointer->packet;
    protocol_PacketLength = packet[packetLengthPosition];      //长度所在位置
    protocol_PacketLength += packet[packetLengthPosition+1]<<8;
    
    sendUartByteBuf(packet, protocol_PacketLength + PROTOCOL_PACKET_CONSISTENT_LENGTH);
}

/*发送一整个未发送队列数据
*
*
*
*/
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


/*释放一个包节点的空间
*uint8PacketNodePointer：要释放的项节点指针
*
*/
void FreePacketNoedItem(Uint8PacketNode* uint8PacketNodePointer)
{
    if(!uint8PacketNodePointer)return;
    free(uint8PacketNodePointer->packet);
    if(uint8PacketNodePointer->packetBlock)
    {
        free(uint8PacketNodePointer->packetBlock->PacketJSONData);
        free(uint8PacketNodePointer->packetBlock);
    }
    free(uint8PacketNodePointer);
}

/*删除队列中的一项
*PacketQueueHandle：包队列指针
*uint8PacketNodePointer：要删除的项节点指针
*uint8PacketNodePreviousPointer：要删除项的前一向的节点指针
*/
void DeletPacketQueueCurrentItem(Uint8PacketQueue* PacketQueueHandle, Uint8PacketNode** uint8PacketNodePointer, Uint8PacketNode** uint8PacketNodePreviousPointer)
{
    if(!*uint8PacketNodePointer)return;
    if(!*uint8PacketNodePreviousPointer)         //上一项不存在  相当于为头节点
    {
        *uint8PacketNodePreviousPointer = *uint8PacketNodePointer;
        *uint8PacketNodePointer = (*uint8PacketNodePointer)->next;
        FreePacketNoedItem(*uint8PacketNodePreviousPointer);      
        *uint8PacketNodePreviousPointer = NULL;
        PacketQueueHandle->head = *uint8PacketNodePointer;
    }
    else
    {
        (*uint8PacketNodePreviousPointer)->next = (*uint8PacketNodePointer)->next;
        FreePacketNoedItem(*uint8PacketNodePreviousPointer);        
        *uint8PacketNodePointer = (*uint8PacketNodePreviousPointer)->next;
        if(!*uint8PacketNodePointer)         //下一项不存在  相当于为尾节点
        {
            PacketQueueHandle->last = *uint8PacketNodePreviousPointer;
        }
    }
}

/*通过条件判断队列中每一项是否要删除
*PacketQueueHandle：包队列指针
*PacketQueueItemCondition：一个判断条件函数指针，接收一个节点，并判断是否需要删除它，如果是则返回True。
*
*/
void DeletPacketQueueConditionalItem(Uint8PacketQueue* PacketQueueHandle, bool (*PacketQueueItemCondition)(Uint8PacketNode* uint8PacketNodePointer))
{
    Uint8PacketNode* uint8PacketNodePointer = NULL;
    Uint8PacketNode* uint8PacketNodePreviousPointer = NULL;
    uint8PacketNodePointer = PacketQueueHandle->head;
    
    while(uint8PacketNodePointer)
    {
        if(PacketQueueItemCondition(uint8PacketNodePointer))
        {
            DeletPacketQueueCurrentItem(PacketQueueHandle, &uint8PacketNodePointer, &uint8PacketNodePreviousPointer);//因为要对外部的指针变量也产生影响，因此此处参数为指向指针的指针。
            continue;
        }
        uint8PacketNodePreviousPointer = uint8PacketNodePointer;
        uint8PacketNodePointer = uint8PacketNodePointer->next;
    }
}

/*释放所有未应答包的条件函数
*
*
*
*/
bool UnackedPacketAllDeletCondition(Uint8PacketNode* uint8PacketNodePointer)
{
    if(uint8PacketNodePointer)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*如果大于时间则重发，超过最大次数则删除的条件函数
*
*
*
*/
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

/*接收到应答包则删除指定序号节点的条件函数
*
*
*
*/
bool UnackedPacketAckIndexCondition(Uint8PacketNode* uint8PacketNodePointer)
{
    if(!uint8PacketNodePointer)return false;
    if(uint8PacketNodePointer->index == Protocol_PacketAckedIndex)
    {
        return true;
    }
    return false;
}

/*发送一整个未回应队列数据
*
*
*
*/
void SendUnackedPacketQueue()
{    
    DeletPacketQueueConditionalItem(UnackedPacketQueueHandle, UnackedPacketRetimeAndRecountCondition);
    DeletPacketQueueConditionalItem(UnackedPacketQueueHandle, UnackedPacketAllDeletCondition);
}

/*增加未回应队列的发送计数时间
*
*
*
*/
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

/*接收到应答包则删除指定序号节点
*
*
*
*/
void DeleteAckedIndexPacket(uint16_t packetAckedIndex)
{
    Protocol_PacketAckedIndex = packetAckedIndex;
    DeletPacketQueueConditionalItem(UnackedPacketQueueHandle, UnackedPacketAckIndexCondition);
}

/*组装一个结构体表示包的信息
*
*
*
*/
PacketBlock* AssembleCommunicationStruct(FunctionWord_TypeDef FunctionWord, uint16_t JSONMessageDataLength,uint8_t* JSONMessageData)
{
    PacketBlock* packetBlock;
    uint8_t packetHeadLength;
    packetBlock = (PacketBlock*)malloc(sizeof(PacketBlock));//生成一个数据包结构体，作为发送时在生成字节流时释放;作为接收时，在处理接收数据时被释放
    packetHeadLength = sizeof(Protocol_PacketHeadData);
    memcpy(packetBlock->PacketHead,Protocol_PacketHeadData,packetHeadLength);
    packetBlock->PacketIndex = Protocol_PacketSendIndex;
    packetBlock->PacketFunctionWord = FunctionWord;
    packetBlock->PacketJSONDataLength = JSONMessageDataLength;
    packetBlock->PacketJSONData = JSONMessageData;
    
    packetBlock->PacketCheckSum = 0;
    while(packetHeadLength-- > 0)
    {
        packetBlock->PacketCheckSum += packetBlock->PacketHead[packetHeadLength];
    }
    packetBlock->PacketCheckSum += (uint8_t)(packetBlock->PacketIndex & 0x00FF);
    packetBlock->PacketCheckSum += (uint8_t)((packetBlock->PacketIndex >> 8) & 0x00FF);
    packetBlock->PacketCheckSum += (uint8_t)packetBlock->PacketFunctionWord;
    packetBlock->PacketCheckSum += (uint8_t)(packetBlock->PacketJSONDataLength & 0x00FF);
    packetBlock->PacketCheckSum += (uint8_t)((packetBlock->PacketJSONDataLength >> 8) & 0x00FF);
    while(JSONMessageDataLength-- > 0)
    {
        packetBlock->PacketCheckSum += packetBlock->PacketJSONData[JSONMessageDataLength];
    }
    
    return packetBlock;
}

/*将一个包结构体解析为字符流
*packetBlock：包结构体
*返回值：字符流
*
*/
uint8_t* ResolvePacketStructIntoBytes(PacketBlock* packetBlock)
{
    uint8_t uint8FunctionWord;
    uint16_t protocol_PacketLength = 0;
    uint8_t* assembledPacketBuf;
    uint16_t packetBufOffset = 0;
    
    if(!packetBlock)return NULL;
    protocol_PacketLength = packetBlock->PacketJSONDataLength + PROTOCOL_PACKET_CONSISTENT_LENGTH;
    uint8FunctionWord = (uint8_t)(packetBlock->PacketFunctionWord);
    assembledPacketBuf = (uint8_t *)malloc(protocol_PacketLength * sizeof(uint8_t));//生成串字节流，做发送时在Unasked队列超时或响应接收时释放，做接收时在处理接收数据时被释放。
    
    memcpy(assembledPacketBuf + packetBufOffset,packetBlock->PacketHead,sizeof(packetBlock->PacketHead));packetBufOffset += sizeof(packetBlock->PacketHead);
    memcpy(assembledPacketBuf + packetBufOffset,&(packetBlock->PacketIndex),sizeof(packetBlock->PacketIndex));packetBufOffset += sizeof(packetBlock->PacketIndex);
    memcpy(assembledPacketBuf + packetBufOffset,&uint8FunctionWord,sizeof(uint8FunctionWord));packetBufOffset += sizeof(uint8FunctionWord);
    memcpy(assembledPacketBuf + packetBufOffset,&(packetBlock->PacketJSONDataLength),sizeof(packetBlock->PacketJSONDataLength));packetBufOffset += sizeof(packetBlock->PacketJSONDataLength);
    memcpy(assembledPacketBuf + packetBufOffset,packetBlock->PacketJSONData,packetBlock->PacketJSONDataLength);packetBufOffset += packetBlock->PacketJSONDataLength;
    memcpy(assembledPacketBuf + packetBufOffset,&(packetBlock->PacketCheckSum),sizeof(packetBlock->PacketCheckSum));packetBufOffset += sizeof(packetBlock->PacketCheckSum);
    
    free(packetBlock->PacketJSONData);
    free(packetBlock);
    return assembledPacketBuf;
}

/*组装一个包并压入未发送队列
*
*
*
*/
void AssembleProtocolPacketPushSendQueue(FunctionWord_TypeDef FunctionWord, uint16_t JSONMessageDataLength,uint8_t* JSONMessageData)
{
    uint8_t* assembledPacketBuf;
    PacketBlock* packetBlock;
    packetBlock = AssembleCommunicationStruct(FunctionWord, JSONMessageDataLength, JSONMessageData);
    assembledPacketBuf = ResolvePacketStructIntoBytes(packetBlock);
    Uint8PacketQueuePushData(UnsentPacketQueueHandle,assembledPacketBuf);
    Protocol_PacketSendIndex++;//包序号递增
}

void PushReceiveByteDataIntoReceiveQueue(uint8_t streamByteData)//对内封装，提供对外push进FIFO的接口
{
    Uint8FIFOPush(ReceiveBytesFIFOQueueHandle, streamByteData);
}

void LoadQueueByteToPacketBlock(Uint8FIFOQueue* uint8FIFOQueueHandle)
{
    static bool isCommunicationPacketReceiveEnd = true;
    static PacketBlock* packetBlock = NULL;
    uint16_t count;
    
    if(isCommunicationPacketReceiveEnd == true)
    {
        if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < AT_LEAST_PACKET_BYTES_LENGTH) return;
        if(!packetBlock)packetBlock = (PacketBlock*)malloc(sizeof(PacketBlock));//只有第一次会执行
        while(true)
        {
            if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) <= 0) return;    //长度不够时退出
            for(count=sizeof(Protocol_PacketHeadData)-1;count>1;count--)    //顺序移位
            {
                packetBlock->PacketHead[count] = packetBlock->PacketHead[count-1];
            }
            packetBlock->PacketHead[0] = Uint8FIFOPop(uint8FIFOQueueHandle);//获取最新值
            
            for(count=0;count<sizeof(Protocol_PacketHeadData);count++)      //比较是否相等
            {
                if(packetBlock->PacketHead[count] != Protocol_PacketHeadData[count])continue;
            }
            for(count=0;count<sizeof(Protocol_PacketHeadData);count++)      //到此步说明数组已相等，做清楚与退出
            {
                packetBlock->PacketHead[count] = 0;
            }
            break;
        }
        if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < AT_LEAST_PACKET_BYTES_LENGTH - sizeof(Protocol_PacketHeadData)) return;
        Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->PacketIndex)),sizeof(((PacketBlock*)0)->PacketIndex));
        Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->PacketFunctionWord)),sizeof((uint8_t)(((PacketBlock*)0)->PacketFunctionWord)));
        Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->PacketJSONDataLength)),sizeof(((PacketBlock*)0)->PacketJSONDataLength));
    }
    if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < packetBlock->PacketJSONDataLength + sizeof(((PacketBlock*)0)->PacketCheckSum))
    {
        isCommunicationPacketReceiveEnd = false;
        return;
    }
    else
    {
        packetBlock->PacketJSONData = (uint8_t*)malloc(packetBlock->PacketJSONDataLength * sizeof(uint8_t));
        Uint8FIFOPopToStream(uint8FIFOQueueHandle, packetBlock->PacketJSONData,packetBlock->PacketJSONDataLength);
        Uint8FIFOPopToStream(uint8FIFOQueueHandle, &(packetBlock->PacketCheckSum),sizeof(((PacketBlock*)0)->PacketCheckSum));
        isCommunicationPacketReceiveEnd = true;
        Uint8PacketQueuePushStruct(ReceivedPacketBlockQueueHandle, packetBlock);
        packetBlock = (PacketBlock*)malloc(sizeof(PacketBlock));
        return;
    }
}

void LoadReceiveQueueByteToPacketBlock()//对内封装，提供对外读取加载接收FIFO队列的接口
{
    LoadQueueByteToPacketBlock(ReceiveBytesFIFOQueueHandle);
}

void DealWithReceivePacketBlock(PacketBlock* packetBlock)
{
    FunctionWord_TypeDef PacketFunctionWord;
    if(!packetBlock)return;
    PacketFunctionWord = packetBlock->PacketFunctionWord;
    switch(PacketFunctionWord)
    {
        case FunctionWord_Data:
        {
            AssembleProtocolPacketPushSendQueue(packetBlock->PacketFunctionWord, packetBlock->PacketJSONDataLength, packetBlock->PacketJSONData);
        }
        break;
        
        default:
        break;
    }
}
void DealWithReceivePacketQueue()
{
    Uint8PacketNode* uint8PacketNodePointer;
    Uint8PacketNode* ReceivedPacketNodePointer;
    PacketBlock* packetBlock;
    uint8_t* assembledPacketBuf;
    
    while(true)
    {
        ReceivedPacketNodePointer = Uint8PacketQueuePop(ReceivedPacketBlockQueueHandle);
        if(!ReceivedPacketNodePointer)return;
        if(ReceivedPacketNodePointer && ReceivedPacketNodePointer->packetBlock)
        {
            if(ReceivedPacketNodePointer->packetBlock->PacketFunctionWord == FunctionWord_Acknowledgement)
            {
                DeleteAckedIndexPacket(ReceivedPacketNodePointer->index);
            }
            else
            {
                packetBlock = AssembleCommunicationStruct(FunctionWord_Acknowledgement, 0, NULL);
                packetBlock->PacketIndex = ReceivedPacketNodePointer->index;
                assembledPacketBuf = ResolvePacketStructIntoBytes(packetBlock);
                
                uint8PacketNodePointer = CreatUint8PacketNode(assembledPacketBuf, NULL);
                uint8PacketNodePointer->resendTime = PROTOCOL_PACKET_RESENT_TIME_MAX;
                uint8PacketNodePointer->resendCount = PROTOCOL_PACKET_RESENT_COUNT_MAX;
                Uint8PacketQueuePush(UnackedPacketQueueHandle, uint8PacketNodePointer);
                
                DealWithReceivePacketBlock(ReceivedPacketNodePointer->packetBlock);
                
            }
            FreePacketNoedItem(ReceivedPacketNodePointer);
        }
    }
}



