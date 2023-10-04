/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_http_client.h"

#define LED_PIN 2
#define myDelay(x) vTaskDelay(x / portTICK_PERIOD_MS);

static const char *TAG = "wifi_example";

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Wi-Fi started");
        esp_wifi_connect();
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Wi-Fi disconnected");
        esp_wifi_connect();
    } else if (event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(TAG, "got IP:" IPSTR, IP2STR(&event->ip_info.ip));
    } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI(TAG, "Wi-Fi connected");
    }
}



void wifi_init_sta()
{
    nvs_flash_init();
    esp_netif_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "Wokwi-GUEST",
            .password = "",
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void send_data_to_thingspeak(void *pvParameters) {
    static const char *api_key = "WOYYUBP110P72K88";
    char thingspeak_url[200];
    esp_err_t err;

    while (1) {
        myDelay(16000);

        // Generate dummy data (replace with actual data)
        int field1_data = 10;  // Replace with your sensor data
        int field2_data = 15;  // Replace with your sensor data
        int field3_data = 20;  // Replace with your sensor data

        // Construct the URL with the data and your API key
        snprintf(thingspeak_url, sizeof(thingspeak_url),
                 "https://api.thingspeak.com/update?api_key=%s&field1=%d&field2=%d&field3=%d",
                 api_key, field1_data, field2_data, field3_data);

        esp_http_client_config_t config = {
            .url = thingspeak_url,
            .method = HTTP_METHOD_GET,  // Use HTTP GET method
        };

        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");

        err = esp_http_client_perform(client);

        if (err == ESP_OK) {
            int status_code = esp_http_client_get_status_code(client);
            if (status_code == 200) {
                ESP_LOGI(TAG, "Message sent Successfully");
            } else {
                ESP_LOGI(TAG, "Message sent Failed");
            }
        } else {
            ESP_LOGI(TAG, "HTTP GET request failed");
        }

        esp_http_client_cleanup(client);
    }
}

// exit:
//     esp_http_client_cleanup(client);
//     vTaskDelete(NULL);
// }


void app_main(void) {
    wifi_init_sta();
    xTaskCreate(&send_data_to_thingspeak, "send_data_to_thingspeak", 8192, NULL, 6, NULL); // Create the task for sending data

    // // int y = 1;
    // // gpio_reset_pin(LED_PIN);
    // // gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // // ESP_LOGE("ALEX-TAG", "hj %d japp", y);

    // // while(1){
    // //     y++;
    // //     ESP_LOGI("MAIN", "check WIFI %d moss", y);
    // //     gpio_set_level(LED_PIN, 1);
    // //     myDelay(1000);
    // //     //vTaskDelay(1000 / portTICK_PERIOD_MS);
    // //     gpio_set_level(LED_PIN, 0);
    // //     //vTaskDelay(1000 / portTICK_PERIOD_MS);
    // //     myDelay(1000);

    // }

}
