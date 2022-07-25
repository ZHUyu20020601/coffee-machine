#include "connect.h"
#include "string.h"
#include "stdio.h"

extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;

uint8_t data_buf[200];
uint8_t received_data[1];
uint8_t top = 0;

void my_uart1_send_string(uint8_t *tdata){
	//�ȴ�����״̬OK
  while(HAL_DMA_GetState(&hdma_usart1_tx) == HAL_DMA_STATE_BUSY) HAL_Delay(1);
  //��������
  HAL_UART_Transmit_DMA(&huart1,tdata,strlen((char*)tdata));
}

void my_uart1_send_data(uint8_t *tdata,uint16_t tnum){
	//�ȴ�����״̬OK
  while(HAL_DMA_GetState(&hdma_usart1_tx) == HAL_DMA_STATE_BUSY) HAL_Delay(1);
  //��������
  HAL_UART_Transmit_DMA(&huart1,tdata,tnum);
}

//�������ڽ����ж�
void my_uart1_enable_interrupt(void){
	//����һ���ж�
  //HAL_UART_Receive_IT(&huart1,&my_uart1_redata,1);
  HAL_UART_Receive_DMA(&huart1,received_data,1);//���ý��ջ�����
 
}

//�����յ����ݻص�
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	if(huart->Instance == USART1)//�жϴ��ں�
	{
		//����
    if(top > 199 || received_data[0] == '$'){
			my_uart1_send_data(data_buf,top);
			//printf("flag is %d\r\n",flag);
			//printf("top is %d\r\n",top);
			top = 0;
		}
		else{
			data_buf[top++] = received_data[0];
						
		}

    //����һ���ж�
    HAL_UART_Receive_IT(&huart1,received_data,1);
				//HAL_UARTEx_ReceiveToIdle_IT(&huart1, received_data, 30) ;      //���¿������ڽ��� 
  }
}

