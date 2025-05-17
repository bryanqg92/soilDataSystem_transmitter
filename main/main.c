#include "app.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"

#define RX_PIN GPIO_NUM_33 // Cambiar según tu configuración
#define TX_PIN GPIO_NUM_34 // Cambiar según tu configuración
#define UART_NUM UART_NUM_1

static void init_log_config(esp_log_level_t level);

static const char* MAIN_TAG = "[MAIN]";

void app_main(void)
{

    init_log_config(ESP_LOG_INFO);
    ESP_LOGI(MAIN_TAG, "Iniciando aplicación");
    app_init();
}

static void init_log_config(esp_log_level_t level)
{
    esp_log_level_set("*", level);
    ESP_LOGE("TEST", "Esto es ERROR");
    ESP_LOGW("TEST", "Esto es WARNING");
    ESP_LOGI("TEST", "Esto es INFO");
    ESP_LOGD("TEST", "Esto es DEBUG");
    ESP_LOGV("TEST", "Esto es VERBOSE");
}