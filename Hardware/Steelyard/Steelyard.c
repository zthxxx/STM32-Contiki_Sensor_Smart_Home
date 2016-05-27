#include "Steelyard.h"

float Steelyard_CurrentlyWeight = 0.0;
float Steelyard_UnitPrice = 0.0;
float Steelyard_CurrentlyPrice = 0.0;
float Steelyard_TotalPrice = 0.0;

uint8_t Keyboard_Key_Mapping[] = [NULL,
	VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_CLEAR,	VK_Steelyard_Accumulate,
	VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_Steelyard_UnitPrice,	VK_Steelyard_Convert_Unit,
	VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9,	VK_Steelyard_Peeling,	VK_Steelyard_Adjust_Coefficient
	VK_NUMPAD0, VK_DECIMAL, VK_RETURN,	VK_Steelyard_Adjust_Zero,	NULL
]

void Steelyard_Display_Number(uint8_t key)
{
	
}



