#ifndef MQTT_COMPONENT_H
#define MQTT_COMPONENT_H
#include <string.h>
#include <stdlib.h>
#include "mqtt_client.h"
#include "cJSON.h"

//��Ϊ����������
#define server_ip "mqtt://39.106.12.138"
#define device_id "CoffeeMachine"
#define user_name "neworld2020"
#define pwd "774225688"
#define mqtt_port 1883
// �����㷨����

//�����ϱ�Topic---����
#define PROPERTY_REPORT_TOPIC device_id "/property/report"
//�������Topic---����
#define COMMAND_RECV_TOPIC device_id "/command/exe"
//�������ý���Topic---����
#define PROPERTY_SET_RECV_TOPIC device_id "/property/set"

#define MQTT_TAG "MQTT"

esp_mqtt_client_handle_t client;
char topic[100];
char msg[200];

// �ַ����Һ���
const char* find_char(const char* str, char c)
{
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == c) {
            return &str[i + 1];
        }
    }
    return NULL;
}

// ��mqtt����ת������STM32��ͨ������
char* command_adapter(const char* origin)
{
    /*add_coffee -> coffee, add_sugar -> sugar, add_milk -> milk, target_temp -> temp
    */
    if (strcmp(origin, "add_coffee") == 0) {
        return "coffee";
    }
    else if (strcmp(origin, "add_sugar") == 0) {
        return "sugar";
    }
    else if (strcmp(origin, "add_milk") == 0) {
        return "milk";
    }
    else if (strcmp(origin, "target_temp") == 0) {
        return "temp";
    }
    else {
        // ��Ӧ�ó��ֵ����
        ESP_LOGE(MQTT_TAG, "A Unexpected Command is Recved: %s", origin);
        esp_restart();
    }
}

// �쳣��������
static void log_error_if_nonzero(const char* message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(MQTT_TAG, "Last error %s: 0x%x", message, error_code);
    }
}

// ��������ID
int alloc_task_id()
{
    static int id = 0;
    return id++;
}

// ������stm32 -- ��ʱʹ��
void send_to_stm32(const char* msg)
{
    ESP_LOGI(MQTT_TAG, "send to stm32: %s", msg);
    extern char TX_CONTENT[1024];
    strcpy(TX_CONTENT, msg);
}

void mqtt_message_handler()
{
    ESP_LOGI("HANDLER", "topic=%s", topic);
    ESP_LOGI("HANDLER", "msg=%s", msg);
    if (strcmp(find_char(topic, '/'), "property/set") == 0) {
        // �������ã���body��ʽ���£�
        /*{
            "property": property_name,
            "value": property_value
        }*/
        ESP_LOGI(MQTT_TAG, "json: %s", msg);
        cJSON* root = cJSON_Parse(msg);
        ESP_LOGI(MQTT_TAG, "msg:%s\n", msg);
        char* property_name = cJSON_GetObjectItem(root, "property")->valuestring;
        ESP_LOGI(MQTT_TAG, "property_name:%s\n", property_name);
        int property_value = cJSON_GetObjectItem(root, "value")->valueint;
        ESP_LOGI(MQTT_TAG, "property_value:%d\n", property_value);
        char* command_variable = command_adapter(property_name);
        ESP_LOGI(MQTT_TAG, "command_variable:%s", command_variable);
        // ������stm32
        int id = alloc_task_id();
        ESP_LOGI(MQTT_TAG, "id:%d", id);
        cJSON* ToStm32 = cJSON_CreateObject();
        cJSON_AddStringToObject(ToStm32, "type", "command");
        cJSON_AddNumberToObject(ToStm32, "id", id);
        cJSON* command = cJSON_CreateObject();
        cJSON_AddStringToObject(command, "variable", command_variable);
        cJSON_AddNumberToObject(command, "value", property_value);
        cJSON_AddItemToObject(ToStm32, "command", command);
        send_to_stm32(cJSON_Print(ToStm32));
        ESP_LOGI(MQTT_TAG, "%s: %d", property_name, property_value);
    }
    else if (strcmp(find_char(topic, '/'), "command/exe") == 0) {
        // ִ�������body�ṹ���£�
        /*{
        "command": command_name
        }*/
        cJSON* root = cJSON_Parse(msg);
        char* command = cJSON_GetObjectItem(root, "command")->valuestring;
        cJSON* ToStm32 = cJSON_CreateObject();
        int id = alloc_task_id();
        if (strcmp(command, "StartMaking") == 0) {
            cJSON_AddStringToObject(ToStm32, "type", "start");
        }
        else if (strcmp(command, "EmergentStop") == 0) {
            cJSON_AddStringToObject(ToStm32, "type", "emergent stop");
        }
        cJSON_AddNumberToObject(ToStm32, "id", id);
        send_to_stm32(cJSON_Print(ToStm32));
        ESP_LOGI(MQTT_TAG, "Command: %s", command);
    }
}

// MQTT �¼��ص�����
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_t* event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id = 0;
    // your_context_t *context = event->context;
    switch (event->event_id) {
    case MQTT_EVENT_CONNECTED:
        //��������
        msg_id = esp_mqtt_client_subscribe(client, COMMAND_RECV_TOPIC, 1);
        msg_id = esp_mqtt_client_subscribe(client, PROPERTY_SET_RECV_TOPIC, 1);
        ESP_LOGI(MQTT_TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        
        ESP_LOGI(MQTT_TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DATA");
        // �ַ�����β����0
        strlcpy(topic, event->topic, event->topic_len + 1);
        strlcpy(msg, event->data, event->data_len+1);
        printf("TOPIC=%s\r\n", topic);
        printf("DATA=%s\r\n", msg);
        mqtt_message_handler(topic, msg);
        // ���ͷ���
        //send_feedback(event->topic);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(MQTT_TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    case MQTT_EVENT_BEFORE_CONNECT:
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_BEFORE_CONNECT");
        break;
    default:
        ESP_LOGI(MQTT_TAG, "Other event id:%d", event->event_id);
        break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void* handler_args,
    esp_event_base_t base,
    int32_t event_id,
    void* event_data)
{
    ESP_LOGD(MQTT_TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb((esp_mqtt_event_t*)event_data);
}

void create_mqtt_client()
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = server_ip,
        .port = mqtt_port,
        .client_id = device_id,
        .username = user_name,
        .password = pwd,
        .disable_auto_reconnect = false,
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, client);
    esp_err_t result = esp_mqtt_client_start(client);
    ESP_LOGI(MQTT_TAG, "Here");
    while (result != ESP_OK)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        result = esp_mqtt_client_start(client);
    }
}


// �ϱ�����Topic����
void mqttIntervalPost(esp_mqtt_client_handle_t client)
{
    extern int coffee, milk, sugar, current_temp;
    // �ϱ�����
    cJSON* report = cJSON_CreateObject();
    cJSON_AddStringToObject(report, "device_id", device_id);
    // �ϱ�ʣ�࿧����
    cJSON_AddStringToObject(report, "property_name", "coffee_rest");
    cJSON_AddNumberToObject(report, "value", coffee);
    char* json_Buf = cJSON_Print(report);
    esp_mqtt_client_publish(client, PROPERTY_REPORT_TOPIC, json_Buf, strlen(json_Buf), 0, 0);
    // �ϱ�ʣ��ţ�̵���
    cJSON_DeleteItemFromObject(report, "property_name");
    cJSON_DeleteItemFromObject(report, "value");
    cJSON_AddStringToObject(report, "property_name", "milk_rest");
    cJSON_AddNumberToObject(report, "value", milk);
    json_Buf = cJSON_Print(report);
    esp_mqtt_client_publish(client, PROPERTY_REPORT_TOPIC, json_Buf, strlen(json_Buf), 0, 0);
    // �ϱ�ʣ����ˮ����
    cJSON_DeleteItemFromObject(report, "property_name");
    cJSON_DeleteItemFromObject(report, "value");
    cJSON_AddStringToObject(report, "property_name", "sugar_rest");
    cJSON_AddNumberToObject(report, "value", sugar);
    json_Buf = cJSON_Print(report);
    esp_mqtt_client_publish(client, PROPERTY_REPORT_TOPIC, json_Buf, strlen(json_Buf), 0, 0);
    // �ϱ���ǰ�¶�
    cJSON_DeleteItemFromObject(report, "property_name");
    cJSON_DeleteItemFromObject(report, "value");
    cJSON_AddStringToObject(report, "property_name", "current_temp");
    cJSON_AddNumberToObject(report, "value", current_temp);
    json_Buf = cJSON_Print(report);
    esp_mqtt_client_publish(client, PROPERTY_REPORT_TOPIC, json_Buf, strlen(json_Buf), 0, 0);
}


void mqtt_loop(void* pv)
{
    create_mqtt_client();
    while (1)
    {
        mqttIntervalPost(client);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

#endif // !MQTT_COMPONENT_H



