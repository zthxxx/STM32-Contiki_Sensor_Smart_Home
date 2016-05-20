#include "T6603.h"
#include "stdio.h"


const uint8_t T6603_Receive_HeadData[T6603_RECEIVE_PACKET_HEAD_LENGTH] = {0xFF,0xFA,0x02};
uint8_t T6603_Read_GAS_PPM_CMD[] = {0xFF,0xFE,0x02,0x02,0x03};

Uint8FIFOQueue T6603_ReceiveBytesFIFO_Queue;
Uint8FIFOQueue* T6603_ReceiveBytesFIFO_QueueHandle = &T6603_ReceiveBytesFIFO_Queue;
T6603_PacketNode T6603_GlobalPacket;

void T6603_Init(void)
{
	UART5_Config(19200);
}
void T6603_SendReadCMD(void)
{
    T6603_USART_SendBytesBuf(T6603_Read_GAS_PPM_CMD,sizeof(T6603_Read_GAS_PPM_CMD));
}

/*对内封装，提供对外push进接收FIFO的接口
*
*/
void T6603_PushReceiveByteIntoFIFO(uint8_t streamByteData)//对内封装，提供对外push进FIFO的接口
{
    Uint8FIFOPush(T6603_ReceiveBytesFIFO_QueueHandle, streamByteData);
}


/*从直节流FIFO中解包成块并添加到接收包队列
*
*
*/
void T6603_LoadQueueByteToPacketBlock(Uint8FIFOQueue* uint8FIFOQueueHandle)
{
    static bool isHeadAllEqual = false;
    T6603_PacketNode* T6603_packet_node;
    T6603_packet_node = &T6603_GlobalPacket;
    while(true)
    {
        if(!isHeadAllEqual)
        {
            if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < T6603_PROTOCOL_PACKET_LENGTH) return;
            while(true)//此处为接收帧头
            {
				uint8_t count;
                if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) <= 0) return;    //长度不够时退出                     
                for(count=0;count<sizeof(T6603_Receive_HeadData)-1;count++)          //顺序移位  先收到低字节
                {
                    T6603_packet_node->packetHead[count] = T6603_packet_node->packetHead[count+1];
                }
                T6603_packet_node->packetHead[sizeof(T6603_Receive_HeadData)-1] = Uint8FIFOPop(uint8FIFOQueueHandle);//获取最新值
          
                for(count=0,isHeadAllEqual=true;count<sizeof(T6603_Receive_HeadData);count++)      //比较是否相等
                {
                    if(T6603_packet_node->packetHead[count] != T6603_Receive_HeadData[count]){isHeadAllEqual=false;break;}
                }
                if(isHeadAllEqual)break;
            }
        }
        if(Uint8FIFOGetQueueSize(uint8FIFOQueueHandle) < T6603_PROTOCOL_PACKET_LENGTH - sizeof(T6603_Receive_HeadData)) return;
        T6603_packet_node->CO2_PPM_ByteHigh = Uint8FIFOPop(uint8FIFOQueueHandle);
        T6603_packet_node->CO2_PPM_ByteLow = Uint8FIFOPop(uint8FIFOQueueHandle);
        isHeadAllEqual = false;
        T6603_packet_node->CO2_PPM_Value = (((uint16_t)T6603_packet_node->CO2_PPM_ByteHigh) << 8) + T6603_packet_node->CO2_PPM_ByteLow;
    }
}


/*对内封装，提供对外读取加载接收FIFO队列的接口
*
*
*/
void T6603_LoadReceiveQueueByteToPacketBlock()
{
    T6603_LoadQueueByteToPacketBlock(T6603_ReceiveBytesFIFO_QueueHandle);
}

uint16_t T6603_getPacketNodeCO2_Value(T6603_PacketNode* T6603_packet_node)
{
    return T6603_packet_node->CO2_PPM_Value;
}
/*对内封装，提供对外读取PM值的接口
*
*
*/
uint16_t T6603_getCO2_PPM()
{
    return T6603_getPacketNodeCO2_Value(&T6603_GlobalPacket);
}








