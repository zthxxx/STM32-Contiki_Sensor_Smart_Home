#include "SDS01.h"
#include "stdio.h"

const uint8_t SDS01_Protocol_HeadData = 0xAA;
const uint8_t SDS01_Protocol_EnddingData = 0xAB;
Uint8FIFOQueue SDS01_ReceiveBytesFIFO_Queue;
Uint8FIFOQueue* SDS01_ReceiveBytesFIFO_QueueHandle = &SDS01_ReceiveBytesFIFO_Queue;
SDS01_PacketNode SDS01_GlobalPacket;

void SDS01_Init(void)
{
	UART4_Config(9600);
}

/*对内封装，提供对外push进接收FIFO的接口
*
*/
void SDS01_PushReceiveByteIntoFIFO(uint8_t streamByteData)//对内封装，提供对外push进FIFO的接口
{
    Uint8FIFOPush(SDS01_ReceiveBytesFIFO_QueueHandle, streamByteData);
}

uint8_t SDS01_CalculatePacketCheckSum(SDS01_PacketNode* SDS01_packet_node)
{
    uint8_t checkSum = 0;
    checkSum += SDS01_packet_node->PM2_5_ByteLow;
    checkSum += SDS01_packet_node->PM2_5_ByteHigh;
    checkSum += SDS01_packet_node->PM10_ByteLow;
    checkSum += SDS01_packet_node->PM10_ByteHigh;
    checkSum += SDS01_packet_node->ID_ByteLow;
    checkSum += SDS01_packet_node->ID_ByteHigh;
    return checkSum;
}

/*从直节流FIFO中解包成块并添加到接收包队列
*
*
*/
void SDS01_LoadQueueByteToPacketBlock(Uint8FIFOQueue* uint8FIFOQueueHandle)
{
    static bool isHeadAllEqual = false;
    SDS01_PacketNode* SDS01_packet_node;
    SDS01_packet_node = &SDS01_GlobalPacket;
    while(true)
    {
        if(!isHeadAllEqual)
        {
            if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < SDS01_PROTOCOL_PACKET_LENGTH) return;
            while(true)//此处为接收帧头
            {
                if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) <= 0) return;    //长度不够时退出     
                SDS01_packet_node->packetHead = Uint8FIFOPop(uint8FIFOQueueHandle);//获取最新值
                isHeadAllEqual=true;
                if(SDS01_packet_node->packetHead != SDS01_Protocol_HeadData)isHeadAllEqual=false;
                if(isHeadAllEqual)break;
            }
        }
        if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < SDS01_PROTOCOL_PACKET_LENGTH - sizeof(SDS01_Protocol_HeadData)) return;
        SDS01_packet_node->signalType = Uint8FIFOPop(uint8FIFOQueueHandle);
        SDS01_packet_node->PM2_5_ByteLow = Uint8FIFOPop(uint8FIFOQueueHandle);
        SDS01_packet_node->PM2_5_ByteHigh = Uint8FIFOPop(uint8FIFOQueueHandle);
        SDS01_packet_node->PM10_ByteLow = Uint8FIFOPop(uint8FIFOQueueHandle);
        SDS01_packet_node->PM10_ByteHigh = Uint8FIFOPop(uint8FIFOQueueHandle);
        SDS01_packet_node->ID_ByteLow = Uint8FIFOPop(uint8FIFOQueueHandle);
        SDS01_packet_node->ID_ByteHigh = Uint8FIFOPop(uint8FIFOQueueHandle);
        SDS01_packet_node->checkSum = Uint8FIFOPop(uint8FIFOQueueHandle);
        SDS01_packet_node->packetEndding = Uint8FIFOPop(uint8FIFOQueueHandle);
        isHeadAllEqual = false;
        if((SDS01_packet_node->checkSum != SDS01_CalculatePacketCheckSum(SDS01_packet_node)) || (SDS01_packet_node->packetEndding != SDS01_Protocol_EnddingData))
        {
            printf("Bad SDS01 packet check\r\n");
            continue;
        }
        SDS01_packet_node->PM2_5_Value = ((((uint16_t)SDS01_packet_node->PM2_5_ByteHigh) << 8) + SDS01_packet_node->PM2_5_ByteLow) / 10.0;
        SDS01_packet_node->PM10_Value = ((((uint16_t)SDS01_packet_node->PM10_ByteHigh) << 8) + SDS01_packet_node->PM10_ByteLow) / 10.0;
    }
}


/*对内封装，提供对外读取加载接收FIFO队列的接口
*
*
*/
void SDS01_LoadReceiveQueueByteToPacketBlock()
{
    SDS01_LoadQueueByteToPacketBlock(SDS01_ReceiveBytesFIFO_QueueHandle);
}

float SDS01_getPacketNodePM2_5_Value(SDS01_PacketNode* SDS01_packet_node)
{
    return SDS01_packet_node->PM2_5_Value;
}
float SDS01_getPacketNodePM10_Value(SDS01_PacketNode* SDS01_packet_node)
{
    return SDS01_packet_node->PM10_Value;
}
/*对内封装，提供对外读取PM值的接口
*
*
*/
float SDS01_getPM2_5_Value()
{
    return SDS01_getPacketNodePM2_5_Value(&SDS01_GlobalPacket);
}

/*对内封装，提供对外读取PM值的接口
*
*
*/
float SDS01_getPM10_Value()
{
    return SDS01_getPacketNodePM10_Value(&SDS01_GlobalPacket);
}








