#include "includes.h"


extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart3_tx;
extern DMA_HandleTypeDef hdma_usart3_rx;

extern uint8_t send_buf[200];

extern int DEBUG;



uint8_t rx_buffer[200];   //接收数据的数组
uint8_t rx_log[50];   //日志数组
volatile uint8_t rx_len = 0; //接收数据的长度
volatile uint8_t recv_end_flag = 0; //接收结束标志位

uint8_t rx_buffer_2[200];   //接收数据的数组
uint8_t rx_log_2[50];   //日志数组
volatile uint8_t rx_len_2 = 0; //接收数据的长度
volatile uint8_t recv_end_flag_2 = 0; //接收结束标志位

//for usart3
uint8_t rx_buffer_3[200];   //接收数据的数组
uint8_t rx_log_3[50];   //日志数组
volatile uint8_t rx_len_3 = 0; //接收数据的长度
volatile uint8_t recv_end_flag_3 = 0; //接收结束标志位




//开启uart1 DMA收发
void uart1_start_dma(void){
	HAL_UART_Receive_DMA(&huart1,rx_buffer,200);//开启DMA
}

void uart2_start_dma(void){
	HAL_UART_Receive_DMA(&huart2,rx_buffer_2,200);//开启DMA
}

void uart3_start_dma(void){
	HAL_UART_Receive_DMA(&huart3,rx_buffer_3,200);//开启DMA
}




/*
FUNCTION:
解析收到的对象，作相应的操作

ATTENTION:
1.中断函数中不能写printf和malloc
2.cJson_Delete必须写在最后，如果写在前面会导致条件判断结果出错，原因不明
*/

void parse_msg(uint8_t* msg){
	
	
		
	cJSON* obj = cJSON_Parse((char*)msg);//解析对象
	
	char* type = cJSON_GetObjectItem(obj, "type")->valuestring;//分析命令类型
	uint8_t id = cJSON_GetObjectItem(obj, "id")->valueint;//解析命令号码
	
	
	//判断是否为command
	if(strcmp(type, "command") == 0){
		cJSON* command = cJSON_GetObjectItem(obj, "command");
		
		char* variable = cJSON_GetObjectItem(command, "variable")->valuestring;
		uint8_t value = cJSON_GetObjectItem(command, "value")->valueint;
		
		set_cfg(variable, value, id);
	}
	
	//判断是否为request
	if( strcmp(type, "request") == 0){
		char* variable = cJSON_GetObjectItem(obj, "variable")->valuestring;
		req_cfg(variable, id);
	}
	
	//判断是否为start
	if( strcmp(type, "start") == 0){
		start(id);
	}
	
	//判断是否为emergent stop
	if( strcmp(type, "emergent stop") == 0){
		emergent_stop(id);
	}
	
	//销毁临时变量，节约内存
	cJSON_Delete(obj);


}


void set_cfg(char* variable, uint8_t value, uint8_t id){
	
	char* msg = NULL;
	
		/*对tempCfg进行修改，并放入队列中*/
	if(strcmp(variable, "coffee") == 0)
		SetNextCfg(coffee, value);
	if(strcmp(variable,"milk") == 0)
		SetNextCfg(milk, value);
	if(strcmp(variable,"sugar") == 0)
		SetNextCfg(sugar, value);
	if(strcmp(variable,"temp") == 0)
		SetNextCfg(temp, value);
	
		
	if(msg == NULL)
		response_ok(id);
	else
		response_error(id, msg);

}


void req_cfg(char* variable, uint8_t id){
	uint8_t value;
		
	if(strcmp(variable, "coffee") == 0)
		value = GetEnviCfg(coffee);
	if(strcmp(variable,"milk") == 0)
		value = GetEnviCfg(milk);
	if(strcmp(variable,"sugar") == 0)
		value = GetEnviCfg(sugar);
	if(strcmp(variable,"temp") == 0)
		value = GetEnviCfg(temp);
	
	response_request(id, variable, value);
	
}


void start(uint8_t id){
	
	//为防止连续两次执行start，放在if中
	if(GetSystemStatus() != Making){
	//读取队列中的参数
		char* msg = SetCurrentCfg();
	
		if(msg != NULL){
			response_error(id, msg);
			return;
		}
		
		SetStatusMaking();
		
		response_status(id);
	}
	
	
}

void emergent_stop(uint8_t id){
	
	if(GetSystemStatus() != Error){
		SetStatusError();
	}
	
	response_status(id);
}





/*----返回----*/
void response_ok(uint8_t id){

	cJSON* cjson = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson, "type", "response");
	cJSON_AddNumberToObject(cjson, "id", id);
	
	cJSON* response = cJSON_CreateObject();
	cJSON_AddNumberToObject(response, "status", 0);
	cJSON_AddStringToObject(response, "msg", "command execute successfully");
	
	cJSON_AddItemToObject(cjson, "result", response);
	
	
	char* temp = cJSON_Print(cjson);
	strcpy((char*)send_buf, temp);
	
	free(temp);
	cJSON_Delete(cjson);
	
}
	
	
void response_making(uint8_t id){

	cJSON* cjson = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson, "type", "response");
	cJSON_AddNumberToObject(cjson, "id", id);
	
	cJSON* response = cJSON_CreateObject();
	cJSON_AddNumberToObject(response, "status", 1);
	cJSON_AddStringToObject(response, "msg", "waiting for finishing");
	
	cJSON_AddItemToObject(cjson, "result", response);
	
	char* temp = cJSON_Print(cjson);
	strcpy((char*)send_buf, temp);
	
	free(temp);
	cJSON_Delete(cjson);                      
}


void response_error(uint8_t id, char* msg){

	cJSON* cjson = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson, "type", "response");
	cJSON_AddNumberToObject(cjson, "id", id);
	
	cJSON* response = cJSON_CreateObject();
	cJSON_AddNumberToObject(response, "status", 2);
	cJSON_AddStringToObject(response, "msg", msg);
	
	cJSON_AddItemToObject(cjson, "result", response);
	
	char* temp = cJSON_Print(cjson);
	strcpy((char*)send_buf, temp);
	
	free(temp);
	cJSON_Delete(cjson);
	

}

void response_request(uint8_t id, char* variable, uint8_t value){

	cJSON* cjson = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson, "type", "variable");
	cJSON_AddNumberToObject(cjson, "id", id);
	
	cJSON* response = cJSON_CreateObject();
	cJSON_AddStringToObject(response, "variable", variable);
	cJSON_AddNumberToObject(response, "value", value);
	
	cJSON_AddItemToObject(cjson, "result", response);
	
	

	char* temp = cJSON_Print(cjson);
	strcpy((char*)send_buf, temp);
	
	
	free(temp);
	cJSON_Delete(cjson);

}

void response_status(uint8_t id){
	cJSON* cjson = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson, "type", "status");
	cJSON_AddNumberToObject(cjson, "id", id);
	
	if(GetSystemStatus() == Waiting)
		cJSON_AddStringToObject(cjson, "status", "waiting");
	else if(GetSystemStatus() == Making)
		cJSON_AddStringToObject(cjson, "status", "making");
	else
		cJSON_AddStringToObject(cjson, "status", "error");

	char* temp = cJSON_Print(cjson);
	strcpy((char*)send_buf, temp);
	
	
	free(temp);
	cJSON_Delete(cjson);

}

