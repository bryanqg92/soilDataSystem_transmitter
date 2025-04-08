#include "esp_log.h"
#include "gnss_uart_handler.h"
#include "npk_uart_handler.h"

#include "api_debounce.h"
#include "app.h"
#include "buttons_manager.h"
#include "gnss_reader.h"
#include "lora_manager.h"
#include "lora_spi_handler.h"
#include "shared_data.h"
#include "soil_sensor_reader.h"
#include "tft_manager.h"

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define VGNSS_CTRL 3

static const char* APP = "==> APP";
BaseType_t ret;

static void soil_sensor_init(void);
static void gnss_sensor_init(void);
static void tft_display_init(void);
static void lora_init(void);

TaskHandle_t taskProcessData_h, taskGNSSData_h, taskTFTDisplay_h, taskButtons_h;

SoilData_t soilData;
GNSSData_t gnssData;
GNSSElements_t gnssContext;
TFTElements_t tft_context;

QueueHandle_t xQueueGNSSData, xQueueSoilData;
SemaphoreHandle_t xSemaphoreData;

button_app_t cat0_btn, cat1_btn;
buttons_params_t buttons_params;

lora_config_t lora_config;

void app_init(void)
{

    soilData = (SoilData_t){
        .conductivity = 0,
        .moisture = 0,
        .nitrogen = 0,
        .phosphorus = 0,
        .potassium = 0,
        .pH = 0,
        .temperature = 0,
    };
    gnssData = (GNSSData_t){
        .altitude = 0,
        .day = 0,
        .fix_status = 0,
        .hour = 0,
        .latitude = 0,
        .longitude = 0,
        .minute = 0,
        .month = 0,
        .satellites_used = 0,
        .year = 0,
    };

    gnssContext.gnssData = gnssData;

    xQueueGNSSData = xQueueCreate(5, sizeof(GNSSData_t));
    configASSERT(xQueueGNSSData != NULL);

    xQueueSoilData = xQueueCreate(5, sizeof(SoilData_t));
    configASSERT(xQueueSoilData != NULL);

    vQueueAddToRegistry(xQueueGNSSData, "GNSSData");
    vQueueAddToRegistry(xQueueSoilData, "SoilData");

    gnss_sensor_init();
    tft_display_init();
    soil_sensor_init();
    lora_init();
    buttons_init(&cat0_btn, &cat1_btn);

    buttons_params.cat0_btn = &cat0_btn;
    buttons_params.cat1_btn = &cat1_btn;

    ret = xTaskCreate(Task_buttons, "ButtonsTask", 4096, (void*)&buttons_params,
                      (tskIDLE_PRIORITY + 3ul), &taskButtons_h);
    configASSERT(pdPASS == ret);

    ret = xTaskCreate(Task_processData, "ProcessDataTask", 3056, (void*)&soilData,
                      (tskIDLE_PRIORITY + 1ul), &taskProcessData_h);
    configASSERT(pdPASS == ret);

    ret = xTaskCreate(Task_GNSSData, "GNSSDataTask", 10000, (void*)&gnssContext,
                      (tskIDLE_PRIORITY + 2ul), &taskGNSSData_h);
    configASSERT(pdPASS == ret);

    ret = xTaskCreate(Task_TFTDisplay, "TFTDisplayTask", 3056, (void*)&tft_context,
                      (tskIDLE_PRIORITY + 1ul), &taskTFTDisplay_h);
    configASSERT(pdPASS == ret);

    ESP_LOGI(APP, "Task created successfully");
}

static void soil_sensor_init(void)
{

    soilData.npk_port = init_npk_sensor_uart();

    if (soilData.npk_port.uart_num == UART_NUM_MAX)
    {
        ESP_LOGE(APP, "Failed to initialize NPK sensor UART");
        // ErrorHandler();
    }
    if (!NPKInit(&soilData.npk_port))
    {
        ESP_LOGE(APP, "Failed to initialize NPK sensor");
        // ErrorHandler();
    }
    ESP_LOGI(APP, "NPK sensor initialized successfully");
}

static void gnss_sensor_init(void)
{

    gpio_set_direction(VGNSS_CTRL, GPIO_MODE_OUTPUT);
    gpio_set_level(VGNSS_CTRL, 1);

    gnssContext.gnss_port = init_gnss_uart();

    if (gnssContext.gnss_port.uart_num == UART_NUM_MAX)
    {
        ESP_LOGE(APP, "Failed to initialize GNSS UART");
    }
    ESP_LOGI(APP, "GNSS sensor initialized successfully");
}

static void tft_display_init(void)
{
    tft_context.tft_host = tft_spi_init();
    if (tft_context.tft_host.host == SPI_HOST_MAX)
    {
        ESP_LOGE(APP, "Failed to initialize TFT SPI");
    }
}

static void lora_init()
{
    lora_config = lora_spi_init();
    if (lora_config.host == SPI_HOST_MAX)
    {
        ESP_LOGE(APP, "Failed to initialize LORA SPI");
        return;
    }
    ret = lora_set_config(&lora_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(APP, "Failed to initialize LORA");
    }
}

void ErrorHandler(void)
{

    ESP_LOGE(APP, "ErrorHandler");
    while (true)
    {
    }
}