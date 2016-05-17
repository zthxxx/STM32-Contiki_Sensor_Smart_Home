#include "ProcessTask.h"


PROCESS(red_blink_process, "Red Blink");
PROCESS(green_blink_process, "Green Blink");
PROCESS(IWDG_Feed_process, "Timing to feed dog");
PROCESS(clock_test_process, "Test system delay");
PROCESS(cJSON_test_process, "Test cJSON Lib");
PROCESS(Communication_Protocol_Send_process, "Communication Protocol Send packet data");
PROCESS(CommunicatProtocol_Send_Sensor_Data, "Communication Protocol Send packet data");


PROCESS(wifi_send_test_process, "Wifi module send data test");
PROCESS(OLED_Show_Increment_process, "Show a increment num in OLED");
PROCESS(DHT11_Read_Data_process, "DHT11 read temperature and humidity test");
PROCESS(MQ02_Read_Value_process, "ADC read MQ02 value and print test");
PROCESS(HCSR501_Read_Status_process, "Read status of is anyone here");
PROCESS(HCSR04_Measure_Distance_process, "Measure distance with HC-SR04 UltrasonicWave rangefinder");
PROCESS(BH1750_Measure_Lumen_process, "Measure lumen with BH1750 Light Sensor");
PROCESS(RC522_Read_Card_process, "Read card ID and data with RC522 RFID");

AUTOSTART_PROCESSES(&etimer_process,&IWDG_Feed_process);

float temperatureGlobalData;
float humidityGlobalData;
float smogPercentageGlobalData;
bool someoneStatusGlobalData;
uint16_t distanceGlobalData;
float lightIntensityGlobalData;
uint32_t CardID_GlobalData;


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
    uint8_t *USART2_SendBuff;
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {
        
        Contiki_etimer_DelayMS(500);
        USART2_SendBuff = " asdfafasdf";
        USART2_DMA_Send_Data(USART2_SendBuff, 11);
        
        Contiki_etimer_DelayMS(500);        
        USART2_SendBuff = " 21398416hy";
        USART2_DMA_Send_Data(USART2_SendBuff, 11);
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
        temperatureGlobalData = (float)temperature+(float)temperature0*0.01;
        humidityGlobalData = (float)humidity+(float)humidity0*0.01;
        
//        printf("temperature: %.2f°C  humidity: %.2f \r\n",(float)temperature+(float)temperature0*0.01,(float)humidity+(float)humidity0*0.01);	
        Contiki_etimer_DelayMS(500);
    }
    PROCESS_END();
}

PROCESS_THREAD(MQ02_Read_Value_process, ev, data)
{
    static struct etimer et;
    float smogPercentage;
    PROCESS_BEGIN();
    while(1)
    {
        smogPercentage = MQ02_Get_Percentage();
        smogPercentageGlobalData = smogPercentage;
//        printf("MQ02 Sensor Percentage: %.2f \r\n",MQ02_Get_Percentage());	
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
        someoneStatusGlobalData = someoneStatus;
//        if(someoneStatus)
//        {
//            printf("HCSR501 monitor people.\r\n");
//        }
//        else
//        {
//            printf("HCSR501 haven't monitor people.\r\n");
//        }
        Contiki_etimer_DelayMS(500);
    }
    PROCESS_END();
}

PROCESS_THREAD(HCSR04_Measure_Distance_process, ev, data)
{
    static struct etimer et;
    uint16_t distance;
    PROCESS_BEGIN();
    while(1)
    {
        UltrasonicWave_StartMeasure();
        Contiki_etimer_DelayMS(200);
        distance = UltrasonicWave_GetDistance();
        distanceGlobalData = distance;
//        printf("Ultrasonic distance : %d cm\r\n",UltrasonicWave_GetDistance());	//
    }
    PROCESS_END();
}

PROCESS_THREAD(BH1750_Measure_Lumen_process, ev, data)
{
    float lightIntensity;
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {
        Contiki_etimer_DelayMS(500);
        lightIntensity = BH1750_GetLumen();
        lightIntensityGlobalData = lightIntensity;
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
                    CardID_GlobalData = CardID;
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

    cJSON *root;char* cJSONout;
    PROCESS_BEGIN();

	root=cJSON_CreateObject();	
    
	cJSON_AddItemToObject(root, "Device", cJSON_CreateString("ContikiOS on STM32F103"));
    cJSON_AddItemToObject(root, "Address", cJSON_CreateNumber(0xFFFF));
    cJSON_AddItemToObject(root, "InfoType", cJSON_CreateString("Information"));
	cJSON_AddItemToObject(root, "DataName", cJSON_CreateNull());

    cJSONout = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);	
    AssembleProtocolPacketPushSendQueue(FunctionWord_StartUP, strlen(cJSONout), (uint8_t*)cJSONout);
    free(cJSONout);
    PROCESS_END();
}


PROCESS_THREAD(CommunicatProtocol_Send_Sensor_Data, ev, data)
{
    static struct etimer et;
    cJSON *root;char* cJSONout;
    PROCESS_BEGIN();
    while(1)
    {
        root=cJSON_CreateObject();	
        
        cJSON_AddItemToObject(root, "Device", cJSON_CreateString("Sensor via ContikiOS"));
        cJSON_AddItemToObject(root, "Address", cJSON_CreateNumber(0xFFFF));
        cJSON_AddItemToObject(root, "InfoType", cJSON_CreateString("Data"));
        cJSON_AddItemToObject(root, "Temperature", cJSON_CreateNumber(temperatureGlobalData));
        cJSON_AddItemToObject(root, "Humidity", cJSON_CreateNumber(humidityGlobalData));
        cJSON_AddItemToObject(root, "SmogPercentage", cJSON_CreateNumber(smogPercentageGlobalData));
        cJSON_AddItemToObject(root, "BodyStatus", cJSON_CreateBool(someoneStatusGlobalData));
        cJSON_AddItemToObject(root, "WaveDistance", cJSON_CreateNumber(distanceGlobalData));
        cJSON_AddItemToObject(root, "LightIntensity", cJSON_CreateNumber(lightIntensityGlobalData));
        cJSON_AddItemToObject(root, "CardID", cJSON_CreateNumber(CardID_GlobalData));
        
        cJSONout = cJSON_PrintUnformatted(root);
        cJSON_Delete(root);	
        AssembleProtocolPacketPushSendQueue(FunctionWord_Data, strlen(cJSONout), (uint8_t*)cJSONout);
        Contiki_etimer_DelayMS(300);
    }
    PROCESS_END();
}


PROCESS_THREAD(Communication_Protocol_Send_process, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();
    while(1)
    {
        SendUnsentPacketQueue();
        SendUnackedPacketQueue();
        LoadReceiveQueueByteToPacketBlock();
        DealWithReceivePacketQueue();
//        IncreaseUnackedPacketQueueResendTime();
        
        Contiki_etimer_DelayMS(60);
    }
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







