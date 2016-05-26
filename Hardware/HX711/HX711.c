#include "HX711.h"

double HX711_fitting_coefficient[] = {-0.1342132, 22076.88160693};//ax^2+bx+c     {MSB, ..., LSB}
	    
//LED IO初始化
void HX711_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(HX711_RCC_Periph, ENABLE);	 //使能端口时钟

	GPIO_InitStructure.GPIO_Pin = HX711_DAT_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HX711_DAT_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = HX711_SCK_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HX711_SCK_Port, &GPIO_InitStructure);
    HX711_Zero_Offset_Adjust();
}

void HX711_SCK_High()
{
	GPIO_SetBits(HX711_SCK_Port,HX711_SCK_Pin);
}

void HX711_SCK_Low()
{
	GPIO_ResetBits(HX711_SCK_Port,HX711_SCK_Pin);
}

void HX711_DAT_High()
{
	GPIO_SetBits(HX711_DAT_Port,HX711_DAT_Pin);
}

uint8_t HX711_Read_DAT_Pin()
{
	return GPIO_ReadInputDataBit(HX711_DAT_Port,HX711_DAT_Pin);
}

void HX711_SCK_Pulse()
{
	HX711_SCK_High();
	HX711_Delay_us(2);
	HX711_SCK_Low();
}

uint8_t HX711_Get_DAT_Pin_State()
{
    return HX711_Read_DAT_Pin();
}

uint32_t HX711_Read_Value(void)
{
	uint32_t HX711_Value = 0;
	uint8_t count = 0;
	uint8_t HX711_Mode = HX711_CHANNEL_A_128;
	
	HX711_DAT_High();
	HX711_SCK_Low();
	while(HX711_Read_DAT_Pin());
	HX711_Delay_us(2);
	
	for(count = 1; count <= HX711_Default_Shift; count++)
	{
		HX711_SCK_Pulse();
		if(HX711_Read_DAT_Pin())
		{
			HX711_Value += 1;
		}
		if(count < HX711_Default_Shift)
		{
			HX711_Value <<= 1;
		}
		HX711_Delay_us(2);
	}
	for(count = 0; count < HX711_Mode - HX711_Default_Shift; count++)
	{
		HX711_SCK_Pulse();
		HX711_Delay_us(2);
    }
	return HX711_Value;
}

double HX711_Read_Weight(void)
{
    double weight = 0.0;
    weight = HX711_fitting_coefficient[0] * ((double)HX711_Read_Value()/100.0) + HX711_fitting_coefficient[1];
    return weight;
}

void HX711_Zero_Offset_Adjust()
{
    uint8_t count;
    uint8_t sample_times = 20;
    double weigth = 0.0;
    
    for(count = 0;count < 5; count++)
    {
        HX711_Read_Weight();  //固定丢弃前面几次
    }
    
    for(count = 0;count < sample_times; count++)
    {
        weigth += HX711_Read_Weight();
    }
    weigth /= sample_times;
    HX711_fitting_coefficient[1] -= weigth;
}

double HX711_Window_Filter()
{
    static bool HX711_is_init = false;
    uint8_t HX711_Slip_Window = 10;
    static double HX711_Last_Weight_List[10] = {0.0};
    static uint8_t HX711_value_index = 0;
    static double HX711_filte_value = 0.0;
    double HX711_Weight = 0.0;
    uint8_t count = 0;
    
    if(HX711_is_init == false)
    {
        for(count = 0;count < HX711_Slip_Window; count++)
        {
            HX711_Weight = HX711_Read_Weight();
            HX711_filte_value += HX711_Weight;
            HX711_Last_Weight_List[count] = HX711_Weight;
        }
        HX711_filte_value /= (float)HX711_Slip_Window;
        if(fabs(HX711_filte_value) > 1)HX711_filte_value = 0;
        HX711_is_init = true;
    }
    
    
    HX711_Weight = HX711_Read_Weight();
    HX711_filte_value += (HX711_Weight - HX711_Last_Weight_List[HX711_value_index]) / (float)HX711_Slip_Window;
    HX711_Last_Weight_List[HX711_value_index++] = HX711_filte_value;
    HX711_value_index = HX711_value_index < HX711_Slip_Window ? HX711_value_index : 0;
    
    return HX711_filte_value;
}




