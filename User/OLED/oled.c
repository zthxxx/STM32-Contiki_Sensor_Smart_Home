/*12864_SPI_OLED_STM32驱动***********/
/*OLED显示驱动模块：SSD1306**********/
/*四线SPI:SCL,SDA,REST,D/C***********/
/*对应接口：PA5,PA7,PA12,PA11********/
/*测试主板STM32F103RC*8M晶振*********/
/*源码来源：网络+店家提供源码********/
/*修改者：陈健***********************/
/*QQ:369763698***********************/
/*2014/7/10**************************/
#include "oled.h"
#include "AsciiLib.h"
#include "font.h"

#define	    BRIGHTNESS	  0xCF			//亮度
#define     X_WIDTH       128				//宽度
#define     Y_WIDTH       64				//长度
#define		  PAGE			    8					//页数
#define			MAX_CHAR_POSX X_WIDTH-6 //字符宽度需要-6
#define			MAX_CHAR_POSY Y_WIDTH-6 //字符长度需要-6

u8 OLED_GRAM [128][8];	 

/*******************一个字节数据写入***********************/
void OLED_WrDat(unsigned char dat)
{
    OLED_DC_H;
	  OLED_WB(dat);
}

/********************一条指令写入**********************/
void OLED_WrCmd(unsigned char cmd)
{
    OLED_DC_L;
    OLED_WB(cmd);
}

/********************更新显存到OLED**********************/	 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WrCmd(0xb0+i);    //设置页地址（0~7）
		OLED_WrCmd(0x10);      //设置显示位置—列低地址
		OLED_WrCmd(0x10);      //设置显示位置—列高地址   
		for(n=0;n<128;n++) 
		OLED_WrDat(OLED_GRAM[n][i]); 
	}   
}


/*********************开启OLED显示 ***********************/   
void OLED_Display_On(void)
{
	OLED_WrCmd(0X8D);  //SET DCDC命令
	OLED_WrCmd(0X14);  //DCDC ON
	OLED_WrCmd(0XAF);  //DISPLAY ON
}

/*********************关闭OLED显示 ***********************/    
void OLED_Display_Off(void)
{
	OLED_WrCmd(0X8D);  //SET DCDC命令
	OLED_WrCmd(0X10);  //DCDC OFF
	OLED_WrCmd(0XAE);  //DISPLAY OFF
}		

/*********************清屏函数***********************/
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_CLS(void)
{	
	OLED_Fill(0,0,127,63, 0x00);
	OLED_DLY_ms(200);//务必要等待，否则会有残留
}

/*********************延时函数***********************/
void OLED_DLY_ms(unsigned int ms)
{
    unsigned int a;
    while(ms)
    {
        a=1335;
        while(a--);
        ms--;
    }
    return;
}

void delay_us(u16 time)
{
	u16 i = 0;
	while(time -- )
	{
		i = 10;
		while(i --);
	}
}
/**********************写满屏数据**********************/
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();
}
/*********************12864初始化***********************/
void OLED_Init(void)
{
	OLED_RST_L;
	OLED_DLY_ms(50);
	OLED_RST_H;
	//从上电到下面开始初始化要有足够的时间，即等待RC复位完毕
  OLED_WrCmd(0xae);//关闭显示

	OLED_WrCmd(0xD5); //设置时钟分频因子,震荡频率
  OLED_WrCmd(0x80);   //[3:0],分频因子;[7:4],震荡频率, 默认100分频
  OLED_WrCmd(0xA8); //设置驱动路数(1 to 64)
  OLED_WrCmd(0X3F); //默认0X3F(1/64) 
  OLED_WrCmd(0xD3); //设置显示偏移
  OLED_WrCmd(0X00); //默认为0

  OLED_WrCmd(0x40); //设置显示开始行 [5:0],行数(0x00~0x3F).
  OLED_WrCmd(0xA1); //段重定义设置,bit0:0,0->0;1,0->127;0xa0左右反置 0xa1正常
  OLED_WrCmd(0xC0); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数0xc0上下反置 0xc8正常
													    
  OLED_WrCmd(0x8D); //电荷泵设置
  OLED_WrCmd(0x14); //bit2，开启/关闭set(0x10) disable
  OLED_WrCmd(0x20); //设置内存地址模式
  OLED_WrCmd(0x02); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
  OLED_WrCmd(0xDA); //设置COM硬件引脚配置
  OLED_WrCmd(0x12); //[5:4]配置
		 
  OLED_WrCmd(0x81); //对比度设置
  OLED_WrCmd(BRIGHTNESS); //1~255;默认0X7F (亮度设置,越大越亮)
  OLED_WrCmd(0xD9); //设置预充电周期
  OLED_WrCmd(0xf1); //[3:0],PHASE 1;[7:4],PHASE 2;
  OLED_WrCmd(0xDB); //设置VCOMH 电压倍率
  OLED_WrCmd(0x30); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

  OLED_WrCmd(0xA4); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
  OLED_WrCmd(0xA6); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
  OLED_WrCmd(0xAF); //开启显示	 
	
  OLED_Fill(0,0,127,63, 0x00);  //初始清屏,不清会有意想不到的问题
  OLED_Display_On();
  OLED_CLS();
}

//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//³¬³ö·¶Î§ÁË.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
//mode:1 填充 0,清空		
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//得到偏移后的值				   
    for(t=0;t<size;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t];  //调用1206字体
		else temp=asc2_1608[chr][t];		 //调用1608字体 	                          
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }  
}

//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体
void OLED_ShowString(u8 x,u8 y,const u8 *p)
{         
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;OLED_CLS();}
        OLED_ShowChar(x,y,*p,16,1);	 
        x+=8;
        p++;
    }
}   			 


//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 

/**********************设置显示位置**********************/
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
	/* PAGE addressing mode */
    OLED_WrCmd(0xb0+(y&0x07));/* set page start address */
    OLED_WrCmd(x&0x0f);/* set lower nibble of the column address */
    OLED_WrCmd(((x&0xf0)>>4)|0x10); /* set higher nibble of the column address */
}
/*********************写一位数据到SPI*******************/
void OLED_WB(uint8_t data)
{
    SPI1SendOneByte(data);
	delay_us(2);//这个延时是必要的，在STM32f103RC上8M晶振，低于1us会导致显存更新出错，绘屏会花，延时最短是2us（粗略延时）
}




/*************    OLED函数使用
//          测试开关，OK
//			OLED_Display_Off();	
//			OLED_Display_On();
//			OLED_CLS();
//			OLED_Fill(0,0,127,63,1);
//			OLED_DrawPoint(48,32,1);
//			OLED_ShowChar(0,0,'a',12,1);
//			OLED_ShowNum(0,0,333,3,12);
//			OLED_ShowString(0,0,"First");
//			OLED_Refresh_Gram();//更新显示
//			OLED_DLY_ms(5000);	
			
//			OLED_ShowString(20,24,"First OLED");
//			OLED_ShowNum(0,0,oled_time_num,3,12);
//			oled_time_num+=13;
//			OLED_Refresh_Gram();//更新显示
//	    OLED_DLY_ms(5000);	





*************/
