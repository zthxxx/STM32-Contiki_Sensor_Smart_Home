#include "NVIC_CONFIG.H"


void NVIC_Configuration_Init()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}


void NVIC_IRQChannel_Configuration_Set(
    uint8_t NVIC_IRQChannel, 
    uint8_t NVIC_IRQChannelPreemptionPriority, 
    uint8_t NVIC_IRQChannelSubPriority, 
    FunctionalState NVIC_IRQChannelCmd
)
{
    NVIC_InitTypeDef NVIC_InitStructure; 

	NVIC_InitStructure.NVIC_IRQChannel = NVIC_IRQChannel;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
	NVIC_InitStructure.NVIC_IRQChannelCmd = NVIC_IRQChannelCmd;
	NVIC_Init(&NVIC_InitStructure);
}


