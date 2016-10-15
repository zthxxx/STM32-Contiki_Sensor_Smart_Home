#include "Uint8PacketQueueManger.h"


/*创建一个发送包的队列
*返回一个管理包的指针
*/
Uint8PacketQueue* CreatUint8PacketQueue(Uint8PacketNode* (*CreatUint8PacketNode)(uint8_t* packet, void* packetBlock))
{
    Uint8PacketQueue*  Uint8PacketQueueHandle = NULL;
    Uint8PacketQueueHandle = (Uint8PacketQueue*)malloc(sizeof(Uint8PacketQueue));//作为全局的管理队列对象，只生成一次，不释放
    if(Uint8PacketQueueHandle == NULL)
    {
        return NULL;
    }
    Uint8PacketQueueHandle->head = NULL;
    Uint8PacketQueueHandle->last = NULL;
    Uint8PacketQueueHandle->CreatUint8PacketNode = CreatUint8PacketNode;
    return Uint8PacketQueueHandle;
}



/*将一个发送包节点压入包队列中
*Uint8PacketQueueHandle:要压入的管理包指针
*uint8PacketNodePointer:发送包节点
*/
void Uint8PacketQueuePush(Uint8PacketQueue* Uint8PacketQueueHandle,Uint8PacketNode* uint8PacketNodePointer)
{
    if(!uint8PacketNodePointer || !Uint8PacketQueueHandle)return;
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
Uint8PacketNode* CreatUint8PacketNode(uint8_t* packet, void* packetBlock)
{
    Uint8PacketNode* uint8PacketNodePointer;
    
    uint8PacketNodePointer = (Uint8PacketNode*)malloc(sizeof(Uint8PacketNode));
    if(!uint8PacketNodePointer)return NULL;
    
    uint8PacketNodePointer->packet = packet;
    uint8PacketNodePointer->packetLength = 0;
    uint8PacketNodePointer->packetBlock = packetBlock;
    uint8PacketNodePointer->next = NULL;
    uint8PacketNodePointer->index = 0;
    uint8PacketNodePointer->resendCount = 0;
    uint8PacketNodePointer->resendTime = 0;
    return uint8PacketNodePointer;
}

Uint8PacketNode* SetUint8PacketNodeLength(Uint8PacketNode* uint8PacketNodePointer,uint16_t packetLength)
{
    if(!uint8PacketNodePointer)return NULL;
    uint8PacketNodePointer->packetLength = packetLength;
    return uint8PacketNodePointer;
}

/*将一个数据包结构体节点压入包队列中
*Uint8PacketQueueHandle:要压入的管理包指针
*packetBlock:数据包指针
*
*
*/
void Uint8PacketQueuePushBlock(Uint8PacketQueue* Uint8PacketQueueHandle,void* packetBlock)
{
    Uint8PacketNode* uint8PacketNodePointer;
    if(!Uint8PacketQueueHandle || !packetBlock)return;
    uint8PacketNodePointer = Uint8PacketQueueHandle->CreatUint8PacketNode(NULL, packetBlock);
    Uint8PacketQueuePush(Uint8PacketQueueHandle, uint8PacketNodePointer);
}

/*将一个数据包节点压入包队列中
*Uint8PacketQueueHandle:要压入的管理包指针
*packet:数据包指针
*
*
*/
void Uint8PacketQueuePushStreamData(Uint8PacketQueue* Uint8PacketQueueHandle,uint8_t* packet,uint16_t packetLength)
{
    Uint8PacketNode* uint8PacketNodePointer;
    if(!Uint8PacketQueueHandle || !packet)return;
    uint8PacketNodePointer = Uint8PacketQueueHandle->CreatUint8PacketNode(packet, NULL);
    if(!uint8PacketNodePointer)return;
    SetUint8PacketNodeLength(uint8PacketNodePointer, packetLength);
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


/*释放一个包节点的空间
*uint8PacketNodePointer：要释放的项节点指针
*
*/
void FreePacketNoedItem(Uint8PacketNode* uint8PacketNodePointer)
{
    if(!uint8PacketNodePointer)return;
    if(uint8PacketNodePointer->packet)
    {
        free(uint8PacketNodePointer->packet);
        uint8PacketNodePointer->packet = NULL;
    }
    if(uint8PacketNodePointer->packetBlock)
    {
        free(uint8PacketNodePointer->packetBlock);
        uint8PacketNodePointer->packetBlock = NULL;
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
    if(!PacketQueueHandle || !*uint8PacketNodePointer)return;
    if(!*uint8PacketNodePreviousPointer)         //上一项不存在  相当于为头节点
    {
        *uint8PacketNodePreviousPointer = *uint8PacketNodePointer;
        *uint8PacketNodePointer = (*uint8PacketNodePointer)->next;
        PacketQueueHandle->FreePacketNoedItem(*uint8PacketNodePreviousPointer);      
        *uint8PacketNodePreviousPointer = NULL;
        PacketQueueHandle->head = *uint8PacketNodePointer;
    }
    else
    {
        (*uint8PacketNodePreviousPointer)->next = (*uint8PacketNodePointer)->next;
        PacketQueueHandle->FreePacketNoedItem(*uint8PacketNodePreviousPointer);        
        *uint8PacketNodePointer = (*uint8PacketNodePreviousPointer)->next;
        if(!*uint8PacketNodePointer)            //下一项不存在  相当于为尾节点
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
    if(!PacketQueueHandle || !PacketQueueItemCondition)return;
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





