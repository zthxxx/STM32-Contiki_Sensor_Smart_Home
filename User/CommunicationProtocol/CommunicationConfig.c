#include "CommunicationConfig.h"



//使用各种地址，如本地、回环、广播、网关地址
//终端地址为1H，节点地址应与其不同
#ifdef __TERMINAL_ON__
    const uint16_t Protocol_LocalhostAddress = 0x0004;
#else
    #ifdef __TERMINAL_OFF__
    const uint16_t Protocol_LocalhostAddress = 0x0003;
    #endif
#endif
const uint16_t Protocol_LoopbackAddress = 0x0000;
const uint16_t Protocol_BroadcastAddress = 0xFFFF;
const uint16_t Protocol_GatewayAddress = 0x0001;








