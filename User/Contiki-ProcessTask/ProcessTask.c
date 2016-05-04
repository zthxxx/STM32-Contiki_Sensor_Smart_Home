#include "ProcessTask.h"


PROCESS(red_blink_process, "Red Blink");
PROCESS(green_blink_process, "Green Blink");
PROCESS(IWDG_Feed_process, "Timing to feed dog");
PROCESS(clock_test_process, "Test system delay");
PROCESS(cJSON_test_process, "Test cJSON Lib");

PROCESS(wifi_send_test_process, "Wifi module send data test");
PROCESS(OLED_Show_Increment_process, "Show a increment num in OLED");
PROCESS(DHT11_Read_Data_process, "DHT11 read temperature and humidity test");
PROCESS(MQ02_Read_Value_process, "ADC read MQ02 value and print test");
PROCESS(HCSR501_Read_Status_process, "Read status of is anyone here");
PROCESS(HCSR04_Measure_Distance_process, "Measure distance with HC-SR04 UltrasonicWave rangefinder");
PROCESS(BH1750_Measure_Lumen_process, "Measure lumen with BH1750 Light Sensor");
PROCESS(RC522_Read_Card_process, "Read card ID and data with RC522 RFID");

AUTOSTART_PROCESSES(&etimer_process,&IWDG_Feed_process);



/*******************PROCESS************************/

PROCESS_THREAD(red_blink_process, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {
        Contiki_etimer_DelayMS(500);
        GPIO_SetBits(GPIOA, GPIO_Pin_8);
        Contiki_etimer_DelayMS(500);
        GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    }
    PROCESS_END();
}

PROCESS_THREAD(green_blink_process, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {        
        Contiki_etimer_DelayMS(200);
        GPIO_SetBits(GPIOD, GPIO_Pin_2);
        Contiki_etimer_DelayMS(200);
        GPIO_ResetBits(GPIOD, GPIO_Pin_2);
    }
    PROCESS_END();
}

PROCESS_THREAD(wifi_send_test_process, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {
        
        Contiki_etimer_DelayMS(500);
        UART2_SendBuff = " asdfafasdf";
        UART2_DMA_Send_Data(UART2_SendBuff, 11);
        
        Contiki_etimer_DelayMS(500);        
        UART2_SendBuff = " 21398416hy";
        UART2_DMA_Send_Data(UART2_SendBuff, 11);
    }
    PROCESS_END();
}

PROCESS_THREAD(OLED_Show_Increment_process, ev, data)
{
    static struct etimer et;
    static int count;
    PROCESS_BEGIN();
    while(1)
    {
        OLED_ShowNum(0,32,count++,5,16);   
        OLED_Refresh_Gram();//更新显示
        Contiki_etimer_DelayMS(500);
    }
    PROCESS_END();
}

PROCESS_THREAD(DHT11_Read_Data_process, ev, data)
{
    static struct etimer et;
    uint8_t temperature;
    uint8_t temperature0;	 
	uint8_t humidity;    
    uint8_t humidity0;
    PROCESS_BEGIN();
    while(1)
    {
        DHT11_Read_Data(&temperature,&temperature0,&humidity,&humidity0);
        printf("temperature: %.2f°C  humidity: %.2f \r\n",(float)temperature+(float)temperature0*0.01,(float)humidity+(float)humidity0*0.01);	
        Contiki_etimer_DelayMS(500);
    }
    PROCESS_END();
}

PROCESS_THREAD(MQ02_Read_Value_process, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {
        printf("MQ02 Sensor Percentage: %.2f \r\n",MQ02_Get_Percentage());	
        Contiki_etimer_DelayMS(500);
    }
    PROCESS_END();
}

PROCESS_THREAD(HCSR501_Read_Status_process, ev, data)
{
    static struct etimer et;
    bool someoneStatus;
    PROCESS_BEGIN();
    while(1)
    {
        someoneStatus = HCSR501_Read_Data();
        if(someoneStatus)
        {
            printf("HCSR501 monitor people.\r\n");
        }
        else
        {
            printf("HCSR501 haven't monitor people.\r\n");
        }
        Contiki_etimer_DelayMS(500);
    }
    PROCESS_END();
}

PROCESS_THREAD(HCSR04_Measure_Distance_process, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {
        UltrasonicWave_StartMeasure();
        Contiki_etimer_DelayMS(200);
        printf("Ultrasonic distance : %d cm\r\n",UltrasonicWave_GetDistance());	//计算距离
    }
    PROCESS_END();
}

PROCESS_THREAD(BH1750_Measure_Lumen_process, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {
        Contiki_etimer_DelayMS(500);
        printf("Light sensor Lumen is : %.2f lx\r\n",BH1750_GetLumen());	//计算距离
    }
    PROCESS_END();
}

PROCESS_THREAD(RC522_Read_Card_process, ev, data)
{
    int8_t status;
    uint8_t CardClassAndIDBuf[4];
    uint32_t CardID;
    static uint32_t LastCardID;
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {
        status = PcdRequest(PICC_REQALL,CardClassAndIDBuf);/*扫描卡*/
        if(status!=MI_OK)
        {
            status = PcdRequest(PICC_REQALL,CardClassAndIDBuf);/*扫描卡*/
        }
        if(status==MI_OK)
        {            
            status = PcdAnticoll(CardClassAndIDBuf);/*防冲撞*/ 
            if(status==MI_OK)
            {
                CardID = (uint32_t)CardClassAndIDBuf[0];
                CardID += (uint32_t)CardClassAndIDBuf[1]<<8;
                CardID += (uint32_t)CardClassAndIDBuf[2]<<16;
                CardID += (uint32_t)CardClassAndIDBuf[3]<<24;
                if(LastCardID != CardID)
                {
                    LastCardID = CardID;
                    
                    printf("卡的序号:");
                    printf("%8X",CardID);					
                    printf("\r\n");
                }
            }
            else
            {
                LastCardID = 0;
            }
        }
        else
        {
            LastCardID = 0;
        }
        Contiki_etimer_DelayMS(200);
    }
    PROCESS_END();
}

PROCESS_THREAD(clock_test_process, ev, data)
{
    static uint16_t i,start_count,end_count,diff;
    PROCESS_BEGIN();

    printf("Clock delay test, (10,000 x i) cycles:\n");
    i = 1;
    while(i<16)
    {
        start_count = clock_time();                   // 记录开始timer
        Delay_NOP_ms(10 * i);                       // 软件延时
        end_count = clock_time();                     // 记录结束timer
        diff = end_count - start_count;               // 计算差值，单位为tick
        printf("Delayed %u \n%u ticks =~ %u ms\n", 10 * i, diff, diff * 10);
        i++;
    }

    printf("Done!\n");

    PROCESS_END();
}

PROCESS_THREAD(cJSON_test_process, ev, data)
{

    cJSON *root;
//    static struct etimer et;
    PROCESS_BEGIN();

    /* Our "Video" datatype: */
	root=cJSON_CreateObject();	
	cJSON_AddItemToObject(root, "MessageType", cJSON_CreateString("Information"));
	cJSON_AddItemToObject(root, "ModuleName", cJSON_CreateString("Contiki via STM32F103"));
    cJSON_AddItemToObject(root, "Address", cJSON_CreateNumber(0xFFFF));
	cJSON_AddItemToObject(root, "DataName", cJSON_CreateNull());
    
    printf("%s\n",cJSON_PrintUnformatted(root));
    cJSON_Delete(root);	

    PROCESS_END();
}


PROCESS_THREAD(IWDG_Feed_process, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {
        IWDG_Feed();
        Contiki_etimer_DelayMS(1000);
    }
    PROCESS_END();
}







