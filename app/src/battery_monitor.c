#include "battery_monitor.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BATTERY_CTRL_GPIO    GPIO_NUM_2
#define BATTERY_ADC_CHANNEL  ADC1_CHANNEL_0  // GPIO1
#define DEFAULT_VREF         1100            // mV
#define NO_OF_SAMPLES        64

bool sleep_mode = false;
static esp_adc_cal_characteristics_t adc_chars;

void battery_monitor_init(void)
{
    // Configurar ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(BATTERY_ADC_CHANNEL, ADC_ATTEN_DB_11);
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);

    // Configurar pin de control del divisor (GPIO2)
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BATTERY_CTRL_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 0,
        .pull_up_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

battery_level_t battery_monitor_update(void)
{
    gpio_set_level(BATTERY_CTRL_GPIO, 1); // Habilita divisor
    vTaskDelay(pdMS_TO_TICKS(10));       // Espera para estabilización

    uint32_t adc_reading = 0;
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        adc_reading += adc1_get_raw(BATTERY_ADC_CHANNEL);
    }
    adc_reading /= NO_OF_SAMPLES;

    gpio_set_level(BATTERY_CTRL_GPIO, 0); // Deshabilita divisor si deseas

    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, &adc_chars);
    float vbat = voltage * 4.9f / 1000.0f;

    //ESP_LOGI("BATTERY", "ADC: %d | mV: %d | VBAT: %.2f V", adc_reading, voltage, vbat);

    // En battery_monitor.c, donde determinas el nivel:
    if (vbat <= 3.30f) {
        sleep_mode = true;
        return BATTERY_EMPTY;
    } else if (vbat < 3.40f) {
        return BATTERY_VERY_LOW;
    } else if (vbat < 3.50f) {
        return BATTERY_LOW;
    } else if (vbat < 3.70f) {
        return BATTERY_MEDIUM;
    } else {
        return BATTERY_FULL;
    }

    
}
