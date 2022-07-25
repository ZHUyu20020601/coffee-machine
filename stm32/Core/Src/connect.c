#include "connect.h"
#include "string.h"
#include "stdio.h"

extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;

uint8_t data_buf[200];
uint8_t received_data[1];
uint8_t top = 0;

void my_uart1_send_string(uint8_t *tdata){
	//等待发送状态OK
  while(HAL_DMA_GetState(&hdma_usart1_tx) == HAL_DMA_STATE_BUSY) HAL_Delay(1);
  //发送数据
  HAL_UART_Transmit_DMA(&huart1,tdata,strlen((char*)tdata));
}

void my_uart1_send_data(uint8_t *tdata,uint16_t tnum){
	//等待发送状态OK
  while(HAL_DMA_GetState(&hdma_usart1_tx) == HAL_DMA_STATE_BUSY) HAL_Delay(1);
  //发送数据
  HAL_UART_Transmit_DMA(&huart1,tdata,tnum);
}

//开启串口接收中断
void my_uart1_enable_interrupt(void){
	//开启一次中断
  //HAL_UART_Receive_IT(&huart1,&my_uart1_redata,1);
  HAL_UART_Receive_DMA(&huart1,received_data,1);//设置接收缓冲区
 
}

//串口收到数据回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	if(huart->Instance == USART1)//判断串口号
	{
		//发送
    if(top > 199 || received_data[0] == '$'){
			my_uart1_send_data(data_buf,top);
			//printf("flag is %d\r\n",flag);
			//printf("top is %d\r\n",top);
			top = 0;
		}
		else{
			data_buf[top++] = received_data[0];
						
		}

    //开启一次中断
    HAL_UART_Receive_IT(&huart1,received_data,1);
				//HAL_UARTEx_ReceiveToIdle_IT(&huart1, received_data, 30) ;      //重新开启串口接收 
  }
}

