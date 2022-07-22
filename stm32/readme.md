GPIO

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



UART

|         功能         | 接口 | 波特率 |
| :------------------: | :--: | :----: |
| uart2(temp_transmit) | PA2  | 38400  |
| uart2(temp_receive)  | PA3  | 38400  |
| uart3(esp_transmit)  | PB10 | 115200 |
|  uart3(esp_receive)  | PB11 | 115200 |
|      uart1(Tx)       | PA9  | 28800  |
|      uart1(Rx)       | PA10 | 28800  |





FUNCTIONS

|               接口               |               功能               | 文件.h |
| :------------------------------: | :------------------------------: | :----: |
|          `printf(...)`           |  已修改，输出到uart1，方便debug  | stdio  |
| `void HAL_Delay_us(uint32_t us)` |             微秒延时             |  main  |
|    `float get_coffee_dist()`     |     输出coffee探头测得的距离     | sensor |
|       milk、sugar接口类似        |                /                 | sensor |
|     `void shut_all_relay()`      |    所有继电器输出高电平，阻塞    |  func  |
|  `void add_coffee(uint8_t ml)`   | 加咖啡，单位毫升，需要知道底面积 |  func  |
|       milk、sugar接口类似        |                /                 |  func  |
|              mix...              |                                  |        |
|             heat...              |                                  |        |



