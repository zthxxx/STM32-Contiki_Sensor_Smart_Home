## Contiki 嵌入式智能家居系统
----------------------------------------------------------------------------------------
`STM32` `Contiki` `传感器` `智能家居`



### 项目简介

- 作品总共分为三个主要部分，分别为硬件端、下位机软件端、云服务器端
- 以下将从下位机软件系统进行较为详细的说明
- 本说明着重讲解**嵌入式下位机程序架构**，硬件只叙述模块接口，不过多涉及硬件架构
- 云服务器端请参见 [**物联网多终端平台**](https://github.com/zthxxx/python-flask-IoT_Sensor_Web) 项目


### 开发环境

| 名称          | 说 明                                                        |
| ------------- | ------------------------------------------------------------ |
| 本项目全称    | 基于STM32 Contiki系统的物联网多传感器智能家居系统-嵌入式部分 |
| 开发系统      | Windows 10                                                   |
| MCU           | STM32F103系列 型号R8以上 (RAM >= 20K，推荐64K)               |
| IDE           | MDK517 (Keil uVision5以上均可)                               |
| Language      | C                                                            |
| STM32 Package | STM32F103 Lib Package v2.0.0 (2.0以上均可)                   |

### 文件结构

>  **./**  项目主目录  
>  ├── **Documents**  说明文档文件夹，包含声明，更新日志，lssues  
>  │  
>  ├── **Hardware**   外围挂接设备驱动文件夹，各外设驱动打包在内各自独立文件夹中  
>  │  
>  ├── **Libraries**  库文件夹，包含STM32库依赖、第三方C库  
>  │  
>  ├── **Listing**    编译链接生成文件夹  
>  │  
>  ├── **Output**     编译输出文件夹  
>  │  
>  ├── **Project**    项目工程文件夹  
>  │  
>  └── **User**       用户文件夹，包含各Contiki系统、内设驱动，各内设驱动打包在内各自独立文件夹中  



### 配置说明

* **始终仅将 USART1 做为 DEBUG 调试打印端口**，波特率 921600
* **下载程序**与在线仿真使用 ST-LINK / JLINK **SWD接口**
* **Flash大小配置文件**:  
    　./User/STM_FLASH/stm_flash.h 中 **STM32_FLASH_SIZE** 配置对应芯片 Flash ROM 大小 (**！必要！**)   
    　Flash 最后一页头4个字节做为随机数种子存放位置  
* **WIFI连接配置文件**:  ./Hardware/ESP8266/ESP8266_Wifi_link_Config.c
* **以太网连接配置文件**:  ./Hardware/W5500/W5500.c
* **传感器裁剪**:   ./User/Contiki-ProcessTask/ProcessTask.h 中，  
    　`__TERMINAL_ON__` 与 `__TERMINAL_OFF__` 宏定义间裁剪所需传感器  
* **传输层配置**:   
    本项目中传感网内及传感网连接服务器间传输层均使用此套自定义协议  
    协议规范请参见 [传感物联网数据包协议格式](./Documents/传感物联网数据包协议格式.docx)  
    * 协议地址:  
        　./User/CommunicationProtocol/CommunicationConfig.c 中设置本机传输层默认地址  
        　./User/Contiki-ProcessTask/ProcessTask.c 文件中 `CommunicatProtocol_Send_Sensor_Data()` 函数调用 `AssembleProtocolPacketPushSendQueue()` 处配置传输层默认向上发送地址  
    * 协议通信出口:  
        　./User/CommunicationProtocol/CommunicationConfig.h 中 `TianProtocolSendBytesDataBuf` 定义处设置默认通信出口设备  
    * 超时与重发:  
         　./User/CommunicationProtocol/CommunicationConfig.h 配置重发次数与超时时间  
* **传感网链路层配置**:  
    传感网链路层指下位机各终端和节点之间互相组成的网络，  
    目前传感网链路层主要使用 E30TTL100 模块传输，  
    在 ./Hardware/E30TTL100/E30TTLUART.c 中:  
    * 目标默认地址与信道:  
        　`E30TTLUART_Appoint_Target_Address` 与 `E30TTLUART_Appoint_Target_Channel` 定义处分别设置对目标的默认地址与信道  
    * 本机地址与信道与工作模式:  
        　`E30TTLUART_Config()` 与 `E30TTLUART_MultiCountConfig()` 函数调用处设置本机地址信道与是否点对点模式  
        　此两函数通常在初始化模块处被调用 (本项目中模块初始化在 main.c `BSP_Config()` 中)  
* **对服务器链路层配置**:  
    传感网连接服务器链路层必定使用互联网模块 ESP8266 WIFI 模块、 W5500 硬件以太网模块，  
    因此对服务器链路层配置本质就是以上互联网模块驱动中对连接服务器的 Socket 配置  
* **中断优先级**:  
    所有用到的中断，初始化处，请使用 ./User/NVIC/NVIC.config.c 中 `NVIC_IRQChannel_Configuration_Set()` 函数正确合理的设置中断优先级。
* **硬件模块接口**:  
    单片机内设通信接口引脚由芯片固定，具体信息请自查单片机数据手册  
    各挂接硬件模块与单片机接口引脚说明文件均在 ./Hardware/ 下模块各自驱动文件夹内，引脚配置在同目录下 .c .h 文件中  
    (**修改时务必确保各模块驱动 .c .h 文件代码中配置引脚与说明文件中一致！**)  


### 添加传感模块流程

当需要添加一个挂载的模块或者添加一种新的功能时，一般可按照下列步骤操作 

1.  在 ./Hardware/ 或 ./User/ 下建立模块驱动对应文件夹  
2.  在模块驱动文件夹中建立接口说明文件，声明预设引脚接口  
3.  大致编写驱动文件(.c .h)，实现初始化及部分操作代码，所使用引脚按照说明文件定义  
     驱动文件及说明文件文件名均以模块名开头，驱动头文件中添加防重复引用宏 `__模块名__H`  
     驱动文件所用模块变量及函数名尽可能均使用 `模块名_` 作为名称开头  
     无特殊情况下 C 文件 include 只引用头文件  
4.  在IDE MDK中，菜单栏 `Project` --> `Option for Target 'xxx'` --> `c/c++` --> `Include Paths` 添加新建的驱动文件夹路径  
5.  在IDE MDK `Project Window` 栏中 Hardware 或 USER 组下添加初步编写的模块 C 文件  
6.  为模块驱动文件添加具体的依赖头文件引用，完善功能代码，确保文件能通过编译  
7.  重启 MDK 以保存工程文件结构  
8.  在 main.c 与 ProcessTask.h 中添加驱动头文件引用  
9.  在 ProcessTask.h 定义开启模块的宏 `__模块名_ON__`  
10.  在 main.c 中 `BSP_Config()` 内添加响应模块宏时的模块初始化操作  
11.  在 ProcessTask.h 中声明模块操作进程名，在 ProcessTask.c 中定义进程及描述，并实现进程操作代码  
12.  在main.c 中 `main()` 内添加响应模块宏时的开启进程操作  
13.  调试代码各依赖项，确保工程通过编译  
14.  模块驱动及模块进程中添加调试输出代码  
15.  编写模块单元测试代码  
16.  将程序下载到 MCU 实际测试驱动输出  
17.  DEBUG，直到模块驱动功能正常  
18.  屏蔽测试代码  
19.  至此添加一个模块完成  


### 天猫自定通信协议 - 实现部分

#### 发送消息流程

应用层产生消息 --> cJSON 封装消息 --> cJSON 获取 json 字符串 --> 生成协议包 PackBlock 结构体 --> 转为待发送的字符流 --> 字符流添加到未发送缓冲队列  
--> 从未发送队列取出字符流包 --> 指定通道发送字符流 --> 将字符流包放入未响应队列  
--> 遍历未响应队列 --> 达到重发时间的包重新发送一遍并把重发计数 +1 --> 删除达到最大重发次数的包  

#### 接收消息流程

传输层通道接收字节流 --> 字节转 FIFO 节点结构体 --> 放入接收 FIFO 缓存队列  
--> 从接收缓存中依次读取字节 --> 判断协议包头相等 --> 读取整个首部 --> 校验首部 --> 读取数据域 --> 校验数据和 --> 存入协议包 PackBlock 结构体 --> 存入接收包缓冲队列  
--> 从接收包缓存中取出协议包 --> 判断包目标地址 --> 删除应答包 --> 发送回应包 --> 处理包









