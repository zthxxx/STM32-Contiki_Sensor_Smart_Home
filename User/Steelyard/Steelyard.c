#include "Steelyard.h"
PROCESS(Steelyard_Load_Key_process, "Load key and push to input steam");

float Steelyard_CurrentlyWeight = 0.0;
float Steelyard_Currently_adjustWeight = 0.0;
double Steelyard_Currently_adjustWeights[20][2] = {0};
uint8_t Steelyard_adjustWeight_count = 0;
float Steelyard_UnitPrice = 0.0;
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

uint8_t Steelyard_Display_Row_Head_Length[] = {6,6,6,6};
uint8_t Steelyard_Display_Row_Endding_Length[] = {1,1,4,1};
bool* Steelyard_Signs[] = {&Steelyard_Is_Decimal, &Steelyard_Is_Set_UnitPrice, &Steelyard_Is_Accumulation, &Steelyard_Is_Adjust_Coefficient, &Steelyard_Is_Inputting};

uint8_t Steelyard_Keyboard_Key_Mapping[] = {NULL,
	VK_7, VK_8, VK_9, VK_CLEAR,	                VK_Steelyard_Accumulate,
	VK_4, VK_5, VK_6, VK_Steelyard_UnitPrice,	VK_Steelyard_Convert_Unit,
	VK_1, VK_2, VK_3, VK_Steelyard_Peeling,	    VK_Steelyard_Adjust_Coefficient,
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

void Steelyard_Display_Weight(void)
{
    uint8_t str[] = {' '+95,' '+96,' '+97,' '+98,':'};
    OLED_ShowAlphabets(Steelyard_Weight_Row,0,str);
    OLED_ShowAlphabets(Steelyard_Weight_Row,15,"g");
}

void Steelyard_Display_Price(void)
{
    uint8_t str[] = {' '+99,' '+100,' '+101,' '+102,':'};
    OLED_ShowAlphabets(Steelyard_Price_Row,0,str);
    OLED_ShowAlphabets(Steelyard_Price_Row,15,"Y");
}

void Steelyard_Display_UnitPrice(void)
{
    uint8_t str[] = {' '+103,' '+104,' '+99,' '+100,':'};
    OLED_ShowAlphabets(Steelyard_UnitPrice_Row,0,str);
    OLED_ShowAlphabets(Steelyard_UnitPrice_Row,12,"Y/Kg");
}

void Steelyard_Display_Total(void)
{
    uint8_t str[] = {' '+105,' '+106,' '+107,' '+108,':'};
    OLED_ShowAlphabets(Steelyard_Total_Row,0,str);
    OLED_ShowAlphabets(Steelyard_Total_Row,15,"Y");
}
void Steelyard_Display_AdjustWeight(void)
{
    uint8_t str[] = {' '+109,' '+110,' '+111,' '+112,':'};
    OLED_ShowAlphabets(Steelyard_Total_Row,0,str);
    OLED_ShowAlphabets(Steelyard_Total_Row,15,"g");
}

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

void Steelyard_Set_Cursor_Position(uint8_t row, uint8_t col)
//设置光标位置
{
    cursor_position_row = row;
    cursor_position_col = col;
}
void Steelyard_Cursor_Shift(void)
//自动往后移动光标
{
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

void Steelyard_Input_Clear(uint8_t row)
//清空某行输入，重置光标，重置小数点位，重置输入状态
{
    cursor_position_row = row;
    cursor_position_col = Steelyard_Display_Row_Head_Length[cursor_position_row];
    cursor_point_deep = 0;
    OLED_Fill_Alphabet(cursor_position_row, cursor_position_col, OLED_Max_Col - cursor_position_col - Steelyard_Display_Row_Endding_Length[cursor_position_row]);
    Steelyard_Is_Inputting = false;
}

void Steelyard_Display_Char(uint8_t row_x, uint8_t col_y, uint8_t virtual_Key)
//通过光标位置显示一个字符键
{
    OLED_ShowAlphabet(row_x, col_y, virtual_Key);
    OLED_Refresh_Gram();
}

void Steelyard_Show_Key_Char(uint8_t virtual_Key)
//通过虚拟键显示对应字符
{
    Steelyard_Display_Char(cursor_position_row, cursor_position_col, virtual_Key);
    Steelyard_Cursor_Shift();
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
//清除模式，标志出栈并置标志位为false
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
    double SquarePoor[4];
    IWDG_Feed();
    LinearRegression((double*)Steelyard_Currently_adjustWeights, Steelyard_adjustWeight_count, &HX711_fitting_coefficient[0], &HX711_fitting_coefficient[1],SquarePoor);
    printf("ax+b, %f %f\r\n",HX711_fitting_coefficient[0],HX711_fitting_coefficient[1]);
    HX711_Save_Adjust_Coefficient();
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
            float* Steelyard_Input_Operation_Number;
            Steelyard_Input_Operation_Number = Steelyard_Is_Set_UnitPrice ? &Steelyard_UnitPrice_Temp : &Steelyard_Currently_adjustWeight;            
            *Steelyard_Input_Operation_Number = *Steelyard_Input_Operation_Number + ((float)key_value / pow(10, ++cursor_point_deep));
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
                
                case Steelyard_Adjust_Coefficient_Sign:
                {
                    if(Steelyard_Is_Inputting == true)
                    {
                        IWDG_Feed();
                        Steelyard_Currently_adjustWeights[Steelyard_adjustWeight_count][0] = (double)HX711_Read_Average_Value()/100.0;
                        Steelyard_Currently_adjustWeights[Steelyard_adjustWeight_count++][1] = Steelyard_Currently_adjustWeight;
                    }
                    if((Steelyard_Is_Inputting == false) || (Steelyard_adjustWeight_count >= (sizeof(Steelyard_Currently_adjustWeights) / sizeof(*Steelyard_Currently_adjustWeights))))
                    {
                        Steelyard_Adjust_Coefficient();
                        Steelyard_Pop_Mode();
                        OLED_Fill_Alphabet(Steelyard_Adjust_Weight_Row,0,16);
                    }
                    Steelyard_Input_Clear(Steelyard_Adjust_Weight_Row);
                    Steelyard_Currently_adjustWeight = 0;
                }
                break;
                
                case Steelyard_Accumulation_Sign:
                {
                    static uint8_t last_length = 16;
                    Steelyard_TotalPrice += Steelyard_Get_CurrentlyPrice();
                    OLED_ShowFloat(Steelyard_Total_Row,Steelyard_Display_Row_Head_Length[Steelyard_Total_Row],15, Steelyard_TotalPrice, &last_length);
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
                case Steelyard_Set_UnitPrice_Sign:
                {
                    Steelyard_UnitPrice_Temp = 0;
                    if(Steelyard_Is_Inputting == true)
                    {
                        Steelyard_UnitPrice_Temp = 0;
                        Steelyard_Input_Clear(Steelyard_UnitPrice_Row);
                    }
                    else
                    {
                        Steelyard_adjustWeight_count = 0;
                        Steelyard_Pop_Mode();
                    }
                }
                break;
                
                case Steelyard_Decimal_Sign:
                {
                    Steelyard_Pop_Mode();
                    Steelyard_Dispose_Control_Key(virtual_Key);
                }
                break;
                
                case Steelyard_Adjust_Coefficient_Sign:
                {
                    if(Steelyard_Is_Inputting == true)
                    {
                        Steelyard_Currently_adjustWeight = 0;
                        Steelyard_Input_Clear(Steelyard_Adjust_Weight_Row);
                    }
                    else
                    {
                        Steelyard_Pop_Mode();
                        OLED_Fill_Alphabet(Steelyard_Adjust_Weight_Row,0,16);
                    }
                }
                break;
                
                case Steelyard_Accumulation_Sign:
                {
                    Steelyard_TotalPrice = 0;
                    Steelyard_Input_Clear(Steelyard_Total_Row);
                    OLED_Fill_Alphabet(Steelyard_Total_Row, 0, 16);
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
            Steelyard_Input_Clear(Steelyard_UnitPrice_Row);
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
            Steelyard_Input_Clear(Steelyard_Total_Row);
            Steelyard_Display_Total();
        }
        break;
        
        case Steelyard_Adjust_Coefficient_Sign:
        {
            Steelyard_Currently_adjustWeight = 0;
            Steelyard_Input_Clear(Steelyard_Adjust_Weight_Row);
            Steelyard_Display_AdjustWeight();
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



















