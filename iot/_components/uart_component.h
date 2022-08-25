#ifndef UART_COMPONENT_H
#define UART_COMPONENT_H
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "cJSON.h"

static const int BUF_SIZE = 1024;
char TX_CONTENT[1024];

#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_5)

void init(void) 
{
	const uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.source_clk = UART_SCLK_APB
	};
	// we use a buffer to store sending data, 
	// and a buffer to receive data
	uart_driver_install(UART_NUM_1, BUF_SIZE, BUF_SIZE, 0, NULL, 0);
	uart_param_config(UART_NUM_1, &uart_config);
	uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void rcv_data_handler(const uint8_t* data)
{
	extern int coffee, milk, sugar, current_temp;
	// 解析stm32返回的信息
	cJSON* response = cJSON_Parse((const char*)data);
	/**
	* {
			"type": "variable",
			"id": 0,
			"result": {
				"variable": "coffee/...",
				"value": 100
			}
		}
	*/
	
	char* type = cJSON_GetObjectItem(response, "type")->valuestring;
	if (strcmp(type, "variable") == 0) {
		// stm32返回的是esp32对变量的请求值
		int id = cJSON_GetObjectItem(response, "id")->valueint;
		cJSON* result = cJSON_GetObjectItem(response, "result");
		char* variable = cJSON_GetObjectItem(result, "variable")->valuestring;
		int value = cJSON_GetObjectItem(result, "value")->valueint;
		if (strcmp(variable, "coffee") == 0) {
			coffee = value;
		}
		else if (strcmp(variable, "milk") == 0) {
			milk = value;
		}
		else if (strcmp(variable, "sugar") == 0) {
			sugar = value;
		}
		else if (strcmp(variable, "current_temp") == 0) {
			current_temp = value;
		}
		else {
			// 不可能
			esp_restart();
		}
	}
}

static void tx_task(void* pv)
{
	static const char* TX_TASK_TAG = "TX_TASK";
	esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
	while (1) {
		// 发送数据
		if (strlen(TX_CONTENT) > 0) {
			ESP_LOGI(TX_TASK_TAG, "send: %s", TX_CONTENT);
			const int len = strlen(TX_CONTENT);
			uart_write_bytes(UART_NUM_1, TX_CONTENT, len);
			memset((void*)TX_CONTENT, 0, 1024);
		}
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}

static void rx_task(void* pv)
{
	// 接收任务
	static const char* RX_TASK_TAG = "RX_TASK";
	esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
	uint8_t* data = (uint8_t*)malloc(BUF_SIZE + 1);
	while (1)
	{
		const int rxBytes = uart_read_bytes(UART_NUM_1, (void*)data, BUF_SIZE, 1000 / portTICK_RATE_MS);
		if (rxBytes > 0) {
			// 补上终止符
			data[rxBytes] = 0;
		}
		// uart 接收信号处理
		if (data[0] == '{') {
			ESP_LOGI(RX_TASK_TAG, "data:%s", data);
			rcv_data_handler(data);
		}
	}
	free(data);
}

void uart_start(void) {
	init();
	xTaskCreate(rx_task, "uart_rx_task", 1024 * 2, NULL, 1, NULL);
	xTaskCreate(tx_task, "uart_tx_task", 1024 * 2, NULL, 1, NULL);
}

#endif // !UART_COMPONENT_H
