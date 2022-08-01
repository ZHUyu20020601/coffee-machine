#include "connect.h"
#include "string.h"
#include "stdio.h"
#include "sys.h"

extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;

//extern SystemCfg tempCfg;


uint8_t rx_buffer[200];   //接收数据的数组
uint8_t rx_log[50];   //日志数组
volatile uint8_t rx_len = 0; //接收数据的长度
volatile uint8_t recv_end_flag = 0; //接收结束标志位

//不要再中断函数中使用！！
void uart1_send_string(uint8_t *tdata){
	//等待发送状态OK
  while(HAL_DMA_GetState(&hdma_usart1_tx) == HAL_DMA_STATE_BUSY) HAL_Delay(1);
  //发送数据
  HAL_UART_Transmit_DMA(&huart1,tdata,strlen((char*)tdata));
}

//不要再中断函数中使用！！
void uart1_send_data(uint8_t *tdata,uint16_t tnum){
	//等待发送状态OK
  while(HAL_DMA_GetState(&hdma_usart1_tx) == HAL_DMA_STATE_BUSY) HAL_Delay(1);
  //发送数据
  HAL_UART_Transmit_DMA(&huart1,tdata,tnum);
}

//开启uart1 DMA收发
void uart1_start_dma(void){
	HAL_UART_Receive_DMA(&huart1,rx_buffer,200);//开启DMA
}



/*
ATTENTION:
1.中断函数中不能写printf和malloc
2.cJson_Delete必须写在最后，如果写在前面会导致条件判断结果出错，原因不明
*/

void parse_msg(uint8_t* msg){
		
		
	cJSON* obj = cJSON_Parse((char*)msg);
	
	char* type = cJSON_GetObjectItem(obj, "type")->valuestring;
	
	
	//判断是否为command
	if(strcmp(type, "command") == 0){
		cJSON* command = cJSON_GetObjectItem(obj, "command");
		
		char* variable = cJSON_GetObjectItem(command, "variable")->valuestring;
		uint8_t value = cJSON_GetObjectItem(command, "value")->valueint;
		
		set_cfg(variable, value);
		
		
	}
	
	
	if( strcmp(type, "request") == 0){
		char* variable = cJSON_GetObjectItem(obj, "variable")->valuestring;
		req_cfg(variable);
		
		
	}
	

	
	cJSON_Delete(obj);


}


void set_cfg(char* variable, uint8_t value){
	
		/*对tempCfg进行修改，并放入队列中*/
	if(strcmp(variable, "coffee") == 0)
		SetNextCfg(coffee, value);
	if(strcmp(variable,"milk") == 0)
		SetNextCfg(milk, value);
	if(strcmp(variable,"sugar") == 0)
		SetNextCfg(sugar, value);
	if(strcmp(variable,"temp") == 0)
		SetNextCfg(temp, value);
		
	AddBuffer();
	
	
	SetCurrentCfg();
	
	sprintf(rx_log, "already set!\ncoffee, milk, sugar, temp = %d, %d, %d, %d\r\n", GetCurrentCfg(coffee),GetCurrentCfg(milk), GetCurrentCfg(sugar), GetCurrentCfg(temp));
	HAL_UART_Transmit_DMA(&huart1, rx_log, strlen((char*)rx_log));
	
}


void req_cfg(char* variable){
	uint8_t value;
	
	if(strcmp(variable, "coffee") == 0)
		value = GetCurrentCfg(coffee);
	if(strcmp(variable,"milk") == 0)
		value = GetCurrentCfg(milk);
	if(strcmp(variable,"sugar") == 0)
		value = GetCurrentCfg(sugar);
	if(strcmp(variable,"temp") == 0)
		value = GetCurrentCfg(temp);
	
	sprintf(rx_log, "value of %s is %d\n", variable, value);
	HAL_UART_Transmit_DMA(&huart1, rx_log, strlen((char*)rx_log));
	
}



