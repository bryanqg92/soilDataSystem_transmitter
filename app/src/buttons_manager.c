#include "buttons_manager.h"
#include "app.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "logger.h"
#include "sd_card_api.h"
#include "sd_spi_handler.h"
#include "shared_data.h"

#define FILE_PATH "/sdcard/npk_data.csv"
#define FILE_HEADER "N,P,K,M,T,EC,pH,alt,lat,lon,datetime,category\n"

const char* BUTTONS = "==> BUTTONS";

static char data_buffer[100];
static void sd_card_init(void);
static void parse_data_to_string(bool category);

sd_card_t card;
sd_config_t sd_config;

GNSSData_t gnss_task_data;
SoilData_t soil_task_data;

void buttons_init(button_app_t* cat0_btn, button_app_t* cat1_btn)
{
    sd_card_init();
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
}

void Task_buttons(void* pvParameters)
{
    buttons_params_t* buttons = (buttons_params_t*)pvParameters;

    while (true)
    {
        buttonFSM_update(buttons->cat0_btn);
        buttonFSM_update(buttons->cat1_btn);

        if (BUTTON_DOWN == buttons->cat0_btn->buttonState &&
            BUTTON_UP == buttons->cat1_btn->buttonState)
        {
            parse_data_to_string(true);
        }
        else if (BUTTON_DOWN == buttons->cat1_btn->buttonState &&
                 BUTTON_UP == buttons->cat0_btn->buttonState)
        {
            parse_data_to_string(false);
        }

        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

static void sd_card_init(void)
{
    sd_config = sd_spi_init();
    if (sd_config.host == SPI_HOST_MAX)
    {
        ESP_LOGE(BUTTONS, "Failed to initialize SD SPI");
        ErrorHandler();
    }

    card = (sd_card_t){.spi = sd_config.spi_handle, .mount_point = "/sdcard"};

    if (!sd_file_exists(&card, FILE_PATH))
    {
        if (sd_create_file(&card, FILE_PATH) != ESP_OK)
        {
            ESP_LOGE(BUTTONS, "Failed to create file");
            ErrorHandler();
        }
        else
        {
            if (sd_write_file(&card, FILE_PATH, FILE_HEADER) != ESP_OK)
            {
                ESP_LOGE(BUTTONS, "Failed to write header to file");
                ErrorHandler();
            }
        }
    }
    else
    {
        ESP_LOGI(BUTTONS, "File exists with %d lines", sd_count_lines(&card, FILE_PATH));
    }

    ESP_LOGI(BUTTONS, "SD card initialized successfully");
}

static void parse_data_to_string(bool category)
{

    if (xSemaphoreTake(xSemaphoreData, portMAX_DELAY) == pdTRUE)
    {
        // recibir de las colas
        if (xQueueReceive(xQueueGNSSData, &gnss_task_data, 0) != pdTRUE)
        {
            ESP_LOGE(BUTTONS, "Error receiving GNSS data");
        }

        if (xQueueReceive(xQueueSoilData, &soil_task_data, 0) != pdTRUE)
        {
            ESP_LOGE(BUTTONS, "Error receiving soil data");
        }

        char datetime[10];
        snprintf(datetime, sizeof(datetime), "%02d/%02d/%02d %02d:%02d", gnss_task_data.day,
                 gnss_task_data.month, gnss_task_data.year, gnss_task_data.hour,
                 gnss_task_data.minute);
        char category_str[4];
        snprintf(category_str, sizeof(category_str), "%d", category);
        snprintf(data_buffer, sizeof(data_buffer),
                 "%d,%d,%d,%.1f,%.2f,%d,%.1f,%.2f,%.6f,%.6f,%s,%s\n", soil_task_data.nitrogen,
                 soil_task_data.phosphorus, soil_task_data.potassium, soil_task_data.moisture,
                 soil_task_data.temperature, soil_task_data.conductivity, soil_task_data.pH,
                 gnss_task_data.altitude, gnss_task_data.latitude, gnss_task_data.longitude,
                 datetime, category_str);

        if (sd_append_file(&card, FILE_PATH, data_buffer) != ESP_OK)
        {
            ESP_LOGE(BUTTONS, "Failed to append data to file");
            ErrorHandler();
        }
        xSemaphoreGive(xSemaphoreData);
    }
}