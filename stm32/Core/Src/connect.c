#include "connect.h"
#include "string.h"
#include "stdio.h"

extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;

uint8_t rx_buffer[200];   //�������ݵ�����
volatile uint8_t rx_len = 0; //�������ݵĳ���
volatile uint8_t recv_end_flag = 0; //���ս�����־λ


void uart1_send_string(uint8_t *tdata){
	//�ȴ�����״̬OK
  while(HAL_DMA_GetState(&hdma_usart1_tx) == HAL_DMA_STATE_BUSY) HAL_Delay(1);
  //��������
  HAL_UART_Transmit_DMA(&huart1,tdata,strlen((char*)tdata));
}

void uart1_send_data(uint8_t *tdata,uint16_t tnum){
	//�ȴ�����״̬OK
  while(HAL_DMA_GetState(&hdma_usart1_tx) == HAL_DMA_STATE_BUSY) HAL_Delay(1);
  //��������
  HAL_UART_Transmit_DMA(&huart1,tdata,tnum);
}

//����uart1 DMA�շ�
void uart1_start_dma(void){
	HAL_UART_Receive_DMA(&huart1,rx_buffer,200);//����DMA
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




