#ifndef MQTT_COMPONENT_H
#define MQTT_COMPONENT_H
#include <string.h>
#include <stdlib.h>
#include "mqtt_client.h"
#include "sha256.h"
#include "hmac_sha256.h"
#include "cJSON.h"

//华为云上云条件
#define server_ip "mqtt://a16199eec3.iot-mqtts.cn-north-4.myhuaweicloud.com"
#define device_id "62d00d586b9813541d5166a2_0001"
#define device_secret "coffee_machine"
#define this_time "2022071415"
#define pwd "aff85855e54e9dee83218e4c78b1f0489d9e35c53ab775cec97d72d5dbeca965"
#define mqtt_port 1883
// 加密算法配置
#define SHA256HMAC_SIZE 256
#define MAX_SIZE 64

//属性上报Topic---发布
#define HUAWEI_TOPIC_PROP_POST "$oc/devices/" device_id "/sys/properties/report"
//命令接收Topic---订阅
#define HUAWEI_TOPIC_PROP_SET "$oc/devices/" device_id "/sys/commands/#"
    //设备报警信息---发布与订阅
#define HUAWEI_TOPIC_PROP_ERROR "$oc/devices/" device_id "/user/error"
//命令反馈信息
#define HUAWEI_COMMAND_FEEDBACK "$oc/devices/" device_id "/sys/commands/response/request_id="

#define MQTT_TAG "MQTT"

esp_mqtt_client_handle_t client;

// 加密算法
static int hmac256(const char* signcontent, int length, char* sign)
{
    uint8_t hashCode[SHA256HMAC_SIZE];
    hmac_sha256(this_time, strlen(this_time), signcontent, strlen(signcontent), hashCode,
        sizeof(hashCode));
    int c = 0;
    for (uint8_t i = 0; i < SHA256HMAC_SIZE; ++i)
    {
        sign[c] = "0123456789abcdef"[hashCode[i] >> 4]; c++;
        sign[c] = "0123456789abcdef"[hashCode[i] & 0xf]; c++;
    }// dec -> hex
    return 1;
}

// 异常处理函数
static void log_error_if_nonzero(const char* message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(MQTT_TAG, "Last error %s: 0x%x", message, error_code);
    }
}

void mqtt_command_handler(const char* command)
{
    // convert char* to json
    cJSON* root = cJSON_Parse(command);
    cJSON* paras = cJSON_GetObjectItem(root, "paras");
    char* cmd_name = cJSON_GetObjectItem(root, "command_name")->valuestring;
    ESP_LOGI(MQTT_TAG, "Command name:%s", cmd_name);
    if (strcmp(cmd_name, "set-add-coffee") == 0) {
        // 设置咖啡加入的量
        int coffee = cJSON_GetObjectItem(paras, "coffee")->valueint;
        ESP_LOGI(MQTT_TAG, "To STM32: Coffee %d ml", coffee);
    }
    else if (strcmp(cmd_name, "set-add-milk") == 0) {
        int milk = cJSON_GetObjectItem(paras, "milk")->valueint;
        ESP_LOGI(MQTT_TAG, "To STM32: Milk %d ml", milk);
    }
    else if (strcmp(cmd_name, "set-add-sugar") == 0) {
        int sugar = cJSON_GetObjectItem(paras, "sugar")->valueint;
        ESP_LOGI(MQTT_TAG, "To STM32: Sugar %d ml", sugar);
    }
}

const char* find_char(const char* str, char c)
{
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == c) {
            return &str[i+1];
        }
    }
    return NULL;
}

void send_feedback(const char* topic)
{
    // 发送反馈
    char* dest = (char*)malloc(200);
    memset(dest, 0, 200);
    const char* request_id = find_char(topic, '=');
    if (request_id == NULL) {
        ESP_LOGE(MQTT_TAG, "未找到字符串");
    }
    strcat(dest, HUAWEI_COMMAND_FEEDBACK);
    strcat(dest, request_id);
    ESP_LOGI(MQTT_TAG, "%s", dest);
    esp_mqtt_client_publish(client, dest, NULL, 0, 0, 0);
    free(dest);
}

// MQTT 事件回调函数
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_t* event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id = 0;
    // your_context_t *context = event->context;
    switch (event->event_id) {
    case MQTT_EVENT_CONNECTED:
        //订阅命令
        msg_id = esp_mqtt_client_subscribe(client, HUAWEI_TOPIC_PROP_SET, 1);
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
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        mqtt_command_handler(event->data);
        // 发送反馈
        send_feedback(event->topic);
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

void create_huaweiyun_mqtt_client()
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = server_ip,
        .port = mqtt_port,
        .client_id = (device_id "_0_0_" this_time),
        .username = device_id,
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


// 上报属性Topic数据
int mqttIntervalPost(esp_mqtt_client_handle_t client)
{
    /*char jsonBuf[200];
    memset(jsonBuf, 0, 200);
    
    int msg_id = esp_mqtt_client_publish(client, HUAWEI_TOPIC_PROP_POST, jsonBuf, 0, 0, 0);
    return msg_id;*/
    return 0;
}


void mqtt_loop(void* pv)
{
    create_huaweiyun_mqtt_client();
    esp_mqtt_client_subscribe(client, HUAWEI_TOPIC_PROP_SET, 0);
    while (1)
    {
        mqttIntervalPost(client);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

#endif // !MQTT_COMPONENT_H



