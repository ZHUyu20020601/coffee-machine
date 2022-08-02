#include "connect.h"
#include "string.h"
#include "stdio.h"
#include "sys.h"

extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;

//extern SystemCfg tempCfg;


uint8_t rx_buffer[200];   //�������ݵ�����
uint8_t rx_log[50];   //��־����
volatile uint8_t rx_len = 0; //�������ݵĳ���
volatile uint8_t recv_end_flag = 0; //���ս�����־λ




//��Ҫ���жϺ�����ʹ�ã���
void uart1_send_string(uint8_t *tdata){
	//�ȴ�����״̬OK
  while(HAL_DMA_GetState(&hdma_usart1_tx) == HAL_DMA_STATE_BUSY) HAL_Delay(1);
  //��������
  HAL_UART_Transmit_DMA(&huart1,tdata,strlen((char*)tdata));
}

//��Ҫ���жϺ�����ʹ�ã���
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



/*
FUNCTION:
�����յ��Ķ�������Ӧ�Ĳ���

ATTENTION:
1.�жϺ����в���дprintf��malloc
2.cJson_Delete����д��������д��ǰ��ᵼ�������жϽ������ԭ����
*/

void parse_msg(uint8_t* msg){
		
		
	cJSON* obj = cJSON_Parse((char*)msg);//��������
	
	char* type = cJSON_GetObjectItem(obj, "type")->valuestring;//������������
	uint8_t id = cJSON_GetObjectItem(obj, "id")->valueint;//�����������
	
	
	//�ж��Ƿ�Ϊcommand
	if(strcmp(type, "command") == 0){
		cJSON* command = cJSON_GetObjectItem(obj, "command");
		
		char* variable = cJSON_GetObjectItem(command, "variable")->valuestring;
		uint8_t value = cJSON_GetObjectItem(command, "value")->valueint;
		
		set_cfg(variable, value, id);
		
		
	}
	
	//�ж��Ƿ�Ϊrequest
	if( strcmp(type, "request") == 0){
		char* variable = cJSON_GetObjectItem(obj, "variable")->valuestring;
		req_cfg(variable, id);
		
		
	}
	
	//�ж��Ƿ�Ϊstart
	if( strcmp(type, "start") == 0){
		start(id);
	}
	
	//�ж��Ƿ�Ϊemergent stop
	if( strcmp(type, "emergent stop") == 0){
		emergent_stop(id);
	}

	
	cJSON_Delete(obj);


}


void set_cfg(char* variable, uint8_t value, uint8_t id){
	
	char* msg = NULL;
	
		/*��tempCfg�����޸ģ������������*/
	if(strcmp(variable, "coffee") == 0)
		SetNextCfg(coffee, value);
	if(strcmp(variable,"milk") == 0)
		SetNextCfg(milk, value);
	if(strcmp(variable,"sugar") == 0)
		SetNextCfg(sugar, value);
	if(strcmp(variable,"temp") == 0)
		SetNextCfg(temp, value);
	if(strcmp(variable, "addbuf") == 0)
			msg = AddBuffer();
		
		
	//AddBuffer();
	
	
	//SetCurrentCfg();
	//sprintf(rx_log, "already set!\ncoffee, milk, sugar, temp = %d, %d, %d, %d\r\n", GetCurrentCfg(coffee),GetCurrentCfg(milk), GetCurrentCfg(sugar), GetCurrentCfg(temp));
	
	if(msg == NULL)
		response_ok(id);
	else
		response_error(id, msg);
	
	//HAL_UART_Transmit_DMA(&huart1, rx_log, strlen((char*)rx_log));
	
}

//ֻ��start�������Ч����ȡ����currentCfg�����ܻ��buf�е�����
void req_cfg(char* variable, uint8_t id){
	uint8_t value;
	
	if(strcmp(variable, "coffee") == 0)
		value = GetCurrentCfg(coffee);
	if(strcmp(variable,"milk") == 0)
		value = GetCurrentCfg(milk);
	if(strcmp(variable,"sugar") == 0)
		value = GetCurrentCfg(sugar);
	if(strcmp(variable,"temp") == 0)
		value = GetCurrentCfg(temp);
	
	//sprintf(rx_log, "value of %s is %d\n", variable, value);
	//HAL_UART_Transmit_DMA(&huart1, rx_log, strlen((char*)rx_log));
	
	response_request(id, variable, value);
	
}


void start(uint8_t id){
	//response_ok(id);
	
	//��ȡ�����еĲ���
	char* msg = SetCurrentCfg();
	
	if(msg != NULL){
		response_error(id, msg);
		return;
	}
	
	
	SetStatusMaking();
	/*
	����������������Ȼ��Ľ��̴���
	*/
	SetStatusWaiting();
	
	response_status(id);
	
	
}

void emergent_stop(uint8_t id){
	SetStatusError();
	/*
	��������뿧�Ȼ�����ͣ���Ĵ���
	*/
	response_status(id);
}



/*----����----*/
void response_ok(uint8_t id){
	//����Ŀ�����
	cJSON* cjson = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson, "type", "response");
	cJSON_AddNumberToObject(cjson, "id", id);
	
	cJSON* response = cJSON_CreateObject();
	cJSON_AddNumberToObject(response, "status", 0);
	cJSON_AddStringToObject(response, "msg", "command execute successfully");
	
	cJSON_AddItemToObject(cjson, "result", response);
	
	//���ݸ�log�ַ���
	strcpy(rx_log, cJSON_Print(cjson));
	
	//���ٶ���
	cJSON_Delete(cjson);
	
	//����
	HAL_UART_Transmit_DMA(&huart1, rx_log, strlen((char*)rx_log));
}


void response_making(uint8_t id){
		//����Ŀ�����
	cJSON* cjson = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson, "type", "response");
	cJSON_AddNumberToObject(cjson, "id", id);
	
	cJSON* response = cJSON_CreateObject();
	cJSON_AddNumberToObject(response, "status", 1);
	cJSON_AddStringToObject(response, "msg", "waiting for finishing");
	
	cJSON_AddItemToObject(cjson, "result", response);
	
	//���ݸ�log�ַ���
	strcpy(rx_log, cJSON_Print(cjson));
	
	//���ٶ���
	cJSON_Delete(cjson);
	
	//����
	HAL_UART_Transmit_DMA(&huart1, rx_log, strlen((char*)rx_log));
}


void response_error(uint8_t id, char* msg){
	//����Ŀ�����
	cJSON* cjson = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson, "type", "response");
	cJSON_AddNumberToObject(cjson, "id", id);
	
	cJSON* response = cJSON_CreateObject();
	cJSON_AddNumberToObject(response, "status", 2);
	cJSON_AddStringToObject(response, "msg", msg);
	
	cJSON_AddItemToObject(cjson, "result", response);
	
	//���ݸ�log�ַ���
	strcpy(rx_log, cJSON_Print(cjson));
	
	//���ٶ���
	cJSON_Delete(cjson);
	
	//����
	HAL_UART_Transmit_DMA(&huart1, rx_log, strlen((char*)rx_log));
}

void response_request(uint8_t id, char* variable, uint8_t value){
	//����Ŀ�����
	cJSON* cjson = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson, "type", "variable");
	cJSON_AddNumberToObject(cjson, "id", id);
	
	cJSON* response = cJSON_CreateObject();
	cJSON_AddStringToObject(response, "variable", variable);
	cJSON_AddNumberToObject(response, "value", value);
	
	cJSON_AddItemToObject(cjson, "result", response);
	
	//���ݸ�log�ַ���
	strcpy(rx_log, cJSON_Print(cjson));
	
	//���ٶ���
	cJSON_Delete(cjson);
	
	//����
	HAL_UART_Transmit_DMA(&huart1, rx_log, strlen((char*)rx_log));
	
}

void response_status(uint8_t id){
	//����Ŀ�����
	cJSON* cjson = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson, "type", "status");
	cJSON_AddNumberToObject(cjson, "id", id);
	
	if(GetSystemStatus() == Waiting)
		cJSON_AddStringToObject(cjson, "status", "waiting");
	else if(GetSystemStatus() == Making)
		cJSON_AddStringToObject(cjson, "status", "making");
	else
		cJSON_AddStringToObject(cjson, "status", "error");

	
	//���ݸ�log�ַ���
	strcpy(rx_log, cJSON_Print(cjson));
	
	//���ٶ���
	cJSON_Delete(cjson);
	
	//����
	HAL_UART_Transmit_DMA(&huart1, rx_log, strlen((char*)rx_log));
}

