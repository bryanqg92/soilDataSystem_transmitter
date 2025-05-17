#include "power_manager.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "app.h"

#define TAG " **** POWER_MANAGER"


void enter_low_power_mode(void)
{
    ESP_LOGI(TAG, "Preparando para entrar en deep sleep...");

    gpio_set_direction(VEXT_CTRL, GPIO_MODE_OUTPUT);
    gpio_set_level(VEXT_CTRL, 0); 

    esp_sleep_enable_ext1_wakeup(
        (1ULL << CAT0_BUTTON_GPIO) | (1ULL << CAT1_BUTTON_GPIO),
        ESP_EXT1_WAKEUP_ANY_LOW);

    ESP_LOGI(TAG, "Deep sleep activado. Esperando doble botón presionado para despertar...");

    // Entrar en deep sleep
    esp_deep_sleep_start();
}
