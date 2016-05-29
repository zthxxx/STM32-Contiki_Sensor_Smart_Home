#include "HX711.h"

double HX711_fitting_coefficient[] = {-0.096456, -0.319117195016503};//ax^2+bx+c     {MSB, ..., LSB}
	    
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

    HX711_Load_Adjust_Conefficient();
    HX711_Zero_Offset_Adjust();
    HX711_Save_Adjust_Coefficient();
    
    HX711_Window_Filter();
    HX711_Window_Weighting_Filter();
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

uint32_t HX711_Read_Average_Value(void)
{
    uint8_t count;
    uint8_t sample_times = 30;
    uint32_t value = 0;
    for(count = 0;count < 10; count++)
    {
        HX711_Read_Value();  //固定丢弃前面几次
    }
    for(count = 0;count < sample_times; count++)
    {
        value += HX711_Read_Value() / sample_times;
    }
    return value;
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
    uint8_t sample_times = 40;
    double weigth = 0.0;
    for(count = 0;count < 15; count++)
    {
        HX711_Read_Weight();  //固定丢弃前面几次
    }
    for(count = 0;count < sample_times; count++)
    {
        weigth += HX711_Read_Weight();
    }
    weigth /= sample_times;
    HX711_fitting_coefficient[1] -= weigth;
    printf("deviation: %f\r\n",weigth);
}

void HX711_Save_Adjust_Coefficient(void)
{
    STMFLASH_Write(STM32_FLASH_HX711_Coefficient_Page,(uint16_t*)HX711_fitting_coefficient,sizeof(HX711_fitting_coefficient) / sizeof(uint16_t));
}

void HX711_Load_Adjust_Conefficient(void)
{
    STMFLASH_Read(STM32_FLASH_HX711_Coefficient_Page,(uint16_t*)HX711_fitting_coefficient,sizeof(HX711_fitting_coefficient) / sizeof(uint16_t));
}

double HX711_Window_Filter()
{
    static bool HX711_Window_is_init = false;
    uint8_t count = 0;
    uint8_t HX711_Slip_Window_Length = 80;
    static double HX711_Last_Weight_List[80] = {0.0};
    static uint8_t HX711_value_index = 0;
    static double HX711_filte_value = 0.0;
    double HX711_Weight = 0.0;
    
    
    if(HX711_Window_is_init == false)
    {
        for(count = 0;count < HX711_Slip_Window_Length; count++)
        {
            HX711_Weight = HX711_Read_Weight();
            HX711_filte_value += HX711_Weight;
            HX711_Last_Weight_List[count] = HX711_Weight;
        }
        HX711_filte_value /= (float)HX711_Slip_Window_Length;
        if(fabs(HX711_filte_value) > 1)HX711_filte_value = 0;
        HX711_Window_is_init = true;
    }
    
    
    HX711_Weight = HX711_Read_Weight();
    HX711_filte_value += (HX711_Weight - HX711_Last_Weight_List[HX711_value_index]) / (float)HX711_Slip_Window_Length;
    HX711_Last_Weight_List[HX711_value_index++] = HX711_Weight;
    HX711_value_index = HX711_value_index < HX711_Slip_Window_Length ? HX711_value_index : 0;
    
    return HX711_filte_value;
}

double HX711_Window_Weighting_Filter()
{
    static bool HX711_Window_Weighting_is_init = false;
    uint8_t count = 0;
    uint8_t HX711_Slip_Window_Length = 30;
    static double HX711_Last_Weight = 0.0;
    static double HX711_Last_Weight_List[30] = {0.0};
    static uint8_t HX711_value_index = 0;
    static double HX711_filte_value = 0.0;
    double HX711_Weight = 0.0;
    float  HX711_Value_Trust = 0.3;
    
    
    if(HX711_Window_Weighting_is_init == false)
    {
        HX711_Last_Weight = HX711_Window_Filter();
        for(count = 0;count < HX711_Slip_Window_Length; count++)
        {
            HX711_Weight = HX711_Window_Filter();
            HX711_Weight = HX711_Weight * HX711_Value_Trust + HX711_Last_Weight * (1 - HX711_Value_Trust);
            HX711_Last_Weight = HX711_Weight;
            HX711_filte_value += HX711_Weight;
            HX711_Last_Weight_List[count] = HX711_Weight;
            
        }
        HX711_filte_value /= (float)HX711_Slip_Window_Length;
        HX711_Window_Weighting_is_init = true;
    }
    
    
    HX711_Weight = HX711_Window_Filter();
    HX711_Weight = HX711_Weight * HX711_Value_Trust + HX711_Last_Weight * (1 - HX711_Value_Trust);
    HX711_Last_Weight = HX711_Weight;
    HX711_filte_value += (HX711_Weight - HX711_Last_Weight_List[HX711_value_index]) / (float)HX711_Slip_Window_Length;
    HX711_Last_Weight_List[HX711_value_index++] = HX711_Weight;
    HX711_value_index = HX711_value_index < HX711_Slip_Window_Length ? HX711_value_index : 0;
    return HX711_filte_value;
}

// 求线性回归方程：Y = ax + b
// dada[rows*2]数组：X, Y；rows：数据行数；a, b：返回回归系数
// SquarePoor[4]：返回方差分析指标: 回归平方和，剩余平方和，回归平方差，剩余平方差
// 返回值：1求解成功，0错误
int LinearRegression(double *data, int rows, double *a, double *b, double *SquarePoor)
{
    int m;
    double *p, Lxx = 0.0, Lxy = 0.0, xa = 0.0, ya = 0.0;
    if (data == 0 || a == 0 || b == 0 || rows < 1)
        return 0;
    for (p = data, m = 0; m < rows; m ++)
    {
        xa += *p ++;
        ya += *p ++;
    }
    xa /= rows;                                     // X平均值
    ya /= rows;                                     // Y平均值
    for (p = data, m = 0; m < rows; m ++, p += 2)
    {
        Lxx += ((*p - xa) * (*p - xa));             // Lxx = Sum((X - Xa)平方)
        Lxy += ((*p - xa) * (*(p + 1) - ya));       // Lxy = Sum((X - Xa)(Y - Ya))
    }
    *a = Lxy / Lxx;                                 // b = Lxy / Lxx
    *b = ya - *a * xa;                              // a = Ya - b*Xa
    if (SquarePoor == 0)
        return 1;
    // 方差分析
    SquarePoor[0] = SquarePoor[1] = 0.0;
    for (p = data, m = 0; m < rows; m ++, p ++)
    {
        Lxy = *b + *a * *p ++;
        SquarePoor[0] += ((Lxy - ya) * (Lxy - ya)); // U(回归平方和)
        SquarePoor[1] += ((*p - Lxy) * (*p - Lxy)); // Q(剩余平方和)
    }
    SquarePoor[2] = SquarePoor[0];                  // 回归方差
    SquarePoor[3] = SquarePoor[1] / (rows - 2);     // 剩余方差
    return 1;
}

