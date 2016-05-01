#include "timer4_cap.h"
#include "stm32f10x_tim.h"

uint8_t  TIM4CH1_CAPTURE_STA = 0;	//通道1输入捕获标志，高两位做捕获标志，低6位做溢出标志		
uint16_t TIM4CH1_CAPTURE_UPVAL;
uint16_t TIM4CH1_CAPTURE_DOWNVAL;


uint32_t Channel_1_UpTime = 0;	//捕获总高电平的时间



//定时器4通道1输入捕获配置

TIM_ICInitTypeDef TIM4_ICInitStructure;

void TIM4_Cap_Channel_1_Init(uint16_t arr, uint16_t psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能TIM4时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIOB时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  //PB6清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PB6 输入 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);//PB6  下拉

	//初始化定时器4 TIM4，该部分配置主要是为PWM输出服务 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 	//预分频器 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	//初始化TIM4输入捕获参数 通道1
	TIM4_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM4_ICInitStructure.TIM_ICFilter = 0x0F;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM4, &TIM4_ICInitStructure);

	//中断分组初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);   //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 

	TIM_ITConfig(TIM4, TIM_IT_CC1,ENABLE);   //不允许更新中断，允许CC1IE,CC2IE,CC3IE,CC4IE捕获中断	

	TIM_Cmd(TIM4, ENABLE); 		//使能定时器4

}

//定时器4中断服务程序,用CH1作为超声波转换的通道 即PB6端口
void TIM4_IRQHandler(void)
{
	if ((TIM4CH1_CAPTURE_STA & 0x80) == 0) 		//还未成功捕获	
	{
		if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET) 		//捕获1发生捕获事件
		{
			TIM_ClearITPendingBit(TIM4, TIM_IT_CC1); 		//清除中断标志位
			if (TIM4CH1_CAPTURE_STA & 0X40)		//捕获到一个下降沿
			{
                Channel_1_UpTime = 0;
				TIM4CH1_CAPTURE_DOWNVAL = TIM_GetCapture1(TIM4);//记录下此时的定时器计数值
				if (TIM4CH1_CAPTURE_DOWNVAL > TIM4CH1_CAPTURE_UPVAL)/* 如果计数器初始值大于末尾值，说明计数器有溢出 不大于才正常*/
				{
					Channel_1_UpTime = TIM4CH1_CAPTURE_DOWNVAL - TIM4CH1_CAPTURE_UPVAL;		//得到总的高电平的时间
				}
				TIM4CH1_CAPTURE_STA = 0;//捕获标志位清零，这一步很重要！
				TIM_OC1PolarityConfig(TIM4, TIM_ICPolarity_Rising); //设置为上升沿捕获		  
			}
			else //发生捕获时间但不是下降沿，第一次捕获到上升沿，记录此时的定时器计数值
			{
				TIM4CH1_CAPTURE_UPVAL = TIM_GetCapture1(TIM4);		//获取上升沿数据
				TIM4CH1_CAPTURE_STA |= 0x40;		//标记已捕获到上升沿
				TIM_OC1PolarityConfig(TIM4, TIM_ICPolarity_Falling);//设置为下降沿捕获
			}
		}
	}
}
