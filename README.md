# Welcome to coffee-machine project!

Contributors: Zhu Yu, Shen Junyi, Song Mengxuan

## 一、项目结构简述

### 1.1 项目流程图

<img src="E:\STUDY\ES\coffee-machine\doc\流程图.png" alt="流程图" style="zoom: 67%;" />

<p align="center" style="color:red;">我期待着一张更好的图来替换这张图(((φ(◎ロ◎;)φ)))</p>

### 1.2 项目流程介绍

该流程将在[程序介绍](###3.1 程序架构)中通过大致程序框架进行表述

1. 机器一启动，在完成了基本的串口时钟等硬件初始化之后，便进入“等待ESP32的指令的状态”（主线程）

   我们将在[这里](###2.3 STM32与ESP32通信协议约定)对STM32与ESP32之间的沟通协议进行约定

2. 当ESP32接收到来自平台的指令后，会将该指令转发至STM32，并等待STM32对该指令进行回复

3. STM32接收到来自ESP32的指令后，根据不同的情况认定成4种不同的处理方式：配置修改、传感器读取、开始制作、紧急停止

   + 配置修改：首先判断系统当前状态，若系统正在制作咖啡中，该操作需要保存，等待咖啡制作结束后再对配置进行修改

     我们将在[这里](###2.1 系统的状态)对系统的状态进行介绍，在[这里](###2.2 系统配置变量)对配置变量做介绍，在[这里](###2.4 操作保存机制)对操作保存机制作基本的介绍

   + 传感器读取：在任何状态下，系统都可以对传感器的值进行读取，并及时将传感器的数据反馈回ESP32

   + 开始制作：这个步骤是最为复杂的步骤。首先需要结合[液位控制算法](###3.2 液位控制算法)放出定量的咖啡、糖水和牛奶使其在混合罐中混合；再结合[温度控制算法](###3.3 温度控制算法)使其加热到指定的温度，加热时需不断搅拌；最后将液体全部放出

   + 紧急停止：该指令仅在咖啡制作过程中有效，收到该指令后，关闭全部阀门、加热板停止供电、搅拌器停转

## 二、一些约定

### 2.1 系统的状态

系统的状态通过下面一个变量来描述

```c
typedef enum { Waiting, Making, Error } SystemStatus;
static SystemStatus SystemCurrentStatus = Waiting;
```

系统有三种状态：Waiting, Making, Error。Waiting表示系统未在制作咖啡中，等待“开始制作”指令；Making表示此时正在制作咖啡，配置的更改将会延后；Error表示制作过程出现问题，机器异常，需要检查。

该变量通过以下两组函数进行使用：

```c
// 读取系统当前状态
SystemStatus GetSystemStatus();

// 设置系统当前状态
void SetStatusMaking();
void SetStatusError();
void SetStatusWaiting();
```

请注意：不要直接对SystemCurrentStatus变量本身进行读写操作，这可能会导致异常。

### 2.2 系统配置变量

这个变量记录了最终咖啡的配料的比例和加热的温度，仅能在系统状态为==Waiting==下修改，可在任意状态下读取。在非==Waiting==状态下修改的是tempCfg的参数，并且会在指定时刻加入到buf队列中。

```c
typedef enum {coffee, sugar, milk, temp} cfg_property;
typedef struct {
	uint8_t coffee;	// 咖啡原浆的加入量
	uint8_t sugar;	// 糖水的加入量
	uint8_t milk;	// 牛奶的加入量
	uint8_t temp;	// 设定的加热温度
} SystemCfg;
typedef struct{
    SystemCfg buffer[5];
    int rear;
}SystemCfgBuffer;

static SystemCfg SystemCurrentCfg;
static SystemCfg tempCfg;
static SystemCfgBuffer buf;
```

使用以下函数对该配置进行设定，其中第一个参数可以在==coffee==,==sugar==,==milk==,==temp==中选择。

```c
void SetNextCfg(cfg_property, uint8_t);
```

使用以下函数加载配置和读取配置

```c
void SetCurrentCfg(void)
uint8_t GetCurrentCfg(cfg_property);
```



### 2.3 STM32与ESP32通信协议约定

0. json概述

   json是一种依靠字符串的一种通信方式，在各语言中都有着成熟的解析和生成方案，下面以C语言cJson为例介绍json的生成和解析。

   也可以点击[这里](https://blog.csdn.net/Mculover666/article/details/103796256)直接学习

   + cJson包的安装--项目中cJson包已经安装完成，只需

     ```c
     #include <cJSON.h>
     ```

   + json的解析（即将json字符串转换成c语言中的变量）

     假设有下面一段json字符串：

     ```c
     const char* json_str = 
         "{\"name\": \"test\",\"type\": \"json-string\"}";
     ```

     可以利用下列函数进行处理：

     ```c
     cJSON* json = cJSON_Parse(json_str);
     char* name = cJSON_GetObjectItem(json, "name")->valuestring;
     char* type = cJSON_GetObjectItem(json, "type")->valuestring;
     ```

   + json的生成（即将变量生成json字符串）

     假设我需要生成下列json对应的字符串

     ```json
     {
         "type": "json",
         "length": 100,
         "content":{
             "contentA": 1,
             "contentB": "str"
         }
     }
     ```

     可以利用以下代码：

     ```c
     cJSON* cjson = cJSON_CreateObject();
     cJSON_AddStringToObject(cjson, "type", "json");
     cJSON_AddNumberToObject(cjson, "length", 100);
     cJSON* content = cJSON_CreateObject();
     cJSON_AddNumberToObject(content, "contentA", 1);
     cJSON_AddStringToObject(content, "contentB", "str");
     cJSON_AddItemToObject(cjson, "content", content);
     char* json_str = cJSON_Print(cjson);
     ```

1. ESP32命令发送至STM32

   ```json
   {
       "type": "command",
       "id": 0,
       "command": {
           "variable": "coffee/sugar/milk/temp",
           "value": 0
       }
   }
   ```

   其中id字段是为了标识这个命令，要求这个命令和其对应的响应拥有相同的id

2. STM32执行命令后向ESP32发送响应

   正常执行命令反馈：

   ```json
   {
       "type": "response",
       "id": 0,
       "result": {
           "status": 0,
           "msg": "command execute succeed"
       }
   }
   ```

   咖啡机处于Making状态时：

   ```json
   {
       "type": "response",
       "id": 0,
       "result": {
           "status": 1,
           "msg": "waiting for finishing"
       }
   }
   ```

   其他的一些异常：

   ```json
   {
       "type": "response",
       "id": 0,
       "result": {
           "status": 2,
           "msg": "..."
       }
   }
   ```

3. ESP32向STM32发送变量获取请求

   ```json
   {
       "type": "request",
       "id": 0,
       "variable": "coffee/..."
   }
   ```

4. STM32回复ESP32的变量请求

   ```json
   {
       "type": "variable",
       "id": 0,
       "result": {
           "variable": "coffee/...",
           "value": 100
       }
   }
   ```

5. 启动与停机命令

   ```json
   {
       "type": "start"/"emergent stop",
       "id": 0
   }
   ```

6. 启动/停机响应

   ```json
   {
       "type": "status",
       "id": 0,
       "status": "waiting/making/error"
   }
   ```

### 2.4 操作保存机制

当ESP32命令STM32修改变量（如需要加入的咖啡的量、需要加热至的温度等）时咖啡机正在运行，采取机制：该变量不会被立即修改，而是等到机器制作完这一杯咖啡后再行修改，具体实现方案如下：

1. 在修改变量前，读取当前系统状态，如果是"Waiting"状态，立即执行。
2. 如果是"Making"状态，将该命令存入一个命令队列中
3. 当系统状态恢复成"Waiting"时，按次序执行该任务队列里的全部任务

这个机制已经封装在[SetNextCfg()](###2.2 系统配置变量)函数中，直接调用即可。

## 3 程序架构与算法

### 3.1 程序基本架构

本程序基于FreeRTOS运行。

---

编辑于2022/7/25









