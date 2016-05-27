#include "Keyboard_4x5.h"

Uint8FIFOQueue KEYBOARD_Button_Queue = {NULL, NULL, 0};
Uint8FIFOQueue* KEYBOARD_Button_QueueHandle = &KEYBOARD_Button_Queue;

GPIO_Pin KEYBOARD_ROW_Pin[] = {KEY_ROW_1_Pin, KEY_ROW_2_Pin, KEY_ROW_3_Pin, KEY_ROW_4_Pin};
GPIO_TypeDef* KEYBOARD_ROW_Port[] = {KEY_ROW_1_Port, KEY_ROW_2_Port, KEY_ROW_3_Port, KEY_ROW_4_Port};
GPIO_Pin KEYBOARD_COL_Pin[] = {KEY_COL_1_Pin, KEY_COL_2_Pin, KEY_COL_3_Pin, KEY_COL_4_Pin, KEY_COL_5_Pin};
GPIO_TypeDef* KEYBOARD_COL_Port[] = {KEY_COL_1_Port, KEY_COL_2_Port, KEY_COL_3_Port, KEY_COL_4_Port, KEY_COL_5_Port};
//按键从1开始编码  0表示没有按键 编号行优先  先行后列 
// [1] [2] [3]
// [4] [5] [6]
// [7] [8] [9]

void KEYBOARD_Set_ROW_Output_Low(void)//做为输出口，输出低电平
{
	uint8_t count;
    uint8_t keyboard_Row_length = sizeof(KEYBOARD_ROW_Pin) / sizeof(*KEYBOARD_ROW_Pin);
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	for(count = 0;count < keyboard_Row_length;count++)
	{
		GPIO_InitStructure.GPIO_Pin = KEYBOARD_ROW_Pin[count];
		GPIO_Init(KEYBOARD_ROW_Port[count], &GPIO_InitStructure);
		GPIO_ResetBits(KEYBOARD_ROW_Port[count], KEYBOARD_ROW_Pin[count]);
	}
}

void KEYBOARD_Set_ROW_Input_IPU(void)//做为输输入口，常高，接入低电平按键变低
{
	uint8_t count;
    uint8_t keyboard_Row_length = sizeof(KEYBOARD_ROW_Pin) / sizeof(*KEYBOARD_ROW_Pin);
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	for(count = 0;count < keyboard_Row_length;count++)
	{
		GPIO_InitStructure.GPIO_Pin = KEYBOARD_ROW_Pin[count];
		GPIO_Init(KEYBOARD_ROW_Port[count], &GPIO_InitStructure);
		GPIO_SetBits(KEYBOARD_ROW_Port[count], KEYBOARD_ROW_Pin[count]);
	}
}

void KEYBOARD_Set_COL_Output_Low(void)//做为输出口，输出低电平
{
	uint8_t count;
	GPIO_InitTypeDef  GPIO_InitStructure;
	uint8_t keyboard_Col_length = sizeof(KEYBOARD_COL_Pin) / sizeof(*KEYBOARD_COL_Pin);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	for(count = 0;count < keyboard_Col_length;count++)
	{
		GPIO_InitStructure.GPIO_Pin = KEYBOARD_COL_Pin[count];
		GPIO_Init(KEYBOARD_COL_Port[count], &GPIO_InitStructure);
		GPIO_ResetBits(KEYBOARD_COL_Port[count], KEYBOARD_COL_Pin[count]);
	}
}

void KEYBOARD_Set_COL_Input_IPU(void)//做为输出口，输出低电平
{
	uint8_t count;
    uint8_t keyboard_Col_length = sizeof(KEYBOARD_COL_Pin) / sizeof(*KEYBOARD_COL_Pin);
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	for(count = 0;count < keyboard_Col_length;count++)
	{
		GPIO_InitStructure.GPIO_Pin = KEYBOARD_COL_Pin[count];
		GPIO_Init(KEYBOARD_COL_Port[count], &GPIO_InitStructure);
		GPIO_SetBits(KEYBOARD_COL_Port[count], KEYBOARD_COL_Pin[count]);
	}
}

//LED IO初始化
void KEYBOARD_Init(void)
{
    RCC_APB2PeriphClockCmd(KEYBOARD_RCC_Periph, ENABLE);	 //使能端口时钟
	KEYBOARD_Set_ROW_Output_Low();
	KEYBOARD_Set_COL_Input_IPU();
}

uint8_t KEYBOARD_Read_ROW(void)
{
	uint8_t Keyboard_Row_Value = 0;
    uint8_t keyboard_Row_length = sizeof(KEYBOARD_ROW_Pin) / sizeof(*KEYBOARD_ROW_Pin);
	uint8_t count;
	KEYBOARD_Set_COL_Output_Low();
	KEYBOARD_Set_ROW_Input_IPU();
	for(count = 0;count < keyboard_Row_length;count++)
	{
		if(GPIO_ReadInputDataBit(KEYBOARD_ROW_Port[count], KEYBOARD_ROW_Pin[count]))
		{
			Keyboard_Row_Value |= 0x01 << count;
		}
	}
	return Keyboard_Row_Value;//0000MSB111LSB  低位在右  低几位就表示按键顺序的第几位
}

uint8_t KEYBOARD_Read_COL(void)
{
	uint8_t Keyboard_Col_Value = 0;
    uint8_t keyboard_Col_length = sizeof(KEYBOARD_COL_Pin) / sizeof(*KEYBOARD_COL_Pin);
	uint8_t count;
	KEYBOARD_Set_ROW_Output_Low();
	KEYBOARD_Set_COL_Input_IPU();
	for(count = 0;count < keyboard_Col_length;count++)
	{
		if(GPIO_ReadInputDataBit(KEYBOARD_COL_Port[count], KEYBOARD_COL_Pin[count]))
		{
			Keyboard_Col_Value |= 0x01 << count;
		}
	}
	return Keyboard_Col_Value;//0000MSB111LSB  低位在右  低几位就表示按键顺序的第几位
}


uint16_t KEYBOARD_Get_Button_Queue_Size(void)
{
	return Uint8FIFOGetQueueSize(KEYBOARD_Button_QueueHandle);
}

void KEYBOARD_Push_Button_IntoQueue(uint8_t Keyboard_Button_Index)
{
    Uint8FIFOPush(KEYBOARD_Button_QueueHandle, Keyboard_Button_Index);
}

uint8_t KEYBOARD_Get_Top_Button(void)
{
	if(KEYBOARD_Get_Button_Queue_Size > 0)
	{
		return Uint8FIFOPop(KEYBOARD_Button_QueueHandle);
	}
	else
	{
		return NULL;
	}
}

uint8_t KEYBOARD_Read_Button(void)
{
	uint8_t Keyboard_Row_Value;
	uint8_t Keyboard_Col_Value;
	uint8_t Keyboard_Button_Index = 0; //从1开始到row * col为止的编号
    uint8_t keyboard_Row_length = sizeof(KEYBOARD_ROW_Pin) / sizeof(*KEYBOARD_ROW_Pin);
    uint8_t keyboard_Col_length = sizeof(KEYBOARD_COL_Pin) / sizeof(*KEYBOARD_COL_Pin);
	uint8_t count;
	Keyboard_Row_Value = KEYBOARD_Read_ROW();
	if(Keyboard_Row_Value == 0x00)return NULL;
	Keyboard_Col_Value = KEYBOARD_Read_COL();
	for(count = 0;count < keyboard_Row_length;count++)
	{
		if(!(Keyboard_Row_Value & (0x01 << count)))break;
	}
	if(count == keyboard_Row_length)return 0;
	Keyboard_Button_Index += count * keyboard_Col_length;
    
	for(count = 0;count < keyboard_Col_length;count++)
	{
		if(!(Keyboard_Col_Value & (0x01 << count)))break;
	}
	if(count == keyboard_Col_length)return 0;
	Keyboard_Button_Index += count + 1;
	if(Keyboard_Button_Index == 0)return 0;
	return Keyboard_Button_Index;
}

void KEYBOARD_Scan_Catch(void)//按键扫描典型代码，用操作系统时需替换延时函数
{
	static uint8_t Keyboard_Button_Index = 0;
	Keyboard_Button_Index = KEYBOARD_Read_Button();
	if(Keyboard_Button_Index == 0)
	{
        KEYBOARD_Delay_ms(20);
		return;
	}
	KEYBOARD_Delay_ms(20);
	if(KEYBOARD_Read_Button() == Keyboard_Button_Index)
	{
		KEYBOARD_Push_Button_IntoQueue(Keyboard_Button_Index);
		Keyboard_Button_Index = 0;
	}
    while(KEYBOARD_Read_Button() != 0)
    {
        KEYBOARD_Delay_ms(50);//放开按键
    }
}






