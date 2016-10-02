## 2016TI杯四川区电设省赛-电子称说明
----------------------------------------------------------------------------------------
`STM32` `Contiki` `HX711` `电子称`

[![Build Status](https://travis-ci.org/zthxxx/STM32-Contiki_Sensor_Smart_Home.svg?branch=master)](https://travis-ci.org/zthxxx/STM32-Contiki_Sensor_Smart_Home)
[![Code Climate](https://codeclimate.com/github/zthxxx/STM32-Contiki_Sensor_Smart_Home/badges/gpa.svg)](https://codeclimate.com/github/zthxxx/STM32-Contiki_Sensor_Smart_Home)
[![Test Coverage](https://codeclimate.com/github/zthxxx/STM32-Contiki_Sensor_Smart_Home/badges/coverage.svg)](https://codeclimate.com/github/zthxxx/STM32-Contiki_Sensor_Smart_Home/coverage)

> Here's how you use **`STM32-2016_NUEDC_TI_electronic_scale`** to download a video from [this web page](https://github.com/zthxxx/STM32-Contiki_Sensor_Smart_Home/tree/competition):


### 项目简介

-  本项目为  2016年第八届TI杯[**全国大学生电子设计竞赛**](http://www.nuedc.com.cn) 四川赛区 **D题-简易电子称** 的参赛作品
-  开发团队为来自西华大学的 **`WNCG 团队`**
-  此作品在参赛评选中获得 **省一等奖**
-  作品总共分为三个主要部分，分别为硬件端、下位机软件端、云服务器端
-  以下将从下位机软件系统进行较为详细的说明


### 必要说明

1. 本项目基于本团队原型 [**智能家居**](https://github.com/zthxxx/STM32-Contiki_Sensor_Smart_Home) 项目， 因此会有上述服务器端，  
        智能家居项目大概架构是采集各传感器数据，上传到云端储存和分析，通过**web**及**webapp**提供GUI。  
        因为智能家居项目所设计架构本身就是为了满足后续使用的通用性，  
        因此本项目中是直接把**应变片**及**HX711**一起看作**称重传感器**，挂载到智能家居整体系统的传感器组件下，  
        并裁剪其他传感器，只保留 **WIFI / 以太网** 模块及 **OLED** 显示模块；  
        称重数据上传服务器，把web界面做为称重**数据可视化及调试**界面。  
2. 本说明着重讲解**下位机程序架构**，硬件只叙述模块接口，不过多涉及硬件架构；  
3. 云服务器端请参见 [**物联网多终端平台**](https://github.com/zthxxx/python-flask-IoT_Sensor_Web) 项目；  


### 开发环境

| 项            | 说 明                                                        |
| ------------- | ------------------------------------------------------------ |
| 本项目全称    | 基于STM32 Contiki系统的物联网多传感器智能家居系统-电子称分支 |
| 开发人员      | 西华大学 WNCG 团队                                           |
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

* **始终仅将 USART1 做为 DEBUG 调试打印端口**
* **下载程序**与在线仿真使用 ST-LINK / JLINK **SWD接口**
* **Flash大小配置文件**:  
    ./User/STM_FLASH/stm_flash.h 中 **STM32_FLASH_SIZE** 配置对应芯片 Flash ROM 大小 (**！必要！**)   
    Flash 最后一页头4个字节做为随机数种子存放位置  
* **WIFI连接配置文件**(雾):  ./Hardware/ESP8266/ESP8266_Wifi_link_Config.c
* **以太网连接配置文件**:  ./Hardware/W5500/W5500.c
* **键盘按键配置文件**:   
    ./Hardware/Keyboard_4x5/Keyboard_4x5.c 实体按键与虚拟按键映射  
    ./Hardware/Keyboard_4x5/Keyboard_4x5.h 按键大小及引脚、系统虚拟按键定义  
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




### 电子称功能程序设计  

#### 程序功能描述与设计思路  
* 程序功能描述  
    根据题目要求软件部分主要分为称重传感器数据的采集、实现键盘的设置和显示。  
    * 键盘部分功能：单价设置输入、切换单位、实现价格累加和去皮功能。  
    * 显示部分：实现显示重量、价格、单价、总价、错误提示等功能。  
* 程序设计思路
    根据题目要求，将软件系统分为数据采集、界面显示、人机交互三大部分，各部分采用多个进程协同完成功能，每个进程职责单一。通过Contiki嵌入式操作系统进行各部分任务进程的调度。  
    * 数据采集主要涉及AD芯片的读取，采集应变片变化，并通过数字滤波FIR处理数据，并转为所需要的重量数据。  
    * 界面显示部分是将重量信息，价格、单价、总价等信息，统一格式显示在屏幕上。  
    * 人机交互部分是通过按键队列产生按键事件驱动不同的进程响应操作，以实现设置单价、累加价格、去皮校零的逻辑功能。  


#### 程序规划流程图  

  
  
![系统流程图](https://github.com/zthxxx/STM32-Contiki_Sensor_Smart_Home/raw/competition/Documents/resource/images/系统流程图.png)
  
  
![互交业务流程图](https://github.com/zthxxx/STM32-Contiki_Sensor_Smart_Home/raw/competition/Documents/resource/images/互交业务流程图.png)
  
  
![测重流程图](https://github.com/zthxxx/STM32-Contiki_Sensor_Smart_Home/raw/competition/Documents/resource/images/测重流程图.png)
  
  
![按键流程图](https://github.com/zthxxx/STM32-Contiki_Sensor_Smart_Home/raw/competition/Documents/resource/images/按键流程图.png)










