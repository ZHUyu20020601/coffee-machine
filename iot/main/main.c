#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

// 一些量
int coffee = 10, milk = 20, sugar = 30, current_temp = 40;
#include "../_components/uart_component.h"
#include "../_components/mqtt_component.h"

#define ESP_WIFI_SSID CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASS CONFIG_ESP_WIFI_PASSWORD

static EventGroupHandle_t s_wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

const char* TAG = "Coffee Machine";

void nvs_init() 
{
    //Initialize NVS
    // NVS -- 非易失性存储库
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

//void nvs_insert_str(const char* content, const char* key)
//{
//    // if exist, do not write. If not exist, write into nvs
//    nvs_handle_t nvs_handle;
//    esp_err_t err = nvs_open("mqtt_cfg", NVS_READWRITE, &nvs_handle);
//    char* out_value = (char*)malloc(128);
//    err = nvs_get_str(nvs_handle, key, out_value, strlen(content));
//    switch (err) {
//    case ESP_OK:
//        // 该值存在
//        return;
//    case ESP_ERR_NVS_NOT_FOUND:
//        // 该值不存在，写入
//        ESP_ERROR_CHECK(nvs_set_str(nvs_handle, key, content));
//        ESP_ERROR_CHECK(nvs_commit(nvs_handle));
//    }
//    ESP_ERROR_CHECK(err);
//    nvs_close(nvs_handle);
//}

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    }else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED){
        ESP_LOGI(TAG, "Retry connecting to AP");
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(TAG, "Got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void station_init()
{
    // 初始化WIFI Station
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &event_handler,
        NULL,
        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &event_handler,
        NULL,
        &instance_got_ip));
    wifi_sta_config_t wifi_sta_config = {};
    wifi_config_t wifi_config = {
        .sta = wifi_sta_config,
    };
    strlcpy((char*)wifi_config.sta.ssid, ESP_WIFI_SSID, sizeof(ESP_WIFI_SSID));
    strlcpy((char*)wifi_config.sta.password, ESP_WIFI_PASS, sizeof(ESP_WIFI_PASS));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_wifi_set_ps(WIFI_PS_NONE);

    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s", ESP_WIFI_SSID, ESP_WIFI_PASS);
}

void app_main(void)
{
    nvs_init();
    station_init();
    // get basic information like device_id, project_id, etc -- from aliyun server
    // create a task receiving and sending properties with platform
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, 0, 0, portMAX_DELAY);
    xTaskCreate(mqtt_loop, "mqtt", 20480, NULL, 2, NULL);
    uart_start();
    while (1) {
        vTaskDelay(100);
    }
}
