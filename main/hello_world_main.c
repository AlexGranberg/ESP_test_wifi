/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define LED_PIN 2
#define myDelay(x) vTaskDelay(x / portTICK_PERIOD_MS);

void app_main(void) {
    int y = 1;
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    ESP_LOGE("ALEX-TAG", "hj %d japp", y);

    while(1){
        y++;
        ESP_LOGI("MAIN", "TESTARigendårå %d moss", y);
        gpio_set_level(LED_PIN, 1);
        myDelay(1000);
        //vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN, 0);
        //vTaskDelay(1000 / portTICK_PERIOD_MS);
        myDelay(1000);

    }

}
