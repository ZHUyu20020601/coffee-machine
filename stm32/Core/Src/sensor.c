#include "includes.h"

//filter size
const static int average_span = 50;

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim6;
extern onewire tempSensor;

float get_coffee_dist(void){
	int i;
	Kalman_Filter_reset();
	for(i = 0 ; i < average_span ; ++i){
		coffee_trig_send();
		rtU.Input = (real_T)distance(coffee_time_cap());
		Kalman_Filter_step();
		HAL_Delay(20);//wait 20ms
		//osDelay(20);
	}
	return rtY.Output;
}

float get_milk_dist(void){
	int i;
	//float dist = 0;
	Kalman_Filter_reset();
	for(i = 0 ; i < average_span ; ++i){
		milk_trig_send();
		rtU.Input = (real_T)distance(milk_time_cap());
		Kalman_Filter_step();
		HAL_Delay(20);//wait 20ms
	}
	return rtY.Output;
}

float get_sugar_dist(void){
	int i;
	//float dist = 0;
	Kalman_Filter_reset();
	for(i = 0 ; i < average_span ; ++i){
		sugar_trig_send();
		rtU.Input = (real_T)distance(sugar_time_cap());
		Kalman_Filter_step();
		HAL_Delay(20);//wait 20ms
	}
	return rtY.Output;
}
	

void coffee_trig_send(void){
	//set high level to send signal
	HAL_GPIO_WritePin(coffee_trigger_GPIO_Port, coffee_trigger_Pin, GPIO_PIN_RESET);
	HAL_Delay_us(2);
	HAL_GPIO_WritePin(coffee_trigger_GPIO_Port, coffee_trigger_Pin, GPIO_PIN_SET);
	//keep sending
	HAL_Delay_us(20);
	//stop
	HAL_GPIO_WritePin(coffee_trigger_GPIO_Port, coffee_trigger_Pin, GPIO_PIN_RESET);
}

void milk_trig_send(void){
	HAL_GPIO_WritePin(milk_trigger_GPIO_Port, milk_trigger_Pin, GPIO_PIN_RESET);
	HAL_Delay_us(2);
	HAL_GPIO_WritePin(milk_trigger_GPIO_Port, milk_trigger_Pin, GPIO_PIN_SET);
	HAL_Delay_us(20);
	HAL_GPIO_WritePin(milk_trigger_GPIO_Port, milk_trigger_Pin, GPIO_PIN_RESET);
}

void sugar_trig_send(void){
	HAL_GPIO_WritePin(sugar_trigger_GPIO_Port, sugar_trigger_Pin, GPIO_PIN_RESET);
	HAL_Delay_us(2);
	HAL_GPIO_WritePin(sugar_trigger_GPIO_Port, sugar_trigger_Pin, GPIO_PIN_SET);
	HAL_Delay_us(20);
	HAL_GPIO_WritePin(sugar_trigger_GPIO_Port, sugar_trigger_Pin, GPIO_PIN_RESET);
}

//get time span must within 65536us
uint32_t coffee_time_cap(void){
	
	uint32_t tim;
	
	//wait for callback
	while(HAL_GPIO_ReadPin(coffee_echo_GPIO_Port, coffee_echo_Pin) == GPIO_PIN_RESET);
	__HAL_TIM_SetCounter(&htim6,0);

	HAL_TIM_Base_Start(&htim6);

	while(HAL_GPIO_ReadPin(coffee_echo_GPIO_Port, coffee_echo_Pin) == GPIO_PIN_SET);
	HAL_TIM_Base_Stop(&htim6);
	
	tim = __HAL_TIM_GetCounter(&htim6);
	return tim;
	
	
}

uint32_t milk_time_cap(void){
	uint32_t tim;
	//wait for callback
	while(HAL_GPIO_ReadPin(milk_echo_GPIO_Port, milk_echo_Pin) == GPIO_PIN_RESET);
	__HAL_TIM_SetCounter(&htim6,0);
	HAL_TIM_Base_Start(&htim6);
	while(HAL_GPIO_ReadPin(milk_echo_GPIO_Port, milk_echo_Pin) == GPIO_PIN_SET);
	HAL_TIM_Base_Stop(&htim6);
	tim = __HAL_TIM_GetCounter(&htim6);
	return tim;
}

uint32_t sugar_time_cap(void){
	uint32_t tim;
	//wait for callback
	while(HAL_GPIO_ReadPin(sugar_echo_GPIO_Port, sugar_echo_Pin) == GPIO_PIN_RESET);
	__HAL_TIM_SetCounter(&htim6,0);
	HAL_TIM_Base_Start(&htim6);
	while(HAL_GPIO_ReadPin(sugar_echo_GPIO_Port, sugar_echo_Pin) == GPIO_PIN_SET);
	HAL_TIM_Base_Stop(&htim6);
	tim = __HAL_TIM_GetCounter(&htim6);
	return tim;
}

float distance(uint32_t us){
	float dist;
	//dist = (340 * us) / 10000 / 2; // cm
	dist = us * 0.017;
	return dist;
}


/*----------temperature----------*/

float read_temp(void){
	//ds18b20
	float temp = ds18b20_readtemperature(&tempSensor) / 100.0;
	while(temp > 100 || temp < 23){//ignore unvalidate data
		temp = ds18b20_readtemperature(&tempSensor) / 100.0;
		osDelay(pdMS_TO_TICKS(10));
	}
	return temp;
	
	//红外
	/*
	int16_t pobj = -1, pabm = -1;
	uint8_t result = UartReadTemp(&pobj, &pabm);
	if(result != 0){
		return pobj;
	}
	*/
	//printf("%d\n",result);
	//printf("TEMP ERROR\n");
	//return -1;
}

int ds18b20_readtemperature(onewire *ptr){
	unsigned char lo, hi;
	int x = 0;
	onewire_reset(ptr);
	onewire_writebyte(ptr, 0xcc);//read serial
	onewire_writebyte(ptr, 0x44);//enable temperature
	osDelay(800);
	onewire_reset(ptr);
	onewire_writebyte(ptr, 0xcc);
	onewire_writebyte(ptr, 0xbe);
	lo = onewire_readbyte(ptr);
	hi = onewire_readbyte(ptr);
	x = lo | (hi << 8);
	x = (int)(x * 0.0625 * 100 + 0.5);
	return x;
}


/* External functions --------------------------------------------------------*/



/**
 *  发送命令
 *  @param cmd  命令
 *  @param data 参数内容
 *  @param len  参数长度
 */
/*
void UartSendCmd(uint8_t cmd, uint8_t *parg, uint8_t len) {
  uint8_t chk = 0, i, buf[16];

  // 包头
  buf[0] = 0xAA;
  buf[1] = 0xA5;

  // 包长
  buf[2] = len + 3;

  // 包命令
  buf[3] = cmd;

  // 包内容
  for (i = 0; i < len; i++) {
    chk += parg[i];
    buf[4 + i] = parg[i];
  }
  // 校验和
  chk = chk + cmd + len + 3;
  buf[4 + len] = chk;
  // 包尾
  buf[5 + len] = 0x55;
  HAL_UART_Transmit(&huart2, buf, 6 + len, 1000);
  //HAL_UART_Transmit_DMA(&huart2, buf, 6+len);
}
*/

/**
 * @brief 读传感器温度
 *
 * @param pobj 返回目标温度
 * @param pamb 返回环境温度
 * @return uint8_t 返回测温结果，1为物温，2为体温，0为无应答
 */
 /*
uint8_t UartReadTemp(int16_t *pobj, int16_t *pamb) {
  uint8_t type = 0;

  rx_len_2 = 0;

  UartSendCmd(0x01, NULL, 0);

  // 延时等待接收完成
  osDelay(pdMS_TO_TICKS(300));
	//printf("%d\n", rx_len_2);
	
  if (rx_len_2 == 11 && rx_buffer_2[3] == 0x01){
    type = rx_buffer_2[4];
    *pobj = (rx_buffer_2[5] << 8) | (rx_buffer_2[6]);
    *pamb = (rx_buffer_2[7] << 8) | (rx_buffer_2[8]);
  }

  return type;
}
*/

/**
 * @brief 设置测量模式
 *
 * @param mode 1为物温，2为体温
 */
 /*
void UartSetTempMode(uint8_t mode) {
  uint8_t arg = mode;
  UartSendCmd(0x02, &arg, 1);
}
*/


/**
 * @brief 设定模块通信方式
 *
 */
 /*
void UartSetCommType(uint8_t type) {
  uint8_t arg = type;
  UartSendCmd(0x05, &arg, 1);
}
*/








