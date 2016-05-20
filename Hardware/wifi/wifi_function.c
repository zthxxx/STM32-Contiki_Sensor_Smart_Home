#include "wifi_function.h"
#include "wifi_config.h"
#include "bsp_gpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include <string.h> 
#include <stdio.h>  
#include <stdbool.h>



/*
 * 函数名：ESP8266_Rst
 * 描述  ：重启WF-ESP8266模块
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被ESP8266_AT_Test调用
 */
void ESP8266_Rst(void)
{

//    ESP8266_Cmd("AT+RST", "OK", "ready", 2500);   	

    ESP8266_RST_LOW_LEVEL();
    Delay_ms(100); 
    ESP8266_RST_HIGH_LEVEL();
    Delay_ms(2000); 
}


/*
 * 函数名：ESP8266_AT_Test
 * 描述  ：对WF-ESP8266模块进行AT测试启动
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_AT_Test(void)
{
	ESP8266_RST_HIGH_LEVEL();
	Delay_ms(2000); 
	while (!ESP8266_Cmd ("AT", "OK", NULL, 250))
    {
        ESP8266_Rst();
    }
}


/*
 * 函数名：ESP8266_Cmd
 * 描述  ：对WF-ESP8266模块发送AT指令
 * 输入  ：cmd，待发送的指令
 *         reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
 *         waittime，等待响应的时间
 * 返回  : 1，指令发送成功
 *         0，指令发送失败
 * 调用  ：被外部调用
 */
bool ESP8266_Cmd (char * cmd, char * reply1, char * reply2, u32 waittime)
{    
	strEsp8266_Fram_Record .InfBit .FramLength = 0;               //从新开始接收新的数据包

	ESP8266_Usart("%s\r\n", cmd);

	if((reply1 == 0) &&(reply2 == 0))                      //不需要接收数据
		return true;
	
	Delay_ms(waittime);                 //延时
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

  
	if((reply1 != 0) &&(reply2 != 0))
		return((bool) strstr(strEsp8266_Fram_Record .Data_RX_BUF, reply1) ||(bool) strstr(strEsp8266_Fram_Record .Data_RX_BUF, reply2)); 
 	
	else if(reply1 != 0)
		return((bool) strstr(strEsp8266_Fram_Record .Data_RX_BUF, reply1));
	
	else
		return((bool) strstr(strEsp8266_Fram_Record .Data_RX_BUF, reply2));
	

}


/*
 * 函数名：ESP8266_Net_Mode_Choose
 * 描述  ：选择WF-ESP8266模块的工作模式
 * 输入  ：enumMode，工作模式
 * 返回  : 1，选择成功
 *         0，选择失败
 * 调用  ：被外部调用
 */
bool ESP8266_Net_Mode_Choose(ENUM_Net_ModeTypeDef enumMode)
{
    switch(enumMode)
    {
        case STA:
            return ESP8266_Cmd("AT+CWMODE=1", "OK", "no change", 250); 

        case AP:
            return ESP8266_Cmd("AT+CWMODE=2", "OK", "no change", 250); 

        case STA_AP:
            return ESP8266_Cmd("AT+CWMODE=3", "OK", "no change", 250); 

        default:
            return false;
    }
	
}


/*
 * 函数名：ESP8266_JoinAP
 * 描述  ：WF-ESP8266模块连接外部WiFi
 * 输入  ：pSSID，WiFi名称字符串
 *       ：pPassWord，WiFi密码字符串
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
 */
bool ESP8266_JoinAP(char * pSSID, char * pPassWord)
{
	char cCmd [120];
	sprintf(cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord);
	return ESP8266_Cmd(cCmd, "OK", NULL, 1300);
}

/*
 * 函数名：ESP8266_STAJoinTCP
 * 描述  ：WF-ESP8266模块连接WiFi后连接tcp server
 * 输入  ：ip，tcp server地址
 *       ：port，tcp server 端口
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
 */
bool ESP8266_STAJoinTCP(char * ip, char * port)
{
	char cCmd [120];

	sprintf(cCmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", ip, port);
	
	return ESP8266_Cmd(cCmd, "OK", "ALREADY CONNECTED", 1000);
	
}

/*
 * 函数名：ESP8266_BuildAP
 * 描述  ：WF-ESP8266模块创建WiFi热点
 * 输入  ：pSSID，WiFi名称字符串
 *       ：pPassWord，WiFi密码字符串
 *       ：enunPsdMode，WiFi加密方式代号字符串
 * 返回  : 1，创建成功
 *         0，创建失败
 * 调用  ：被外部调用
 */
bool ESP8266_BuildAP(char * pSSID, char * pPassWord, char * enunPsdMode)
{
	char cCmd [120];

	sprintf(cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%s", pSSID, pPassWord, enunPsdMode);
	
	return ESP8266_Cmd(cCmd, "OK", 0, 1000);
	
}


/*
 * 函数名：ESP8266_Enable_MultipleId
 * 描述  ：WF-ESP8266模块启动多连接
 * 输入  ：enumEnUnvarnishTx，配置是否多连接
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_Enable_MultipleId(FunctionalState enumEnUnvarnishTx)
{
	char cStr [20];
	
	sprintf(cStr, "AT+CIPMUX=%d",(enumEnUnvarnishTx ? 1 : 0));
	
	return ESP8266_Cmd(cStr, "OK", 0, 500);
	
}


/*
 * 函数名：ESP8266_Link_Server
 * 描述  ：WF-ESP8266模块连接外部服务器
 * 输入  ：enumE，网络协议
 *       ：ip，服务器IP字符串
 *       ：ComNum，服务器端口字符串
 *       ：id，模块连接服务器的ID
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
 */
bool ESP8266_Link_Server(ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
    char cStr [100] = { 0 }, cCmd [120];

    switch( enumE)
    {
        case enumTCP:
            sprintf(cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum);
            break;

        case enumUDP:
            sprintf(cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum);
            break;

        default:
            break;
    }

    if(id < 5)
        sprintf(cCmd, "AT+CIPSTART=%d,%s", id, cStr);

    else
        sprintf(cCmd, "AT+CIPSTART=%s", cStr);

    return ESP8266_Cmd(cCmd, "OK", "ALREAY CONNECT", 500);
	
}
//连接 tcp的函数
void ESP8266_linkTCP_join(void)
{
	while(!ESP8266_STAJoinTCP(TCP_SERVER_IPADDRESS,TCP_SERVER_PORT));
}

/*
 * 函数名：ESP8266_StartOrShutServer
 * 描述  ：WF-ESP8266模块开启或关闭服务器模式
 * 输入  ：enumMode，开启/关闭
 *       ：pPortNum，服务器端口号字符串
 *       ：pTimeOver，服务器超时时间字符串，单位：秒
 * 返回  : 1，操作成功
 *         0，操作失败
 * 调用  ：被外部调用
 */
bool ESP8266_StartOrShutServer(FunctionalState enumMode, char * pPortNum, char * pTimeOver)
{
	char cCmd1 [120], cCmd2 [120];

	if(enumMode)
	{
		sprintf(cCmd1, "AT+CIPSERVER=%d,%s", 1, pPortNum);
		
		sprintf(cCmd2, "AT+CIPSTO=%s", pTimeOver);

		return(ESP8266_Cmd(cCmd1, "OK", 0, 500) && ESP8266_Cmd(cCmd2, "OK", 0, 500));
	}
	
	else
	{
		sprintf(cCmd1, "AT+CIPSERVER=%d,%s", 0, pPortNum);

		return ESP8266_Cmd(cCmd1, "OK", 0, 500);
	}
	
}


/*
 * 函数名：ESP8266_TransparentTransmission
 * 描述  ：配置WF-ESP8266模块进入透传发送
 * 输入  ：无
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_TransparentTransmission(void)
{
	return (ESP8266_Cmd("AT+CIPMODE=1", "OK", "Link is builded", 250) && ESP8266_Cmd("AT+CIPSEND", "\r\n", ">", 250));
}


/*
 * 函数名：ESP8266_SendString
 * 描述  ：WF-ESP8266模块发送字符串
 * 输入  ：enumEnUnvarnishTx，声明是否已使能了透传模式
 *       ：pStr，要发送的字符串
 *       ：ulStrLength，要发送的字符串的字节数
 *       ：ucId，哪个ID发送的字符串
 * 返回  : 1，发送成功
 *         0，发送失败
 * 调用  ：被外部调用

 //	ESP8266_SendString(DISABLE, cStrInput, ul,(ENUM_ID_NO_TypeDef) uc);
 */
bool ESP8266_SendString(FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId)
{
	char cStr [20];
	bool bRet = false;
		
	if(enumEnUnvarnishTx)
		ESP8266_Usart("%s", pStr);

	
	else
	{
		if(ucId < 5)
			sprintf(cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2);

		else
			sprintf(cStr, "AT+CIPSEND=%d", ulStrLength + 2);
		
		ESP8266_Cmd(cStr, "> ", 0, 1000);

		bRet = ESP8266_Cmd(pStr, "SEND OK", 0, 1000);
  }
	
	return bRet;

}
void ESP8266_WIFIAP_join(void)
{
    uint8_t tryCount=0;
	while(!ESP8266_JoinAP(WIFI_SSID_NAME,WIFI_PASSWORD))printf("%d\r\n",tryCount++);;	
}


/*
 * 函数名：ESP8266_ReceiveString
 * 描述  ：WF-ESP8266模块接收字符串
 * 输入  ：enumEnUnvarnishTx，声明是否已使能了透传模式
 * 返回  : 接收到的字符串首地址
 * 调用  ：被外部调用
 */
char * ESP8266_ReceiveString(FunctionalState enumEnUnvarnishTx)
{
	char * pRecStr = 0;
	
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;
	while(! strEsp8266_Fram_Record .InfBit .FramFinishFlag);
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';
	
	if(enumEnUnvarnishTx)
	{
		if(strstr(strEsp8266_Fram_Record .Data_RX_BUF, ">"))
			pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

	}
	
	else 
	{
		if(strstr(strEsp8266_Fram_Record .Data_RX_BUF, "+IPD"))
			pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

	}

	return pRecStr;
}

/*
 * 函数名：ESP8266_STA_TCP_Client
 * 描述  ：WF-ESP8266模块进行STA TCP Clien测试
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_STA_TCP_Client(void)
{
	IS_WIFI_LOG_DEBUG && PC_Usart("\r\nESP8266 WiFi Module Start.\r\n");     //打印测试例程提示信息
    IS_WIFI_LOG_DEBUG && PC_Usart("Try to enter command mode.\r\n");
    do
    {
        ESP8266_Usart("+++");
        Delay_ms (50) ;
    }while(!ESP8266_Cmd ("AT", "OK", NULL, 100));//进入AT命令模式
    IS_WIFI_LOG_DEBUG && PC_Usart("AT command is OK.\r\n");
    
    if(ESP8266_Cmd ("AT+CIFSR","+CIFSR:STAIP,\"0.0.0.0\"",NULL,250))
    {
        PC_Usart("Not connected wifi\r\n");
        while(!ESP8266_Net_Mode_Choose(STA));
        IS_WIFI_LOG_DEBUG && PC_Usart("Try to connect wifi with SSID:%s.\r\n",WIFI_SSID_NAME);
        ESP8266_WIFIAP_join();
        IS_WIFI_LOG_DEBUG && PC_Usart("Linked Wifi.\r\n");
        IS_WIFI_LOG_DEBUG && PC_Usart("Config module into TCP Client\r\n");
        ESP8266_Enable_MultipleId(DISABLE);
         
    }
    IS_WIFI_LOG_DEBUG && PC_Usart("Try to connect TCP Server\r\n");
    ESP8266_linkTCP_join();//处理  tcp 服务连接的函数
    IS_WIFI_LOG_DEBUG && PC_Usart("Connected TCP Server\r\n");
    //配置相关设置  进入透传模式
    IS_WIFI_LOG_DEBUG && PC_Usart("Try to enter Transparent Transmission\r\n");
    while(!ESP8266_TransparentTransmission());
    IS_WIFI_LOG_DEBUG && PC_Usart("Changed into Transparent Transmission\r\n");
}


/*
 * 函数名：ESP8266_AP_TCP_Server
 * 描述  ：WF-ESP8266模块进行AP TCP Server测试
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_AP_TCP_Server(void)
{
	char cStrInput [100] = { 0 }, * pStrDelimiter [3], * pBuf, * pStr;
	u8 uc = 0;
 	 u32 ul = 0;


	ESP8266_AT_Test ();
	
	ESP8266_Net_Mode_Choose(AP);


	PC_Usart("\r\n请输入要创建的WiFi的名称、加密方式和密钥，加密方式的编号为：\
              \r\n0 = OPEN\
              \r\n1 = WEP\
              \r\n2 = WPA_PSK\
	            \r\n3 = WPA2_PSK\
              \r\n4 = WPA_WPA2_PSK\
							\r\n输入格式为：名称字符+英文逗号+加密方式编号+英文逗号+密钥字符+空格，点击发送\r\n");

	scanf("%s", cStrInput);

	PC_Usart("\r\n稍等片刻 ……\r\n");

	pBuf = cStrInput;
	uc = 0;
	while(( pStr = strtok(pBuf, ",")) != NULL)
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 
	
	ESP8266_BuildAP(pStrDelimiter [0], pStrDelimiter [2], pStrDelimiter [1]);
	ESP8266_Cmd("AT+RST", "OK", "ready", 2500); //*
		

	ESP8266_Enable_MultipleId(ENABLE);
		
	
	PC_Usart("\r\n请输入服务器要开启的端口号和超时时间（0~28800，单位：秒），输入格式为：端口号字符+英文逗号+超时时间字符+空格，点击发送\r\n");

	scanf("%s", cStrInput);

	PC_Usart("\r\n稍等片刻 ……\r\n");

	pBuf = cStrInput;
	uc = 0;
	while(( pStr = strtok(pBuf, ",")) != NULL)
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 

	ESP8266_StartOrShutServer(ENABLE, pStrDelimiter [0], pStrDelimiter [1]);
	
	
	do
	{
		PC_Usart("\r\n正查询本模块IP……\r\n");
	  ESP8266_Cmd("AT+CIFSR", "OK", "Link", 500);
		
		PC_Usart("\r\n请用手机连接刚才创建的WiFi，这里只连接一个手机，作为ID0，然后用手机网络调试助手以TCP Client连接刚才开启的服务器（AP IP）……\r\n");
		Delay_ms(20000) ;
	}	while(! ESP8266_Cmd("AT+CIPSTATUS", "+CIPSTATUS:0", 0, 500));
	

	PC_Usart("\r\n请输入要向端口手机（ID0）发送的字符串，输入格式为：字符串（不含空格）+空格，点击发送\r\n");

	scanf("%s", cStrInput);

	ul = strlen(cStrInput);
	
	ESP8266_SendString(DISABLE, cStrInput, ul, Multiple_ID_0);

	
	PC_Usart("\r\n请在手机网络调试助手发送字符串\r\n");
	while (1)
	{
	  pStr = ESP8266_ReceiveString(DISABLE);
		PC_Usart("%s", pStr);
	}

}


/*
 * 函数名：ESP8266_StaTcpClient_ApTcpServer
 * 描述  ：WF-ESP8266模块进行STA(TCP Client)+AP(TCP Server)测试
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_StaTcpClient_ApTcpServer(void)
{
	char cStrInput [100] = { 0 }, * pStrDelimiter [3], * pBuf, * pStr;
	u8 uc = 0;
  u32 ul = 0;


	ESP8266_AT_Test ();
	
	ESP8266_Net_Mode_Choose(STA_AP);


	PC_Usart("\r\n请输入要创建的WiFi的名称、加密方式和密钥，加密方式的编号为：\
						\r\n0 = OPEN\
						\r\n1  =WEP\
						\r\n2 = WPA_PSK\
						\r\n3 = WPA2_PSK\
						\r\n4 = WPA_WPA2_PSK\
						\r\n输入格式为：名称字符+英文逗号+加密方式编号+英文逗号+密钥字符+空格，点击发送\r\n");

	scanf("%s", cStrInput);

	PC_Usart("\r\n稍等片刻 ……\r\n");

	pBuf = cStrInput;
	uc = 0;
	while(( pStr = strtok(pBuf, ",")) != NULL)
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 
	
	ESP8266_BuildAP(pStrDelimiter [0], pStrDelimiter [2], pStrDelimiter [1]);
	ESP8266_Cmd("AT+RST", "OK", "ready", 2500); //*
	

	ESP8266_Cmd("AT+CWLAP", "OK", 0, 5000);
		
  do
	{
		PC_Usart("\r\n请输入要连接的WiFi名称和密钥，输入格式为：名称字符+英文逗号+密钥字符+空格，点击发送\r\n");

		scanf("%s", cStrInput);

		PC_Usart("\r\n稍等片刻 ……\r\n");

		pBuf = cStrInput;
		uc = 0;
		while(( pStr = strtok(pBuf, ",")) != NULL)
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
  } while(! ESP8266_JoinAP(pStrDelimiter [0], pStrDelimiter [1]));

	
	ESP8266_Enable_MultipleId(ENABLE);
		
	
	PC_Usart("\r\n请输入服务器要开启的端口号和超时时间（0~28800，单位：秒），输入格式为：端口号字符+英文逗号+超时时间字符+空格，点击发送\r\n");

	scanf("%s", cStrInput);

	PC_Usart("\r\n稍等片刻 ……\r\n");

	pBuf = cStrInput;
	uc = 0;
	while(( pStr = strtok(pBuf, ",")) != NULL)
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 

	ESP8266_StartOrShutServer(ENABLE, pStrDelimiter [0], pStrDelimiter [1]);
	
	
	do 
	{
		PC_Usart("\r\n请在电脑上将网络调试助手以TCP Server连接网络，并输入电脑的IP和端口号，输入格式为：电脑IP+英文逗号+端口号+空格，点击发送\r\n");

		scanf("%s", cStrInput);

		PC_Usart("\r\n稍等片刻 ……\r\n");

		pBuf = cStrInput;
		uc = 0;
		while(( pStr = strtok(pBuf, ",")) != NULL)
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
  } while(!(ESP8266_Link_Server(enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_0) &&
	              ESP8266_Link_Server(enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_1) &&
	              ESP8266_Link_Server(enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_2)));
		
	
	do
	{
		PC_Usart("\r\n正查询本模块IP，前一个为AP IP，后一个为STA IP……\r\n");
	  ESP8266_Cmd("AT+CIFSR", "OK", "Link", 500);
		
		PC_Usart("\r\n请用手机连接刚才创建的WiFi，这里只连接一个手机，作为ID3，然后用手机网络调试助手以TCP Client连接刚才开启的服务器（AP IP）……\r\n");
		Delay_ms(20000) ;
	}	while(! ESP8266_Cmd("AT+CIPSTATUS", "+CIPSTATUS:3", 0, 500));
	

	for(uc = 0; uc < 3; uc ++)
	{
		PC_Usart("\r\n请输入端口ID%d要发送的字符串，输入格式为：字符串（不含空格）+空格，点击发送\r\n", uc);

		scanf("%s", cStrInput);

		ul = strlen(cStrInput);
		
		ESP8266_SendString(DISABLE, cStrInput, ul,(ENUM_ID_NO_TypeDef) uc);
		
	}
	
	
	PC_Usart("\r\n请输入要向端口手机（ID3）发送的字符串，输入格式为：字符串（不含空格）+空格，点击发送\r\n");

	scanf("%s", cStrInput);

	ul = strlen(cStrInput);
	
	ESP8266_SendString(DISABLE, cStrInput, ul, Multiple_ID_3);

	
	PC_Usart("\r\n请在电脑网络调试助手或手机网络调试助手发送字符串……\r\n");
	while (1)
	{
	  pStr = ESP8266_ReceiveString(DISABLE);
		PC_Usart("%s", pStr);
	}
	
}


