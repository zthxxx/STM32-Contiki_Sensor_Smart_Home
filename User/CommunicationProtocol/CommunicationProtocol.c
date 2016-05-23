#include "CommunicationProtocol.h"


Uint8PacketQueue UnsentPacketQueue = {NULL, NULL, CreatTianProtocolUint8PacketNode, FreeTianProtocolPacketNoedItem};
Uint8PacketQueue UnackedPacketQueue = {NULL, NULL, CreatTianProtocolUint8PacketNode, FreeTianProtocolPacketNoedItem};
Uint8PacketQueue ReceivedPacketQueue = {NULL, NULL, CreatTianProtocolUint8PacketNode, FreeTianProtocolPacketNoedItem};
Uint8FIFOQueue   TianMaoProtocolReceiveBytesFIFOQueue;

Uint8PacketQueue* UnsentPacketQueueHandle = &UnsentPacketQueue;
Uint8PacketQueue* UnackedPacketQueueHandle = &UnackedPacketQueue;
Uint8PacketQueue* ReceivedPacketBlockQueueHandle = &ReceivedPacketQueue;
Uint8FIFOQueue*   TianMaoProtocolReceiveBytesFIFOQueueHandle = &TianMaoProtocolReceiveBytesFIFOQueue;


const uint8_t Protocol_HeadData[PROTOCOL_PACKET_HEAD_LENGTH] = {0xEF,0x02,0xAA,0xAA};//固定包头
uint16_t Protocol_PacketSendIndex = 0;//连续增加的发送序号
uint16_t Protocol_PacketAckedIndex = 0;//作为缓存中转的接收序号



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
    
    TianProtocolSendBytesDataBuf(packet, protocol_messageDataLength + PROTOCOL_PACKET_CONSISTENT_LENGTH);
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
void FreeTianProtocolPacketNoedItem(Uint8PacketNode* uint8PacketNodePointer)
{
    if(!uint8PacketNodePointer)return;
    if(uint8PacketNodePointer->packet)
    {
        free(uint8PacketNodePointer->packet);
        uint8PacketNodePointer->packet = NULL;
    }
    
    if(uint8PacketNodePointer->packetBlock)
    {
        if(((PacketBlock*)uint8PacketNodePointer->packetBlock)->messageData)
        {
            free(((PacketBlock*)uint8PacketNodePointer->packetBlock)->messageData);
            ((PacketBlock*)uint8PacketNodePointer->packetBlock)->messageData = NULL;
        }
        free(uint8PacketNodePointer->packetBlock);
        uint8PacketNodePointer->packetBlock = NULL;
    }
    free(uint8PacketNodePointer);
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

/*通过包数据或者数据块创建一个包节点，用于加入队列等操作
*packet:数据包
*packetBlock:数据块
*返回新建的包节点指针
*
*/
Uint8PacketNode* CreatTianProtocolUint8PacketNode(uint8_t* packet, void* packetBlock)
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
        uint8PacketNodePointer->index = ((PacketBlock*)packetBlock)->index;
    }
    else
    {
        uint8PacketNodePointer->index = 0;
    }
    
    uint8PacketNodePointer->resendCount = 0;
    uint8PacketNodePointer->resendTime = 0;
    return uint8PacketNodePointer;
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
    if(!packetBlock->messageData)
    {
        free(packetBlock);
        packetBlock = NULL;
        return NULL;
    }
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
    packetBlock->messageData = NULL;
    free(packetBlock);
    packetBlock = NULL;
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
    Uint8PacketQueuePushStreamData(UnsentPacketQueueHandle,assembledPacketBuf,packetBlock->messageDataLength + PROTOCOL_PACKET_CONSISTENT_LENGTH);
    Protocol_PacketSendIndex++;//包序号递增
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
    static bool isHeadAllEqual = false;
    
    if(!packetBlock){packetBlock = (PacketBlock*)malloc(sizeof(PacketBlock));for(count=0;count<sizeof(Protocol_HeadData);count++)packetBlock->head[count] = 0;packetBlock->functionWord = FunctionWord_Null;}/*只有第一次会执行,清零头部和功能字*/
    while(true)
    {
        if(isCommunicationPacketReceiveEnd == true)
        {
            if(!isHeadAllEqual)
            {
                if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < PROTOCOL_PACKET_CONSISTENT_LENGTH) return;
                while(true)//此处为接收帧头
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
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->targetAddress)),sizeof(((PacketBlock*)0)->targetAddress));//目标地址
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->sourceAddress)),sizeof(((PacketBlock*)0)->sourceAddress));//源地址
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->index)),sizeof(((PacketBlock*)0)->index));//包序号ID
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->functionWord)),sizeof((uint8_t)(((PacketBlock*)0)->functionWord)));//功能字
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->messageDataLength)),sizeof(((PacketBlock*)0)->messageDataLength));//数据内容长度
            Uint8FIFOPopToStream(uint8FIFOQueueHandle, (uint8_t*)(&(packetBlock->headCheckSum)),sizeof(((PacketBlock*)0)->headCheckSum));//首部校验和
            if(packetBlock->headCheckSum != CalculatePacketBlockHeadCheckSum(packetBlock))
            {
                isHeadAllEqual = false;
                for(count=0;count<sizeof(Protocol_HeadData);count++)packetBlock->head[count] = 0;
                printf("Bad block head\r\n");
                continue;
            }
        }
        isCommunicationPacketReceiveEnd = false;
        if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < packetBlock->messageDataLength + sizeof(((PacketBlock*)0)->messageDataCheckSum))return;
       
        packetBlock->messageData = (uint8_t*)malloc(packetBlock->messageDataLength * sizeof(uint8_t));
        Uint8FIFOPopToStream(uint8FIFOQueueHandle, packetBlock->messageData,packetBlock->messageDataLength);//数据内容
        Uint8FIFOPopToStream(uint8FIFOQueueHandle, &(packetBlock->messageDataCheckSum),sizeof(((PacketBlock*)0)->messageDataCheckSum));//数据校验和
        isCommunicationPacketReceiveEnd = true;
        isHeadAllEqual = false;
        if(packetBlock->messageDataCheckSum != CalculatePacketBlockMessageDataCheckSum(packetBlock))
        {
            free(packetBlock->messageData);
            packetBlock->messageData = NULL;
            free(packetBlock);
            packetBlock = NULL;
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
/*对内封装，提供对外push进接收FIFO的接口
*
*/
void PushTianProtocolReceiveByteIntoFIFO(uint8_t streamByteData)//对内封装，提供对外push进FIFO的接口
{
    Uint8FIFOPush(TianMaoProtocolReceiveBytesFIFOQueueHandle, streamByteData);
}
/*对内封装，提供对外读取加载接收FIFO队列的接口
*
*
*/
void LoadReceiveQueueByteToPacketBlock()
{
    LoadQueueByteToPacketBlock(TianMaoProtocolReceiveBytesFIFOQueueHandle);
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
//    uint8_t* assembledPacketBuf;
    
    while(true)
    {
        ReceivedPacketNodePointer = Uint8PacketQueuePop(ReceivedPacketBlockQueueHandle);
        if(!ReceivedPacketNodePointer)return;
        packetBlock = ReceivedPacketNodePointer->packetBlock;
        if(packetBlock)
        {
            if((packetBlock->targetAddress == Protocol_LocalhostAddress) || (packetBlock->targetAddress == Protocol_BroadcastAddress))
            {
                if(packetBlock->functionWord == FunctionWord_Acknowledgement)
                {
                    DeleteAckedIndexPacket(packetBlock->index);
                }
                else
                {
                    /**发送回应包**/
//                    packetBlock = AssembleProtocolPacketBlock(packetBlock->sourceAddress,Protocol_LocalhostAddress,FunctionWord_Acknowledgement, 0, NULL);
//                    packetBlock->index = ReceivedPacketNodePointer->index;
//                    assembledPacketBuf = ResolvePacketStructIntoBytes(packetBlock);
//                    
//                    uint8PacketNodePointer = CreatTianProtocolUint8PacketNode(assembledPacketBuf, NULL);
//                    uint8PacketNodePointer->resendTime = PROTOCOL_PACKET_RESENT_TIME_MAX;
//                    uint8PacketNodePointer->resendCount = PROTOCOL_PACKET_RESENT_COUNT_MAX;
//                    Uint8PacketQueuePush(UnackedPacketQueueHandle, uint8PacketNodePointer);
                    
                    packetBlock = ReceivedPacketNodePointer->packetBlock;
                    ReceivedPacketNodePointer->packetBlock = NULL;
                    DealWithReceivePacketBlock(packetBlock); 
                }
            }
        }
        ReceivedPacketBlockQueueHandle->FreePacketNoedItem(ReceivedPacketNodePointer);
    }
}



