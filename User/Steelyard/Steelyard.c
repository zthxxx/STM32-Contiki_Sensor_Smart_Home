#include "Steelyard.h"
PROCESS(Steelyard_Load_Key_process, "Load key and push to input steam");

float Steelyard_CurrentlyWeight = 0.0;
float Steelyard_Currently_adjustWeight = 0.0;
float Steelyard_Currently_adjustWeights[20] = {0};
uint8_t Steelyard_adjustWeight_count = 0;
float Steelyard_UnitPrice = 1.0;
static float Steelyard_UnitPrice_Temp = 0.0;
float Steelyard_CurrentlyPrice = 0.0;
float Steelyard_TotalPrice = 0.0; 
static uint8_t cursor_position_row = 0;
static uint8_t cursor_position_col = 0;
static uint8_t cursor_point_deep = 0;

Uint8LIFOQueue Steelyard_Mode_LIFO_Queue = {NULL, NULL, 0};
Uint8LIFOQueue* Steelyard_Mode_LIFO_Queue_Handle = &Steelyard_Mode_LIFO_Queue;

bool Steelyard_Is_Decimal       = false;    //进位，小数点
bool Steelyard_Is_Set_UnitPrice = false;    //设置单价
bool Steelyard_Is_Accumulation  = false;    //累加
bool Steelyard_Is_Adjust_Coefficient = false;//手动校准
bool Steelyard_Is_Inputting     = false;    //正在输入

uint8_t Steelyard_Display_Row_Head_Length[] = {7,6,5,6};
uint8_t Steelyard_Display_Row_Endding_Length[] = {1,1,4,1};
bool* Steelyard_Signs[] = {&Steelyard_Is_Decimal, &Steelyard_Is_Set_UnitPrice, &Steelyard_Is_Accumulation, &Steelyard_Is_Adjust_Coefficient, &Steelyard_Is_Inputting};

uint8_t Steelyard_Keyboard_Key_Mapping[] = {NULL,
	VK_1, VK_2, VK_3, VK_CLEAR,	                VK_Steelyard_Accumulate,
	VK_4, VK_5, VK_6, VK_Steelyard_UnitPrice,	VK_Steelyard_Convert_Unit,
	VK_7, VK_8, VK_9, VK_Steelyard_Peeling,	    VK_Steelyard_Adjust_Coefficient,
	VK_0, VK_DECIMAL, VK_RETURN,	            VK_Steelyard_Adjust_Zero,	NULL
};

uint8_t Steelyard_Keyboard_Key_Type_Mapping[] = {NULL,
	Steelyard_Value_Key, Steelyard_Value_Key,   Steelyard_Value_Key,    Steelyard_Control_Key,  Steelyard_Mode_Key,
	Steelyard_Value_Key, Steelyard_Value_Key,   Steelyard_Value_Key,    Steelyard_Mode_Key,     Steelyard_Control_Key,
	Steelyard_Value_Key, Steelyard_Value_Key,   Steelyard_Value_Key,    Steelyard_Control_Key,  Steelyard_Mode_Key,
	Steelyard_Value_Key, Steelyard_Mode_Key,    Steelyard_Control_Key,  Steelyard_Control_Key,  NULL
};

Steelyard_Key_Process Steelyard_Key_Dispose_Method[] = {
    (Steelyard_Key_Process)(void*)NULL,
    Steelyard_Dispose_ValueKey,
    Steelyard_Dispose_Mode_Key,
    Steelyard_Dispose_Control_Key
};




uint8_t Steelyard_Get_MapVirtualKey(uint8_t key_index)
//得到虚拟按键映射
{
	if(key_index >= sizeof(Steelyard_Keyboard_Key_Mapping)/sizeof(*Steelyard_Keyboard_Key_Mapping))return NULL;
	return Steelyard_Keyboard_Key_Mapping[key_index];
}

uint8_t Steelyard_Get_Key_Type(uint8_t key_index)
//得到按键类型映射
{
	if(key_index >= sizeof(Steelyard_Keyboard_Key_Type_Mapping)/sizeof(*Steelyard_Keyboard_Key_Type_Mapping))return NULL;
	return Steelyard_Keyboard_Key_Type_Mapping[key_index];
}


void Steelyard_Display_Char(uint8_t row_x, uint8_t col_y, uint8_t virtual_Key)
//通过光标位置显示一个字符键
{
    OLED_ShowAlphabet(row_x, col_y, virtual_Key);
    OLED_Refresh_Gram();
}

void Steelyard_Show_Key_Char(uint8_t virtual_Key)
//判断是否为字符键并显示
{
    Steelyard_Display_Char(cursor_position_row, cursor_position_col, virtual_Key);
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

void Steelyard_Push_Mode(uint8_t mode_offset)
//设置模式并把标志并压入堆栈
{
    if(*(Steelyard_Signs[mode_offset]) == false)
    {
        *(Steelyard_Signs[mode_offset]) = true;
        Uint8LIFOPush(Steelyard_Mode_LIFO_Queue_Handle, mode_offset);
    }
}

void Steelyard_Pop_Mode(void)
{
    if(Uint8LIFOGetQueueSize(Steelyard_Mode_LIFO_Queue_Handle) == 0)
    {
        return;
    }
    *(Steelyard_Signs[Uint8LIFOPop(Steelyard_Mode_LIFO_Queue_Handle)]) = false;
}



float Steelyard_Get_CurrentlyPrice(void)
//获得并设置当前价格
{
    Steelyard_CurrentlyPrice = Steelyard_CurrentlyWeight * Steelyard_UnitPrice;
    Steelyard_CurrentlyPrice = Steelyard_CurrentlyPrice < 0 ? 0 : Steelyard_CurrentlyPrice;
    return Steelyard_CurrentlyPrice;
}

void Steelyard_Adjust_Coefficient(void)
{
    IWDG_Feed();
    Steelyard_adjustWeight_count = 0;
}

uint8_t Steelyard_Get_ValueKey_Value(uint8_t virtual_Key)
//得到数值键对应的数值
{
    return virtual_Key - VK_0;
}

void Steelyard_Dispose_ValueKey(uint8_t virtual_Key)
//处理数值键
{
    uint8_t mode_LIFO_Top;
    uint8_t key_value = Steelyard_Get_ValueKey_Value(virtual_Key);
    if(Uint8LIFOGetQueueSize(Steelyard_Mode_LIFO_Queue_Handle) == 0)
    {
        return;
    }
    mode_LIFO_Top = Uint8LIFO_Query_StackTop(Steelyard_Mode_LIFO_Queue_Handle);
    switch(mode_LIFO_Top)
    {
        case Steelyard_Set_UnitPrice_Sign:
            Steelyard_UnitPrice_Temp = Steelyard_UnitPrice_Temp * 10 + key_value;
        break;
        
        case Steelyard_Decimal_Sign:
        {
            if(Steelyard_Is_Set_UnitPrice)
            {
                Steelyard_UnitPrice_Temp = Steelyard_UnitPrice_Temp + ((float)key_value / pow(10, ++cursor_point_deep));
            }
            else if(Steelyard_Is_Adjust_Coefficient)
            {
                Steelyard_Currently_adjustWeight = Steelyard_Currently_adjustWeight + ((float)key_value / pow(10, ++cursor_point_deep));
            }
        }
        break;
        
        case Steelyard_Accumulation_Sign:
            return;
        break;
        
        case Steelyard_Adjust_Coefficient_Sign:
            Steelyard_Currently_adjustWeight = Steelyard_Currently_adjustWeight * 10 + key_value;
        break;
    }
    Steelyard_Show_Key_Char(virtual_Key);
    Steelyard_Is_Inputting = true;
}

void Steelyard_Dispose_Control_Key(uint8_t virtual_Key)
{
//    uint8_t control_offset = virtual_Key - VK_CLEAR;
    uint8_t mode_LIFO_Top;
    if(Uint8LIFOGetQueueSize(Steelyard_Mode_LIFO_Queue_Handle) == 0)
    {
        if((virtual_Key == VK_CLEAR) || (virtual_Key == VK_RETURN))return;
    }
    else
    {
        mode_LIFO_Top = Uint8LIFO_Query_StackTop(Steelyard_Mode_LIFO_Queue_Handle);
        switch(mode_LIFO_Top)
        {
            case Steelyard_Set_UnitPrice_Sign:
            case Steelyard_Decimal_Sign:
            case Steelyard_Adjust_Coefficient_Sign:
                if((virtual_Key != VK_CLEAR) && (virtual_Key != VK_RETURN))return;
            break;
        }
    }
    switch(virtual_Key)
    {
        case VK_RETURN:
        {
            switch(mode_LIFO_Top)
            {
                case Steelyard_Set_UnitPrice_Sign:
                {
                    Steelyard_UnitPrice = Steelyard_UnitPrice_Temp;
                    Steelyard_UnitPrice_Temp = 0;
                    Steelyard_Pop_Mode();
                }
                break;
                
                case Steelyard_Decimal_Sign:
                {
                    Steelyard_Pop_Mode();
                    Steelyard_Dispose_Control_Key(virtual_Key);
                }
                break;
                
                case Steelyard_Adjust_Coefficient_Sign://此部分代码重复
                {
                    if(Steelyard_Is_Inputting == true)
                    {
                        Steelyard_Currently_adjustWeights[Steelyard_adjustWeight_count++] = Steelyard_Currently_adjustWeight;
                        cursor_position_row = 0;
                        cursor_position_col = Steelyard_Display_Row_Head_Length[cursor_position_row];
                        cursor_point_deep = 0;
                        OLED_Fill_Alphabet(cursor_position_row, cursor_position_col, OLED_Max_Col - cursor_position_col - Steelyard_Display_Row_Endding_Length[cursor_position_row]);
                    }
                    if((Steelyard_Is_Inputting == false) || (Steelyard_adjustWeight_count >= (sizeof(Steelyard_Currently_adjustWeights) / sizeof(*Steelyard_Currently_adjustWeights))))
                    {
                        Steelyard_Adjust_Coefficient();
                        Steelyard_Pop_Mode();
                    }
                    Steelyard_Is_Inputting = false;
                    Steelyard_Currently_adjustWeight = 0;
                }
                break;
                
                case Steelyard_Accumulation_Sign://此部分代码重复
                {
                    char num_string[16];
                    uint8_t count;
                    static uint8_t last_length = 16;
                    Steelyard_TotalPrice += Steelyard_Get_CurrentlyPrice();
                    sprintf(num_string,"%.1f",Steelyard_TotalPrice);
                    OLED_ShowAlphabets(3,7,(uint8_t*)num_string); 
                    count = strlen(num_string) + 7;
                    if(count < last_length)
                    {
                        OLED_Fill_Alphabet(3,count,15-count);
                    }
                    last_length = count;
                    OLED_Refresh_Gram();
                }
                break;
            }
        }
        break;
        
        case VK_CLEAR:
        {
            switch(mode_LIFO_Top)
            {
                case Steelyard_Set_UnitPrice_Sign://此部分代码重复
                {
                    Steelyard_UnitPrice_Temp = 0;
                    if(Steelyard_Is_Inputting == true)
                    {
                        Steelyard_UnitPrice_Temp = 0;
                        cursor_position_row = 2;
                        cursor_position_col = Steelyard_Display_Row_Head_Length[cursor_position_row];
                        cursor_point_deep = 0;
                        OLED_Fill_Alphabet(cursor_position_row, cursor_position_col, OLED_Max_Col - cursor_position_col - Steelyard_Display_Row_Endding_Length[cursor_position_row]);
                        Steelyard_Is_Inputting = false;
                    }
                    else
                    {
                        Steelyard_adjustWeight_count = 0;
                        Steelyard_Pop_Mode();
                    }
                }
                break;
                
                case Steelyard_Decimal_Sign://此部分代码重复
                {
                    Steelyard_Pop_Mode();
                    Steelyard_Dispose_Control_Key(virtual_Key);
                }
                break;
                
                case Steelyard_Adjust_Coefficient_Sign://此部分代码重复
                {
                    if(Steelyard_Is_Inputting == true)
                    {
                        Steelyard_Currently_adjustWeight = 0;
                        cursor_position_row = 0;
                        cursor_position_col = Steelyard_Display_Row_Head_Length[cursor_position_row];
                        cursor_point_deep = 0;
                        OLED_Fill_Alphabet(cursor_position_row, cursor_position_col, OLED_Max_Col - cursor_position_col - Steelyard_Display_Row_Endding_Length[cursor_position_row]);
                        Steelyard_Is_Inputting = false;
                    }
                    else
                    {
                        Steelyard_Pop_Mode();
                    }
                }
                break;
                
                case Steelyard_Accumulation_Sign:
                {
                    Steelyard_TotalPrice = 0;
                    cursor_position_row = 3;
                    cursor_position_col = 0;
                    cursor_point_deep = 0;
                    OLED_Fill_Alphabet(cursor_position_row, cursor_position_col, OLED_Max_Col);
                    Steelyard_Is_Inputting = false;
                    Steelyard_Pop_Mode();
                }
                break;
            }
        }
        break;
        
        case VK_Steelyard_Peeling:
        case VK_Steelyard_Adjust_Zero:
        {
            IWDG_Feed();
            printf("key adjust 0\r\n");
            HX711_Zero_Offset_Adjust();
            IWDG_Feed();
        }
        break;
    }
}

void Steelyard_Dispose_Mode_Key(uint8_t virtual_Key)
//处理模式按键
{
    uint8_t mode_offset = virtual_Key - VK_DECIMAL;
    uint8_t mode_LIFO_Top;
    if(Uint8LIFOGetQueueSize(Steelyard_Mode_LIFO_Queue_Handle) == 0)
    {
        if(mode_offset == Steelyard_Decimal_Sign)return;
    }
    else
    {
        mode_LIFO_Top = Uint8LIFO_Query_StackTop(Steelyard_Mode_LIFO_Queue_Handle);
        //根据栈顶做过滤
        switch(mode_LIFO_Top)
        {
            case Steelyard_Set_UnitPrice_Sign:
                if(mode_offset != Steelyard_Decimal_Sign)return;
            break;
            
            case Steelyard_Decimal_Sign:
                return;
            break;
            
            case Steelyard_Accumulation_Sign:
                if(mode_offset != Steelyard_Set_UnitPrice_Sign)return;
            break;
            
            case Steelyard_Adjust_Coefficient_Sign:
                if(mode_offset != Steelyard_Decimal_Sign)return;
            break;
        }
    }
    switch(mode_offset)
    {
        case Steelyard_Set_UnitPrice_Sign:
        {
            Steelyard_UnitPrice_Temp = 0;
            cursor_position_row = 2;
            cursor_position_col = Steelyard_Display_Row_Head_Length[cursor_position_row];
            cursor_point_deep = 0;
            OLED_Fill_Alphabet(cursor_position_row, cursor_position_col, OLED_Max_Col - cursor_position_col - Steelyard_Display_Row_Endding_Length[cursor_position_row]);
            Steelyard_Is_Inputting = false;
        }
        break;
        
        case Steelyard_Decimal_Sign:
        {
            Steelyard_Show_Key_Char('.');
        }
        break;
        
        case Steelyard_Accumulation_Sign:
        {
            Steelyard_TotalPrice = 0;
            cursor_position_row = 3;
            cursor_position_col = Steelyard_Display_Row_Head_Length[cursor_position_row];
            cursor_point_deep = 0;
            OLED_Fill_Alphabet(cursor_position_row, cursor_position_col, OLED_Max_Col - cursor_position_col - Steelyard_Display_Row_Endding_Length[cursor_position_row]);
            Steelyard_Is_Inputting = false;
            OLED_ShowAlphabets(3,0,"Total:");
            OLED_ShowAlphabets(3,15,"Y");
        }
        break;
        
        case Steelyard_Adjust_Coefficient_Sign:
        {
            Steelyard_Currently_adjustWeight = 0;
            cursor_position_row = 0;
            cursor_position_col = Steelyard_Display_Row_Head_Length[cursor_position_row];
            cursor_point_deep = 0;
            OLED_Fill_Alphabet(cursor_position_row, cursor_position_col, OLED_Max_Col - cursor_position_col - Steelyard_Display_Row_Endding_Length[cursor_position_row]);
            Steelyard_Is_Inputting = false;
        }
        break;
    }
    Steelyard_Push_Mode(mode_offset);
}

//按键处理进程
PROCESS_THREAD(Steelyard_Load_Key_process, ev, data)
{
    uint8_t key_index = NULL; 
    PROCESS_BEGIN();
    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == keyboard_press_event);
        while(KEYBOARD_Get_Button_Queue_Size() > 0)
        {
            key_index = KEYBOARD_Get_Top_Button();
            if((key_index == NULL) || (Steelyard_Get_MapVirtualKey(key_index) == NULL))continue;
            Steelyard_Key_Dispose_Method[Steelyard_Get_Key_Type(key_index)](Steelyard_Get_MapVirtualKey(key_index));
        }
    }
    PROCESS_END();
}



















