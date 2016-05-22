#include "ProtocolQueueManger.h"


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

/*对内封装，提供对外push进接收FIFO的接口
*
*/
void PushReceiveByteDataIntoReceiveFIFO(uint8_t streamByteData)//对内封装，提供对外push进FIFO的接口
{
    Uint8FIFOPush(ReceiveBytesFIFOQueueHandle, streamByteData);
}








