#include "buttons_manager.h"
#include "app.h"
#include "battery_monitor.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lora_manager.h"
#include "power_manager.h"
#include "shared_data.h"
const char* BUTTONS = "==> [BUTTONS]";

#define LONG_PRESS_TIME 1000

void buttons_init(button_app_t* cat0_btn, button_app_t* cat1_btn)
{
    // Configure GPIO pins
    gpio_config_t io_conf = {.pin_bit_mask =
                                 (1ULL << CAT0_BUTTON_GPIO) | (1ULL << CAT1_BUTTON_GPIO),
                             .mode = GPIO_MODE_INPUT,
                             .pull_up_en = GPIO_PULLUP_ENABLE,
                             .pull_down_en = GPIO_PULLDOWN_DISABLE,
                             .intr_type = GPIO_INTR_DISABLE};

    gpio_config(&io_conf);

    cat0_btn->debounceTime = cat1_btn->debounceTime = DEBOUNCE_BTN_TIME;

    cat0_btn->gpio = CAT0_BUTTON_GPIO;
    cat1_btn->gpio = CAT1_BUTTON_GPIO;

    debounceFSM_init(cat0_btn);
    debounceFSM_init(cat1_btn);
    ESP_LOGI(BUTTONS, "Buttons initialized successfully");
}

void Task_buttons(void* pvParameters)
{
    buttons_params_t* buttons = (buttons_params_t*)pvParameters;
    TickType_t pressStartTimeCat0 = 0;
    TickType_t pressStartTimeCat1 = 0;

    while (true)
    {
        // Actualizar el estado de los botones
        buttonFSM_update(buttons->cat0_btn);
        buttonFSM_update(buttons->cat1_btn);

        // Verificar si CAT0 está presionado y CAT1 no
        if (BUTTON_DOWN == buttons->cat0_btn->buttonState &&
            BUTTON_UP == buttons->cat1_btn->buttonState)
        {
            if (pressStartTimeCat0 == 0)
            {
                pressStartTimeCat0 = xTaskGetTickCount();
            }
            else if ((xTaskGetTickCount() - pressStartTimeCat0) >= pdMS_TO_TICKS(LONG_PRESS_TIME))
            {
                if (ESP_OK != capture_and_send_data(false))
                {
                    ESP_LOGE(BUTTONS, "Error al enviar datos de categoría 0");
                }
                else
                {
                    ESP_LOGD(BUTTONS, "Datos enviados correctamente");
                }
                pressStartTimeCat0 = 0;
            }
        }
        else if (BUTTON_DOWN == buttons->cat1_btn->buttonState &&
                 BUTTON_UP == buttons->cat0_btn->buttonState)
        {
            if (pressStartTimeCat1 == 0)
            {
                pressStartTimeCat1 = xTaskGetTickCount();
            }
            else if ((xTaskGetTickCount() - pressStartTimeCat1) >= pdMS_TO_TICKS(LONG_PRESS_TIME))
            {
                if (ESP_OK != capture_and_send_data(true))
                {
                    ESP_LOGE(BUTTONS, "Error al enviar datos de categoría 1");
                }
                else
                {
                    ESP_LOGD(BUTTONS, "Datos enviados correctamente");
                }
                pressStartTimeCat1 = 0; // Reset press start time
            }
        }
        else if ((BUTTON_DOWN == buttons->cat1_btn->buttonState &&
                  BUTTON_DOWN == buttons->cat0_btn->buttonState) ||
                 sleep_mode == true)
        {
            if (pressStartTimeCat1 == 0)
            {
                pressStartTimeCat1 = xTaskGetTickCount();
            }
            else if ((xTaskGetTickCount() - pressStartTimeCat1) >= pdMS_TO_TICKS(LONG_PRESS_TIME))
            {
                pressStartTimeCat1 = 0; // Reset press start time
                enter_low_power_mode();
            }
        }
        else
        {
            pressStartTimeCat1 = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

esp_err_t capture_and_send_data(bool category)
{

    SoilData_t soilData;
    GNSSData_t gnssData;

    if (xQueueReceive(xQueueSoilData, &soilData, portMAX_DELAY) != pdTRUE)
    {
        ESP_LOGE(BUTTONS, "Error al recibir datos de suelo");
        return ESP_FAIL;
    }

    if (xQueueReceive(xQueueGNSSData, &gnssData, portMAX_DELAY) != pdTRUE)
    {
        ESP_LOGE(BUTTONS, "Error al recibir datos de GNSS");
        return ESP_FAIL;
    }

    // Enviar datos por LoRa
    if (lora_send_all_data(&lora_config, &soilData, &gnssData, category) != ESP_OK)
    {
        ESP_LOGE(BUTTONS, "Error al enviar datos por LoRa");
        return ESP_FAIL;
    }

    ESP_LOGD(BUTTONS, "Datos enviados correctamente");
    return ESP_OK;
}