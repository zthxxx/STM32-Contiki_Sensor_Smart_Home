#include "CommunicationProtocol.h"


const uint8_t Protocol_HeadData[PROTOCOL_PACKET_HEAD_LENGTH] = {0xEF,0x02,0xAA,0xAA};
const uint16_t Protocol_LocalhostAddress = 0x0001;
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





/*创建一个发送包的队列
*返回一个管理包的指针
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
/*通过包数据或者数据块创建一个包节点，用于加入队列等操作
*packet:数据包
*packetBlock:数据块
*返回新建的包节点指针
*
*/
Uint8PacketNode* CreatUint8PacketNode(uint8_t* packet, PacketBlock* packetBlock)
{
    Uint8PacketNode* uint8PacketNodePointer;
    uint8_t packetIndexPosition;
    packetIndexPosition = sizeof(((PacketBlock*)0)->head);
    
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
        uint8PacketNodePointer->index = packetBlock->index;
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
void Uint8PacketQueuePushBlock(Uint8PacketQueue* Uint8PacketQueueHandle,PacketBlock* packetBlock)
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
void Uint8PacketQueuePushStreamData(Uint8PacketQueue* Uint8PacketQueueHandle,uint8_t* packet)
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
*/
void SendOneCommunicationPacketNode(Uint8PacketNode* uint8PacketNodePointer)
{
    uint16_t protocol_messageDataLength;
    uint8_t* packet;
    uint8_t messageDataLengthPosition;
    messageDataLengthPosition = sizeof(((PacketBlock*)0)->head)+sizeof(((PacketBlock*)0)->targetAddress)+sizeof(((PacketBlock*)0)->sourceAddress)+sizeof(((PacketBlock*)0)->index)+sizeof((uint8_t)(((PacketBlock*)0)->functionWord));
    if(!uint8PacketNodePointer)return;
    if(!uint8PacketNodePointer->packet)return;
    packet = uint8PacketNodePointer->packet;
    protocol_messageDataLength = packet[messageDataLengthPosition];      //长度所在位置
    protocol_messageDataLength += packet[messageDataLengthPosition+1]<<8;
    
    sendUartByteBuf(packet, protocol_messageDataLength + PROTOCOL_PACKET_CONSISTENT_LENGTH);
}

/*发送一整个未发送队列数据
*
*/
void SendUnsentPacketQueue()
{
    Uint8PacketNode* uint8PacketNodePointer;
    while(UnsentPacketQueueHandle->head)
    {
        uint8PacketNodePointer = Uint8PacketQueuePop(UnsentPacketQueueHandle);
        SendOneCommunicationPacketNode(uint8PacketNodePointer);
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
    if(uint8PacketNodePointer->packet)free(uint8PacketNodePointer->packet);
    if(uint8PacketNodePointer->packetBlock)
    {
        if(uint8PacketNodePointer->packetBlock->messageData)free(uint8PacketNodePointer->packetBlock->messageData);
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
*/
bool UnackedPacketRetimeAndRecountCondition(Uint8PacketNode* uint8PacketNodePointer)
{
    if(!uint8PacketNodePointer)return false;
    if(uint8PacketNodePointer->resendTime >= PROTOCOL_PACKET_RESENT_TIME_MAX)
    {
        uint8PacketNodePointer->resendTime = 0;
        SendOneCommunicationPacketNode(uint8PacketNodePointer);
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
*/
void SendUnackedPacketQueue()
{    
    DeletPacketQueueConditionalItem(UnackedPacketQueueHandle, UnackedPacketRetimeAndRecountCondition);
    DeletPacketQueueConditionalItem(UnackedPacketQueueHandle, UnackedPacketAllDeletCondition);
}

/*增加未回应队列的发送计数时间
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
*/
void DeleteAckedIndexPacket(uint16_t packetAckedIndex)
{
    Protocol_PacketAckedIndex = packetAckedIndex;
    DeletPacketQueueConditionalItem(UnackedPacketQueueHandle, UnackedPacketAckIndexCondition);
}
/*计算一个数据块头部固定字节的校验和
*
*/
uint8_t CalculatePacketBlockHeadCheckSum(PacketBlock* packetBlock)
{
    uint8_t headLength;
    uint8_t headCheckSum = 0;
    if(!packetBlock)return NULL;
    headLength = sizeof(packetBlock->head);
    while(headLength-- > 0)
    {
        headCheckSum += packetBlock->head[headLength];
    }
    headCheckSum += (uint8_t)(packetBlock->targetAddress & 0x00FF);
    headCheckSum += (uint8_t)((packetBlock->targetAddress >> 8) & 0x00FF);
    headCheckSum += (uint8_t)(packetBlock->sourceAddress & 0x00FF);
    headCheckSum += (uint8_t)((packetBlock->sourceAddress >> 8) & 0x00FF);
    headCheckSum += (uint8_t)(packetBlock->index & 0x00FF);
    headCheckSum += (uint8_t)((packetBlock->index >> 8) & 0x00FF);
    headCheckSum += (uint8_t)packetBlock->functionWord;
    headCheckSum += (uint8_t)(packetBlock->messageDataLength & 0x00FF);
    headCheckSum += (uint8_t)((packetBlock->messageDataLength >> 8) & 0x00FF);
    return headCheckSum;
}
/*计算一个数据块的校验和
*
*/
uint8_t CalculatePacketBlockMessageDataCheckSum(PacketBlock* packetBlock)
{
    uint16_t messageDataLength;
    uint8_t messageDataCheckSum = 0;
    if(!packetBlock)return NULL;
    if(!packetBlock->messageData)return NULL;
    
    messageDataLength = packetBlock->messageDataLength;

    while(messageDataLength-- > 0)
    {
        messageDataCheckSum += packetBlock->messageData[messageDataLength];
    }
    return messageDataCheckSum;
}

/*组装一个结构体表示包的信息
*
*/
PacketBlock* AssembleProtocolPacketBlock(uint16_t targetAddress, uint16_t sourceAddress, FunctionWord_TypeDef FunctionWord, uint16_t MessageDataLength,uint8_t* MessageData)
{
    PacketBlock* packetBlock;
    packetBlock = (PacketBlock*)malloc(sizeof(PacketBlock));//生成一个数据包结构体，作为发送时在生成字节流时释放;作为接收时，在处理接收数据时被释放
    memcpy(packetBlock->head,Protocol_HeadData,sizeof(Protocol_HeadData));
    packetBlock->targetAddress = targetAddress;
    packetBlock->sourceAddress = sourceAddress;
    packetBlock->index = Protocol_PacketSendIndex;
    packetBlock->functionWord = FunctionWord;
    packetBlock->messageDataLength = MessageDataLength;
    packetBlock->headCheckSum = CalculatePacketBlockHeadCheckSum(packetBlock);
    packetBlock->messageData = MessageData;
    packetBlock->messageDataCheckSum = CalculatePacketBlockMessageDataCheckSum(packetBlock);
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
    if(!packetBlock->messageData){free(packetBlock);return NULL;}
    protocol_PacketLength = packetBlock->messageDataLength + PROTOCOL_PACKET_CONSISTENT_LENGTH;
    uint8FunctionWord = (uint8_t)(packetBlock->functionWord);
    assembledPacketBuf = (uint8_t *)malloc(protocol_PacketLength * sizeof(uint8_t));//生成串字节流，做发送时在Unasked队列超时或响应接收时释放，做接收时在处理接收数据时被释放。
    
    memcpy(assembledPacketBuf + packetBufOffset,packetBlock->head,sizeof(packetBlock->head));packetBufOffset += sizeof(packetBlock->head);
    memcpy(assembledPacketBuf + packetBufOffset,&(packetBlock->targetAddress),sizeof(packetBlock->targetAddress));packetBufOffset += sizeof(packetBlock->targetAddress);
    memcpy(assembledPacketBuf + packetBufOffset,&(packetBlock->sourceAddress),sizeof(packetBlock->sourceAddress));packetBufOffset += sizeof(packetBlock->sourceAddress);
    memcpy(assembledPacketBuf + packetBufOffset,&(packetBlock->index),sizeof(packetBlock->index));packetBufOffset += sizeof(packetBlock->index);
    memcpy(assembledPacketBuf + packetBufOffset,&uint8FunctionWord,sizeof(uint8FunctionWord));packetBufOffset += sizeof(uint8FunctionWord);
    memcpy(assembledPacketBuf + packetBufOffset,&(packetBlock->messageDataLength),sizeof(packetBlock->messageDataLength));packetBufOffset += sizeof(packetBlock->messageDataLength);
    memcpy(assembledPacketBuf + packetBufOffset,&(packetBlock->headCheckSum),sizeof(packetBlock->headCheckSum));packetBufOffset += sizeof(packetBlock->headCheckSum);
    memcpy(assembledPacketBuf + packetBufOffset,packetBlock->messageData,packetBlock->messageDataLength);packetBufOffset += packetBlock->messageDataLength;
    memcpy(assembledPacketBuf + packetBufOffset,&(packetBlock->messageDataCheckSum),sizeof(packetBlock->messageDataCheckSum));packetBufOffset += sizeof(packetBlock->messageDataCheckSum);
    
    free(packetBlock->messageData);
    free(packetBlock);
    return assembledPacketBuf;
}

/*组装一个包并压入未发送队列
*
*
*/
void AssembleProtocolPacketPushSendQueue(uint16_t targetAddress, FunctionWord_TypeDef FunctionWord, uint16_t MessageDataLength,uint8_t* MessageData)
{
    uint8_t* assembledPacketBuf;
    PacketBlock* packetBlock;
    packetBlock = AssembleProtocolPacketBlock(targetAddress, Protocol_LocalhostAddress,FunctionWord, MessageDataLength, MessageData);
    assembledPacketBuf = ResolvePacketStructIntoBytes(packetBlock);
    Uint8PacketQueuePushStreamData(UnsentPacketQueueHandle,assembledPacketBuf);
    Protocol_PacketSendIndex++;//包序号递增
}
/*对内封装，提供对外push进接收FIFO的接口
*
*/
void PushReceiveByteDataIntoReceiveFIFO(uint8_t streamByteData)//对内封装，提供对外push进FIFO的接口
{
    Uint8FIFOPush(ReceiveBytesFIFOQueueHandle, streamByteData);
}
/*从直节流FIFO中解包成块并添加到接收包队列
*
*
*/
void LoadQueueByteToPacketBlock(Uint8FIFOQueue* uint8FIFOQueueHandle)
{
    static bool isCommunicationPacketReceiveEnd = true;
    static PacketBlock* packetBlock = NULL;
    uint16_t count;
    bool isHeadAllEqual = false;
    
    if(!packetBlock){packetBlock = (PacketBlock*)malloc(sizeof(PacketBlock));for(count=0;count<sizeof(Protocol_HeadData);count++)packetBlock->head[count] = 0;packetBlock->functionWord = FunctionWord_Null;}/*只有第一次会执行,清零头部和功能字*/
    while(true)
    {
        if(isCommunicationPacketReceiveEnd == true)
        {
            if(!isHeadAllEqual)
            {
                if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < PROTOCOL_PACKET_CONSISTENT_LENGTH) return;
                while(true)
                {
                    if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) <= 0) return;    //长度不够时退出     
                    for(count=0;count<sizeof(Protocol_HeadData)-1;count++)          //顺序移位  先收到低字节
                    {
                        packetBlock->head[count] = packetBlock->head[count+1];
                    }
                    packetBlock->head[sizeof(Protocol_HeadData)-1] = Uint8FIFOPop(uint8FIFOQueueHandle);//获取最新值
              
                    for(count=0,isHeadAllEqual=true;count<sizeof(Protocol_HeadData);count++)      //比较是否相等
                    {
                        if(packetBlock->head[count] != Protocol_HeadData[count])isHeadAllEqual=false;
                    }
                    if(isHeadAllEqual)break;
                }
            }
            if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < PROTOCOL_PACKET_CONSISTENT_LENGTH - sizeof(Protocol_HeadData)) return;
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->targetAddress)),sizeof(((PacketBlock*)0)->targetAddress));
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->sourceAddress)),sizeof(((PacketBlock*)0)->sourceAddress));
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->index)),sizeof(((PacketBlock*)0)->index));
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->functionWord)),sizeof((uint8_t)(((PacketBlock*)0)->functionWord)));
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->messageDataLength)),sizeof(((PacketBlock*)0)->messageDataLength));
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->headCheckSum)),sizeof(((PacketBlock*)0)->headCheckSum));
            if(packetBlock->headCheckSum != CalculatePacketBlockHeadCheckSum(packetBlock))
            {
                isHeadAllEqual = false;
                for(count=0;count<sizeof(Protocol_HeadData);count++)packetBlock->head[count] = 0;
                continue;
            }
        }
        isCommunicationPacketReceiveEnd = false;
        if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < packetBlock->messageDataLength + sizeof(((PacketBlock*)0)->messageDataCheckSum))return;
       
        packetBlock->messageData = (uint8_t*)malloc(packetBlock->messageDataLength * sizeof(uint8_t));
        Uint8FIFOPopToStream(uint8FIFOQueueHandle, packetBlock->messageData,packetBlock->messageDataLength);
        Uint8FIFOPopToStream(uint8FIFOQueueHandle, &(packetBlock->messageDataCheckSum),sizeof(((PacketBlock*)0)->messageDataCheckSum));
        isCommunicationPacketReceiveEnd = true;
        isHeadAllEqual = false;
        if(packetBlock->messageDataCheckSum != CalculatePacketBlockMessageDataCheckSum(packetBlock))
        {
            free(packetBlock->messageData);
            free(packetBlock);
            printf("Bad block\r\n");
        }
        else
        {
            Uint8PacketQueuePushBlock(ReceivedPacketBlockQueueHandle, packetBlock);
        }
        packetBlock = (PacketBlock*)malloc(sizeof(PacketBlock));
        for(count=0;count<sizeof(Protocol_HeadData);count++)packetBlock->head[count] = 0;//清零头部
        packetBlock->functionWord = FunctionWord_Null;
    }
}
/*对内封装，提供对外读取加载接收FIFO队列的接口
*
*
*/
void LoadReceiveQueueByteToPacketBlock()
{
    LoadQueueByteToPacketBlock(ReceiveBytesFIFOQueueHandle);
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
            printf("%d\t%d\r\n",Protocol_PacketSendIndex++,packetBlock->messageDataLength);
            Uint8PacketQueuePushStreamData(UnsentPacketQueueHandle,ResolvePacketStructIntoBytes(packetBlock));
        }
        break;
        
        default:
            free(packetBlock->messageData);
            free(packetBlock);
        break;
    }
}
/*对接收包队列每个数据块进行处理
*
*
*/
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
            if(ReceivedPacketNodePointer->packetBlock->functionWord == FunctionWord_Acknowledgement)
            {
                DeleteAckedIndexPacket(ReceivedPacketNodePointer->index);
            }
            else
            {
                /**发送回应包**/
//                packetBlock = AssembleProtocolPacketBlock(FunctionWord_Acknowledgement, 0, NULL);
//                packetBlock->index = ReceivedPacketNodePointer->index;
//                assembledPacketBuf = ResolvePacketStructIntoBytes(packetBlock);
//                
//                uint8PacketNodePointer = CreatUint8PacketNode(assembledPacketBuf, NULL);
//                uint8PacketNodePointer->resendTime = PROTOCOL_PACKET_RESENT_TIME_MAX;
//                uint8PacketNodePointer->resendCount = PROTOCOL_PACKET_RESENT_COUNT_MAX;
//                Uint8PacketQueuePush(UnackedPacketQueueHandle, uint8PacketNodePointer);
                
                packetBlock = ReceivedPacketNodePointer->packetBlock;
                ReceivedPacketNodePointer->packetBlock = NULL;
                DealWithReceivePacketBlock(packetBlock); 
            }
        }
        FreePacketNoedItem(ReceivedPacketNodePointer);
    }
}



