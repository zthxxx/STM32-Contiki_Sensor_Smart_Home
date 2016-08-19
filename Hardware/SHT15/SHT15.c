#include "SHT15.h"
#include <math.h>

/*************************************************************
  Function   ：SHT15_Dly  
  Description：SHT15时序需要的延时
  Input      : none        
  return     : none    
*************************************************************/
void SHT15_Dly(void)
{
    uint16_t i;
    for(i = 500; i > 0; i--);
}


/*************************************************************
  Function   ：SHT15_Config  
  Description：初始化 SHT15引脚
  Input      : none        
  return     : none    
*************************************************************/
void SHT15_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;        
    //初始化SHT15引脚时钟
    RCC_APB2PeriphClockCmd(SHT15_RCC_Periph ,ENABLE);
            
    //DATA 推挽输出        
    GPIO_InitStructure.GPIO_Pin = SHT15_DATA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SHT15_DATA_PORT, &GPIO_InitStructure);
    //SCK 推挽输出
    GPIO_InitStructure.GPIO_Pin = SHT15_SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SHT15_SCK_PORT, &GPIO_InitStructure);

    SHT15_ConReset();        //复位通讯
}


/*************************************************************
  Function   ：SHT15_DATAOut
  Description：设置DATA引脚为输出
  Input      : none        
  return     : none    
*************************************************************/
void SHT15_DATAOut(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //DATA 推挽输出        
    GPIO_InitStructure.GPIO_Pin = SHT15_DATA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         
    GPIO_Init(SHT15_DATA_PORT, &GPIO_InitStructure);
}


/*************************************************************
  Function   ：SHT15_DATAIn  
  Description：设置DATA引脚为输入
  Input      : none        
  return     : none    
*************************************************************/
void SHT15_DATAIn(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //DATA 浮动输入        
    GPIO_InitStructure.GPIO_Pin = SHT15_DATA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(SHT15_DATA_PORT, &GPIO_InitStructure);
}


/*************************************************************
  Function   ：SHT15_WriteByte  
  Description：写1字节
  Input      : value:要写入的字节        
  return     : err: 0-正确  1-错误    
*************************************************************/
uint8_t SHT15_WriteByte(uint8_t value)
{
    uint8_t i, err = 0;
    
    SHT15_DATAOut();                //设置DATA数据线为输出

    for(i = 0x80; i > 0; i /= 2)    //写1个字节
    {
        if(i & value)
                SHT15_DATA_H();
        else
                SHT15_DATA_L();
        SHT15_Dly();
        SHT15_SCK_H();
        SHT15_Dly();
        SHT15_SCK_L();
        SHT15_Dly();
    }
    SHT15_DATAIn();                 //设置DATA数据线为输入,释放DATA线
    SHT15_SCK_H();
    err = SHT15_DATA_R();           //读取SHT15的应答位
    SHT15_SCK_L();

    return err;
}

/*************************************************************
  Function   ：SHT15_ReadByte  
  Description：读1字节数据
  Input      : Ack: 0-不应答  1-应答        
  return     : err: 0-正确 1-错误    
*************************************************************/
uint8_t SHT15_ReadByte(uint8_t Ack)
{
    uint8_t i, val = 0;

    SHT15_DATAIn();                //设置DATA数据线为输入
    for(i = 0x80; i > 0; i /= 2)   //读取1字节的数据
    {
        SHT15_Dly();
        SHT15_SCK_H();
        SHT15_Dly();
        if(SHT15_DATA_R())
        {
           val = (val | i);
        }
        SHT15_SCK_L();
    }
    SHT15_DATAOut();               //设置DATA数据线为输出
    if(Ack)
            SHT15_DATA_L();        //应答，则会接下去读接下去的数据(校验数据)
    else
            SHT15_DATA_H();        //不应答，数据至此结束
    SHT15_Dly();
    SHT15_SCK_H();
    SHT15_Dly();
    SHT15_SCK_L();
    SHT15_Dly();

    return val;                    //返回读到的值
}


/*************************************************************
  Function   ：SHT15_TransStart  
  Description：开始传输信号，时序如下：
                     _____         ________
               DATA:      |_______|
                         ___     ___
               SCK : ___|   |___|   |______        
  Input      : none        
  return     : none    
*************************************************************/
void SHT15_TransStart(void)
{
    SHT15_DATAOut();               //设置DATA数据线为输出

    SHT15_DATA_H();
    SHT15_SCK_L();
    SHT15_Dly();
    SHT15_SCK_H();
    SHT15_Dly();
    SHT15_DATA_L();
    SHT15_Dly();
    SHT15_SCK_L();
    SHT15_Dly();
    SHT15_SCK_H();
    SHT15_Dly();
    SHT15_DATA_H();
    SHT15_Dly();
    SHT15_SCK_L();
}


/*************************************************************
  Function   ：SHT15_ConReset  
  Description：通讯复位，时序如下：
                     _____________________________________________________         ________
               DATA:                                                      |_______|
                        _    _    _    _    _    _    _    _    _        ___     ___
               SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
  Input      : none        
  return     : none    
*************************************************************/
void SHT15_ConReset(void)
{
    uint8_t i;

    SHT15_DATAOut();

    SHT15_DATA_H();
    SHT15_SCK_L();

    for(i = 0; i < 9; i++)              //触发SCK时钟9c次
    {
        SHT15_SCK_H();
        SHT15_Dly();
        SHT15_SCK_L();
        SHT15_Dly();
    }
    SHT15_TransStart();                 //启动传输
}



/*************************************************************
  Function   ：SHT15_SoftReset  
  Description：软复位
  Input      : none        
  return     : err: 0-正确  1-错误    
*************************************************************/
uint8_t SHT15_SoftReset(void)
{
    uint8_t err = 0;

    SHT15_ConReset();                   //通讯复位
    err += SHT15_WriteByte(SOFTRESET);  //写RESET复位命令

    return err;
}


/*************************************************************
  Function   ：SHT15_ReadStatusReg  
  Description：读状态寄存器
  Input      : p_value-读到的数据；p_checksun-读到的校验数据       
  return     : err: 0-正确  0-错误    
*************************************************************/
uint8_t SHT15_ReadStatusReg(uint8_t *p_value, uint8_t *p_checksum)
{
    uint8_t err = 0;

    SHT15_TransStart();                                        //开始传输
    err = SHT15_WriteByte(STATUS_REG_R);//写STATUS_REG_R读取状态寄存器命令
    *p_value = SHT15_ReadByte(ACK);                //读取状态数据
    *p_checksum = SHT15_ReadByte(noACK);//读取检验和数据
    
    return err;
}



/*************************************************************
  Function   ：SHT15_WriteStatusReg  
  Description：写状态寄存器
  Input      : p_value-要写入的数据值       
  return     : err: 0-正确  1-错误    
*************************************************************/
uint8_t SHT15_WriteStatusReg(uint8_t *p_value)
{
        uint8_t err = 0;

        SHT15_TransStart();                     //开始传输
        err += SHT15_WriteByte(STATUS_REG_W);   //写STATUS_REG_W写状态寄存器命令
        err += SHT15_WriteByte(*p_value);       //写入配置值

        return err;
}



/*************************************************************
  Function   ：SHT15_Measure  
  Description：从温湿度传感器读取温湿度
  Input      : p_value-读到的值；p_checksum-读到的校验数        
  return     : err: 0-正确 1—错误    
*************************************************************/
uint8_t SHT15_Measure(uint16_t *p_value, uint8_t *p_checksum, uint8_t mode)
{
    uint8_t err = 0;
    uint32_t i;
    uint8_t value_H = 0;
    uint8_t value_L = 0;

    SHT15_TransStart();                                     //开始传输
    switch(mode)                                                         
    {
    case TEMP:                                              //测量温度
        err += SHT15_WriteByte(MEASURE_TEMP);           //写MEASURE_TEMP测量温度命令
        break;
    case HUMI:
        err += SHT15_WriteByte(MEASURE_HUMI);           //写MEASURE_HUMI测量湿度命令
        break;
    default:
        break;
    }
    if(err != 0)
    {
        return err;
    }
    SHT15_DATAIn();
    for(i = 0; i < 1200000; i++)                           //等待DATA信号被拉低
    {
        if(SHT15_DATA_R() == 0) break;                  //检测到DATA被拉低了，跳出循环
    }
    if(SHT15_DATA_R() == 1)                                //如果等待超时了
    {
        err += 1;
        return err;
    }
    value_H = SHT15_ReadByte(ACK);
    value_L = SHT15_ReadByte(ACK);
    *p_checksum = SHT15_ReadByte(noACK);                    //读取校验数据
    *p_value = (value_H << 8) | value_L;
    return err;
}


/*************************************************************
  Function   ：SHT15_Calculate  
  Description：计算温湿度的值
  Input      : Temp-从传感器读出的温度值；Humi-从传感器读出的湿度值
               p_humidity-计算出的实际的湿度值；p_temperature-计算出的实际温度值        
  return     : none    
*************************************************************/
void SHT15_Calculate(uint16_t t, uint16_t rh, float *p_temperature, float *p_humidity)
{
    //参数数据由手册中来
    const float d1 = -39.55;
    const float d2 = +0.01;
    const float C1 = -4;
    const float C2 = +0.0405;
    const float C3 = -0.0000028;        
    const float T1 = +0.01;
    const float T2 = +0.00008;

    float RH_Lin;                                                     //RH线性值        
    float RH_Ture;                                                    //RH真实值
    float temp_C;

    temp_C = d1 + d2 * t;                                            //计算温度值        
    RH_Lin = C1 + C2 * rh + C3 * rh * rh;                            //计算湿度值
    RH_Ture = (temp_C -25) * (T1 + T2 * rh) + RH_Lin;                //湿度的温度补偿，计算实际的湿度值
    RH_Ture = (RH_Ture > 100) ? 100 : RH_Ture;
    RH_Ture = (RH_Ture < 0.1) ? 0.1 : RH_Ture;                       //设置湿度值下限

    *p_humidity = RH_Ture;
    *p_temperature = temp_C;

}


/*************************************************************
  Function   ：SHT15_CalcuDewPoint  
  Description：计算露点
  Input      : h-实际的湿度；t-实际的温度        
  return     : dew_point-露点    
*************************************************************/
float SHT15_CalcuDewPoint(float t, float h)
{
    float logEx, dew_point;

    logEx = 0.66077 + 7.5 * t / (237.3 + t) + (log10(h) - 2);
    dew_point = ((0.66077 - logEx) * 237.3) / (logEx - 8.16077);

    return dew_point; 
}


//main函数的编写：
//int main(void)
//{  
//        uint16_t humi_val, temp_val;
//        uint8_t err = 0, checksum = 0;
//        float humi_val_real = 0.0; 
//        float temp_val_real = 0.0;
//        float dew_point = 0.0;
//        
//        BSP_Init();
//        printf("\nSHT15温室度测试程序!!!\n");
//        SHT15_Config();
//        while(1)
//        {
//                err += SHT15_Measure(&temp_val, &checksum, TEMP);                  //获取温度测量值
//                err += SHT15_Measure(&humi_val, &checksum, HUMI);                  //获取湿度测量值
//                if(err != 0)
//                        SHT15_ConReset();
//                else
//                {
//                        SHT15_Calculate(temp_val, humi_val, &temp_val_real, &humi_val_real); //计算实际的温湿度值
//                        dew_point = SHT15_CalcuDewPoint(temp_val_real, humi_val_real);                 //计算露点温度
//                } 
//                printf("当前环境温度为:%2.1f℃，湿度为:%2.1f%%，露点温度为%2.1f℃\r\n", temp_val_real, humi_val_real, dew_point);
//                LED1_Toggle();
//                Delay_ms(1000);
//        }
//}





