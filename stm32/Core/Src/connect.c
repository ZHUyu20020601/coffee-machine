#include "connect.h"
#include "string.h"
#include "stdio.h"
#include "sys.h"
#include "FreeRtos.h"
#include "cmsis_os.h"
#include "task.h"

extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern osThreadId_t makingTaskHandle;

extern int DEBUG;

//extern SystemCfg tempCfg;


uint8_t rx_buffer[200];   //接收数据的数组
uint8_t rx_log[50];   //日志数组
volatile uint8_t rx_len = 0; //接收数据的长度
volatile uint8_t recv_end_flag = 0; //接收结束标志位

//for usart3
uint8_t rx_buffer_3[200];   //接收数据的数组
uint8_t rx_log_3[50];   //日志数组
volatile uint8_t rx_len_3 = 0; //接收数据的长度
volatile uint8_t recv_end_flag_3 = 0; //接收结束标志位




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

void uart3_start_dma(void){
	HAL_UART_Receive_DMA(&huart3,rx_buffer,200);//开启DMA
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
	if(strcmp(variable, "addbuf") == 0)
			msg = AddBuffer();
		
		
	if(msg == NULL)
		response_ok(id);
	else
		response_error(id, msg);

}


void req_cfg(char* variable, uint8_t id){
	uint8_t value;
	
	
//每次都读取的是currentCfg，不能获得buf中的内容
//如果从未addbuf和start，即没有进行过setcurrentcfg，那么currentcfg将是刚刚初始化的状态
	/*
	if(strcmp(variable, "coffee") == 0)
		value = GetCurrentCfg(coffee);
	if(strcmp(variable,"milk") == 0)
		value = GetCurrentCfg(milk);
	if(strcmp(variable,"sugar") == 0)
		value = GetCurrentCfg(sugar);
	if(strcmp(variable,"temp") == 0)
		value = GetCurrentCfg(temp);
	*/
	
//每次都只读取tempCfg中的参数，对于已经存入队列的参数以及currentcfg则无法读取	
	if(strcmp(variable, "coffee") == 0)
		value = GetTempCfg(coffee);
	if(strcmp(variable,"milk") == 0)
		value = GetTempCfg(milk);
	if(strcmp(variable,"sugar") == 0)
		value = GetTempCfg(sugar);
	if(strcmp(variable,"temp") == 0)
		value = GetTempCfg(temp);
	
	
	
	//sprintf(rx_log, "value of %s is %d\n", variable, value);
	//HAL_UART_Transmit_DMA(&huart1, rx_log, strlen((char*)rx_log));
	
	response_request(id, variable, value);
	
}


void start(uint8_t id){
	//response_ok(id);
	
	//为防止连续两次执行start，放在if中
	if(GetSystemStatus() != Making){
	//读取队列中的参数
		char* msg = SetCurrentCfg();
	
		if(msg != NULL){
			response_error(id, msg);
			return;
		}
	}
	
	
	if(GetSystemStatus() != Making)
		SetStatusMaking();

	
	response_status(id);
	
	
}

void emergent_stop(uint8_t id){
	
	if(GetSystemStatus() != Error){
		SetStatusError();
	}
	
	response_status(id);
}





/*----返回----*/
void response_ok(uint8_t id){
	//生成目标对象
	cJSON* cjson = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson, "type", "response");
	cJSON_AddNumberToObject(cjson, "id", id);
	
	cJSON* response = cJSON_CreateObject();
	cJSON_AddNumberToObject(response, "status", 0);
	cJSON_AddStringToObject(response, "msg", "command execute successfully");
	
	cJSON_AddItemToObject(cjson, "result", response);
	
	
	//发送
	if(DEBUG){
		//传递给log字符串
		strcpy(rx_log, cJSON_Print(cjson));
		HAL_UART_Transmit_DMA(&huart1, rx_log, strlen((char*)rx_log));
	}
	else{
		//传递给log字符串
		strcpy(rx_log_3, cJSON_Print(cjson));
		HAL_UART_Transmit_DMA(&huart3, rx_log, strlen((char*)rx_log_3));
	}
	
	//销毁对象
	cJSON_Delete(cjson);
	
}
	
	
void response_making(uint8_t id){
		//生成目标对象
	cJSON* cjson = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson, "type", "response");
	cJSON_AddNumberToObject(cjson, "id", id);
	
	cJSON* response = cJSON_CreateObject();
	cJSON_AddNumberToObject(response, "status", 1);
	cJSON_AddStringToObject(response, "msg", "waiting for finishing");
	
	cJSON_AddItemToObject(cjson, "result", response);
	
	if(DEBUG){
		//传递给log字符串
		strcpy(rx_log, cJSON_Print(cjson));
	
		//发送
		HAL_UART_Transmit_DMA(&huart1, rx_log, strlen((char*)rx_log));
	}
	else{
		//传递给log字符串
		strcpy(rx_log_3, cJSON_Print(cjson));
	
		//发送
		HAL_UART_Transmit_DMA(&huart3, rx_log_3, strlen((char*)rx_log));
	}
	
	
	//销毁对象
	cJSON_Delete(cjson);
}


void response_error(uint8_t id, char* msg){
	//生成目标对象
	cJSON* cjson = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson, "type", "response");
	cJSON_AddNumberToObject(cjson, "id", id);
	
	cJSON* response = cJSON_CreateObject();
	cJSON_AddNumberToObject(response, "status", 2);
	cJSON_AddStringToObject(response, "msg", msg);
	
	cJSON_AddItemToObject(cjson, "result", response);
	
	if(DEBUG){
		strcpy(rx_log, cJSON_Print(cjson));
		HAL_UART_Transmit_DMA(&huart1, rx_log, strlen((char*)rx_log));
	}
	else{
		strcpy(rx_log_3, cJSON_Print(cjson));
		HAL_UART_Transmit_DMA(&huart3, rx_log_3, strlen((char*)rx_log));
	}
	
	
	//销毁对象
	cJSON_Delete(cjson);
	

}

void response_request(uint8_t id, char* variable, uint8_t value){
	//生成目标对象
	cJSON* cjson = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson, "type", "variable");
	cJSON_AddNumberToObject(cjson, "id", id);
	
	cJSON* response = cJSON_CreateObject();
	cJSON_AddStringToObject(response, "variable", variable);
	cJSON_AddNumberToObject(response, "value", value);
	
	cJSON_AddItemToObject(cjson, "result", response);
	
	
	if(DEBUG){
		strcpy(rx_log, cJSON_Print(cjson));
		HAL_UART_Transmit_DMA(&huart1, rx_log, strlen((char*)rx_log));
	}
	else{
		strcpy(rx_log_3, cJSON_Print(cjson));
		HAL_UART_Transmit_DMA(&huart3, rx_log_3, strlen((char*)rx_log));
	}
		
	//销毁对象
	cJSON_Delete(cjson);

}

void response_status(uint8_t id){
	//生成目标对象
	cJSON* cjson = cJSON_CreateObject();
	cJSON_AddStringToObject(cjson, "type", "status");
	cJSON_AddNumberToObject(cjson, "id", id);
	
	if(GetSystemStatus() == Waiting)
		cJSON_AddStringToObject(cjson, "status", "waiting");
	else if(GetSystemStatus() == Making)
		cJSON_AddStringToObject(cjson, "status", "making");
	else
		cJSON_AddStringToObject(cjson, "status", "error");

	
	if(DEBUG){
		strcpy(rx_log, cJSON_Print(cjson));
		HAL_UART_Transmit_DMA(&huart1, rx_log, strlen((char*)rx_log));
	}
	else{
		strcpy(rx_log_3, cJSON_Print(cjson));
		HAL_UART_Transmit_DMA(&huart3, rx_log_3, strlen((char*)rx_log));
	}
	
	//销毁对象
	cJSON_Delete(cjson);

}

