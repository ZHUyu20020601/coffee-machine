#ifndef STM32_COMMUNICATE_H
#define STM32_COMMUNICATE_H
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "cJSON.h"

#define MAX_TASKS 100

// UART通信
static const int RX_BUF_SIZE = 1024;
#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_5)

static void uart_recv_data_task(void* pv);
void uart_init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .source_clk = UART_SCLK_APB,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    xTaskCreate(uart_recv_data_task, "recv task", 1024 * 2, NULL, configMAX_PRIORITIES - 1, NULL);
}

static int uart_send_data(const char* content)
{
    const int len = strlen(content);
    const int tx_bytes = uart_write_bytes(UART_NUM_1, content, len);
    ESP_LOGI("UART", "Wrote %d bytes", tx_bytes);
    return tx_bytes;
}

xSemaphoreHandle data_mutex; // 保护cp_data这个两个线程共用的资源
xTaskHandle data_handler;
static void recv_data_handler(void* pv);
static void uart_recv_data_task(void* pv)
{
    // will be a task always running
    data_mutex = xSemaphoreCreateMutex();
    static const char* RECV_TAG = "RX";
    esp_log_level_set(RECV_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*)malloc(RX_BUF_SIZE + 1);
    uint8_t* cp_data = (uint8_t*)malloc(RX_BUF_SIZE + 1);
    while (1) {
        const int rx_bytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        if (rx_bytes > 0) {
            data[rx_bytes] = 0;
            ESP_LOGI(RECV_TAG, "Read %d bytes", rx_bytes);
            ESP_LOG_BUFFER_HEXDUMP(RECV_TAG, data, rx_bytes, ESP_LOG_INFO);
            xSemaphoreTake(data_mutex, portMAX_DELAY);
            strlcpy(cp_data, data, rx_bytes);
            xSemaphoreGive(data_mutex);
            xTaskCreate(recv_data_handler, "receive data handler", 1024, (void*)cp_data, configMAX_PRIORITIES - 2, &data_handler);
        }
    }
    free(data);
    free(cp_data);
}

static void recv_data_handler(void* pv)
{
    xSemaphoreTake(data_mutex, portMAX_DELAY);
    const char* data = (char*)pv;
    cJSON* response = cJSON_Parse(data);
    xSemaphoreGive(data_mutex);
    const char* type = cJSON_GetObjectItem(response, "type")->valuestring;
    if (strcmp(type, "response") == 0) {
        uint8_t id = (uint8_t)cJSON_GetObjectItem(response, "id")->valueint;
        cJSON* result = cJSON_GetObjectItem(response, "result");
        uint8_t status = (uint8_t)cJSON_GetObjectItem(result, "status")->valueint;
        const char* msg = cJSON_GetObjectItem(result, "msg")->valuestring;
        if (status == 0) {
            // succeed
            TaskStatus[id] = 0;
        }
        else if (status == 2)
        {
            ESP_LOGE("UART", "%s", msg);
        }
    }
    else if (strcmp(type, "variable") == 0)
    {
        uint8_t id = (uint8_t)cJSON_GetObjectItem(response, "id")->valueint;
        TaskStatus[id] = 0;
        cJSON* result = cJSON_GetObjectItem(response, "result");
        const char* variable = cJSON_GetObjectItem(result, "variable");
        const uint8_t value = (uint8_t)cJSON_GetObjectItem(result, "value");
        // TODO mqtt发送至客户端
        ;
    }
    else if (strcmp(type, "status") == 0) {
        ;
    }
    else {
        ESP_LOGE("UART", "Unable to recognize response from STM32");
    }
    vTaskDelete(data_handler);
}

// 任务ID分配
uint8_t TaskStatus[MAX_TASKS];
static uint8_t allocate_task_id()
{
    static uint8_t id = 0;
    // 0 - task succeed; 1 - task waiting
    for (uint8_t i = 0; i < MAX_TASKS; i++)
    {
        if (id >= MAX_TASKS) {
            id %= MAX_TASKS;
        }
        if (TaskStatus[id] == 0) {
            TaskStatus[id] = 1;
            return id;
        }
        else {
            id++;
        }
    }
    // reach the maximum, allocate failed
    return -1;
}

// APIs
void send_cmd_to_stm32(const char* set_variable, uint8_t value)
{
    uint8_t id = allocate_task_id();
    cJSON* cmd = cJSON_CreateObject();
    cJSON_AddStringToObject(cmd, "type", "command");
    cJSON_AddNumberToObject(cmd, "id", id);
    cJSON* cmd_content = cJSON_CreateObject();
    cJSON_AddStringToObject(cmd_content, "variable", set_variable);
    cJSON_AddNumberToObject(cmd_content, "value", value);
    cJSON_AddItemToObject(cmd, "command", cmd_content);
    const char* cmd_str = cJSON_Print(cmd);
    // uart send to stm32
    ESP_LOGI("UART", "%s", cmd_str);
    uart_send_data(cmd_str);
}
void send_request_to_stm32(const char* get_variable)
{
    uint8_t id = allocate_task_id();
    cJSON* req = cJSON_CreateObject();
    cJSON_AddStringToObject(req, "type", "request");
    cJSON_AddNumberToObject(req, "id", id);
    cJSON_AddStringToObject(req, "variable", get_variable);
    const char* req_str = cJSON_Print(req);
    ESP_LOGI("UART", "%s", req_str);
    uart_send_data(req_str);
}
void send_start_to_stm32()
{
    uint8_t id = allocate_task_id();
    cJSON* start = cJSON_CreateObject();
    cJSON_AddStringToObject(start, "type", "start");
    cJSON_AddNumberToObject(start, "id", id);
    const char* start_str = cJSON_Print(start);
    ESP_LOGI("UART", "%s", start_str);
    uart_send_data(start_str);
}
void send_stop_to_stm32()
{
    uint8_t id = allocate_task_id();
    cJSON* stop = cJSON_CreateObject();
    cJSON_AddStringToObject(stop, "type", "emergent stop");
    cJSON_AddNumberToObject(stop, "id", id);
    const char* stop_str = cJSON_Print(stop);
    ESP_LOGI("UART", "%s", stop_str);
    uart_send_data(stop_str);
}

#endif // !STM32_COMMUNICATE_H
