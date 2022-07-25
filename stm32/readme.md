<center><h2>
    咖啡机程序设计说明
    </h2></center>

<center>SMX、SJY、ZY</center>

### 芯片接口功能定义

1. GPIO

|   加料传感器   | 接口 |
| :------------: | :--: |
| coffee_trigger | PC0  |
|  coffee_echo   | PC1  |
|  milk_trigger  | PA1  |
|   milk_echo    | PA2  |
| sugar_trigger  | PA4  |
|   sugar_echo   | PA5  |



|    继电器    | 接口 |
| :----------: | :--: |
| coffee_relay | PB0  |
|  milk_relay  | PB1  |
| sugar_relay  | PB2  |
| heater_relay | PB4  |
| mixer_relay  | PB3  |



2. UART

|         功能         | 接口 | 波特率 |
| :------------------: | :--: | :----: |
| uart2(temp_transmit) | PA2  | 38400  |
| uart2(temp_receive)  | PA3  | 38400  |
| uart3(esp_transmit)  | PB10 | 115200 |
|  uart3(esp_receive)  | PB11 | 115200 |
|      uart1(Tx)       | PA9  | 28800  |
|      uart1(Rx)       | PA10 | 28800  |





### 文件结构设计

脚本文件存储在`./stm32/MDK-ARM/coffee-machine/Core/{Inc, Rec}`下，部分文件由cubemx生成，仅做少量修改，我们自定义的文件包括：

- `sensor.h/.c` 定义了传感器读取距离
- `func.h/.c `定义了调用传感器数据、控制搅拌器、控制加热器的函数
- `sys.h/.c `定义了系统状态机
- `connect.h/.c`定义通信
- `main.h/.c`定义主函数







