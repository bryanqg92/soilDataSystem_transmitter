#include "gnss_uart_handler.h"
#include "logger.h"
#include "npk_uart_handler.h"

#include "app.h"
#include "gnss_reader.h"
#include "shared_data.h"
#include "soil_sensor_reader.h"
#include "tft_manager.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#define VGNSS_CTRL 3

static const char* APP = "==> APP";

static void soil_sensor_init(void);
static void gnss_sensor_init(void);
static void tft_display_init(void);

TaskHandle_t taskProcessData_h;
TaskHandle_t taskGNSSData_h;
TaskHandle_t taskTFTDisplay_h;

SoilData_t soilData;
GNSSElements_t gnssContext;
TFTElements_t tft_context;

QueueHandle_t xQueueGNSSData;
QueueHandle_t xQueueSoilData;

void app_init(void)
{

    BaseType_t ret;

    xQueueGNSSData = xQueueCreate(1, sizeof(GNSSData_t));
    configASSERT(xQueueGNSSData != NULL);
    xQueueSoilData = xQueueCreate(1, sizeof(SoilData_t));
    configASSERT(xQueueSoilData != NULL);

    vQueueAddToRegistry(xQueueGNSSData, "GNSSData");
    vQueueAddToRegistry(xQueueSoilData, "SoilData");

    gnss_sensor_init();
    tft_display_init();
    soil_sensor_init();

    ret = xTaskCreate(Task_processData, "ProcessDataTask", 3056, (void*)&soilData,
                      (tskIDLE_PRIORITY + 1ul), &taskProcessData_h);

    configASSERT(pdPASS == ret);

    ret = xTaskCreate(Task_GNSSData, "GNSSDataTask", 10000, (void*)&gnssContext,
                      (tskIDLE_PRIORITY + 1ul), &taskGNSSData_h);

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
        ErrorHandler();
    }
    if (!NPKInit(&soilData.npk_port))
    {
        ESP_LOGE(APP, "Failed to initialize NPK sensor");
        ErrorHandler();
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
        ErrorHandler();
    }
    ESP_LOGI(APP, "GNSS sensor initialized successfully");
}

static void tft_display_init(void)
{
    tft_context.tft_host = tft_spi_init();
    if (tft_context.tft_host.host == SPI_HOST_MAX)
    {
        ESP_LOGE(APP, "Failed to initialize TFT SPI");
        ErrorHandler();
    }
}

void ErrorHandler(void)
{

    ESP_LOGE(APP, "ErrorHandler");
    while (true)
    {
    }
}
