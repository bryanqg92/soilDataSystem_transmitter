#ifndef APP_H
#define APP_H

#include "HT_st7735.h"
#include "api_gnss.h"
#include "api_uart.h"
#include "freertos/queue.h"
#include "shared_data.h"
#include "tft_spi_handler.h"

#define GNSS_MAX_MESSAGE_SIZE 2000
#define GNSS_TIMEOUT_MS 1000

typedef struct
{
    SoilData_t soilData;
    uart_t npk_port;
} soilElements_t;

typedef struct
{
    GNSSData_t gnssData;
    uart_t gnss_port;
} GNSSElements_t;

typedef struct
{
    tft_config_t tft_host;
    ST7735_Config tft_config;
} TFTElements_t;

extern QueueHandle_t xQueueGNSSData; // cola para los datos del GNSS
extern QueueHandle_t xQueueSoilData; // cola para los datos del sensor de humedad

void app_init(void);
void ErrorHandler(void);

#endif // APP_H
