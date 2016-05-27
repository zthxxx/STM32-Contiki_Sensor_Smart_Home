#include "Steelyard.h"

float Steelyard_CurrentlyWeight = 0.0;
float Steelyard_UnitPrice = 0.0;
float Steelyard_CurrentlyPrice = 0.0;
float Steelyard_TotalPrice = 0.0;
uint8_t cursor_position_row = 0;
uint8_t cursor_position_col = 0;

uint8_t Steelyard_Keyboard_Key_Mapping[] = {NULL,
	VK_1, VK_2, VK_3, VK_CLEAR,	VK_Steelyard_Accumulate,
	VK_4, VK_5, VK_6, VK_Steelyard_UnitPrice,	VK_Steelyard_Convert_Unit,
	VK_7, VK_8, VK_9, VK_Steelyard_Peeling,	VK_Steelyard_Adjust_Coefficient,
	VK_0, VK_DECIMAL, VK_RETURN,	VK_Steelyard_Adjust_Zero,	NULL
};

typedef void(*Steelyard_Process_Key)(uint8_t);

uint8_t Steelyard_Get_MapVirtualKey(uint8_t key_index)
{
	if(key_index >= sizeof(Steelyard_Keyboard_Key_Mapping)/sizeof(*Steelyard_Keyboard_Key_Mapping))return NULL;
	return Steelyard_Keyboard_Key_Mapping[key_index];
}

void Steelyard_Display_Number(uint8_t row_x, uint8_t col_y, uint8_t key_index)
{
	uint8_t virtual_Key = Steelyard_Get_MapVirtualKey(key_index);
	if((virtual_Key >= VK_0) && (virtual_Key <= VK_NUMPAD9))
	{
		OLED_ShowAlphabet(row_x, col_y, virtual_Key);
	}
}

void Steelyard_Process_Number_Key(uint8_t key_index)
{
	if((Steelyard_Get_MapVirtualKey(key_index) >= VK_0) && (Steelyard_Get_MapVirtualKey(key_index) <= VK_NUMPAD9))
	{
		Steelyard_Display_Number(cursor_position_row, cursor_position_col, key_index);
		cursor_position_col++;
		if(cursor_position_col >= OLED_Max_Col)
		{
			cursor_position_col = 0;
			cursor_position_row++;
			if(cursor_position_row >= OLED_Max_Row)
			{
				cursor_position_row = 0;
			}
		}
	}
}

void Steelyard_Process_Load_Keys(void)
{
	while(KEYBOARD_Get_Button_Queue_Size() > 0)
	{
		uint8_t key_index = 0;
		key_index = KEYBOARD_Get_Top_Button();
		if(key_index == NULL)continue;
	}
}





















