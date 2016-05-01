#include "timers.h"

uint32_t TM5_CH1_CAPTURE_STA = 0;//8位0x00~0x80
uint32_t TM5_CH1_CAPTURE_VAL;  //捕获高电平后定时器溢出的次数


//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//autoReload：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3
void Timer2_Init(u16 autoReload,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = autoReload; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM2, //TIM2
		TIM_IT_Update  |  //TIM 中断源
		TIM_IT_Trigger,   //TIM 触发中断源 
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器			 
}



void Timer5_Init(u16 autoReload,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
    
    TIM_TimeBaseStructure.TIM_Period = autoReload;              
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM5, & TIM_TimeBaseStructure);
    
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;     //通道1
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;   //上升沿捕获
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  //映射到TI1
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  //不分频
    TIM_ICInitStructure.TIM_ICFilter = 0x00;                //不滤波
    TIM_ICInit(TIM5, &TIM_ICInitStructure);    
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;                               
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
    NVIC_Init(&NVIC_InitStructure);    
    
    TIM_ITConfig(TIM5, TIM_IT_Update | TIM_IT_CC1, ENABLE );
    TIM_Cmd(TIM5, ENABLE);
}






void TIM2_IRQHandler(void)   //TIM2中断
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
    }
}


void TIM5_IRQHandler(void)
{
    if((TM5_CH1_CAPTURE_STA & 0x80) == 0 )  //未成功捕获，0x80捕获完成
    {
        if(TIM_GetITStatus(TIM5, TIM_IT_Update) == SET)  //数据更新中断产生
        {
            if(TM5_CH1_CAPTURE_STA & 0x40)     //已经捕获到高电平
            {
                if((TM5_CH1_CAPTURE_STA & 0x3f)==0x3f)   //-溢出
                {
                    TM5_CH1_CAPTURE_STA |= 0x80;  //强制捕获成功
                    TM5_CH1_CAPTURE_VAL = 0xffff;  //此时的计数值
                }
                else
                {
                    TM5_CH1_CAPTURE_STA++; 
                }
            
            }
        }
        if(TIM_GetITStatus(TIM5, TIM_IT_CC1) == SET)//发生捕获
        {
            if(TM5_CH1_CAPTURE_STA & 0x40)  //成功捕获到一次下降沿，但不是第一次捕获
            {
            TM5_CH1_CAPTURE_STA |= 0x80;  //捕获成功
            TM5_CH1_CAPTURE_VAL = TIM_GetCapture1(TIM5);//获取捕获值
            TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Rising);//要设置为上升沿，等待下降沿的来临
            }
            else  //第一次捕获
            {
                TM5_CH1_CAPTURE_STA = 0;
                TM5_CH1_CAPTURE_VAL = 0;
                TIM_SetCounter(TIM5, 0);  //还没等到下降沿来时把所有的都清零
                TM5_CH1_CAPTURE_STA |= 0x40;
                TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Falling);//要设置成下降沿，等到上升沿的来临
            }   
        }
    }
    TIM_ClearITPendingBit(TIM5, TIM_IT_CC1 | TIM_IT_Update);
}









