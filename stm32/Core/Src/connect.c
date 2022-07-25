#include "connect.h"
#include "string.h"
#include "stdio.h"

extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;

uint8_t rx_buffer[200];   //接收数据的数组
volatile uint8_t rx_len = 0; //接收数据的长度
volatile uint8_t recv_end_flag = 0; //接收结束标志位


void uart1_send_string(uint8_t *tdata){
	//等待发送状态OK
  while(HAL_DMA_GetState(&hdma_usart1_tx) == HAL_DMA_STATE_BUSY) HAL_Delay(1);
  //发送数据
  HAL_UART_Transmit_DMA(&huart1,tdata,strlen((char*)tdata));
}

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



void parse_msg(uint8_t* msg){
	cJSON* obj = cJSON_Parse((char*)msg);
	cJSON* command = cJSON_GetObjectItem(obj, "command");
	char* variable = cJSON_GetObjectItem(command, "variable")->string;
	uint8_t value = cJSON_GetObjectItem(command, "value")->valueint;
	
	if(strcmp(variable,"coffee")==0)
		SetNextCfg(coffee, value);
	if(strcmp(variable,"milk")==0)
		SetNextCfg(milk, value);
	if(strcmp(variable,"sugar")==0)
		SetNextCfg(sugar, value);
	if(strcmp(variable,"temp")==0)
		SetNextCfg(temp, value);
	AddBuffer();
	SetCurrentCfg();
	printf("coffee, milk, sugar, temp = %d %d %d %d\r\n", 
					GetCurrentCfg(coffee),GetCurrentCfg(milk), GetCurrentCfg(sugar), GetCurrentCfg(temp));
}




