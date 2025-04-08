#ifndef APP_H
#define APP_H

#include "HT_st7735.h"
#include "api_gnss.h"
#include "api_uart.h"
#include "freertos/queue.h"
#include "lora_spi_handler.h"
#include "shared_data.h"
#include "tft_spi_handler.h"

extern QueueHandle_t xQueueGNSSData; // cola para los datos del GNSS
extern QueueHandle_t xQueueSoilData; // cola para los datos del sensor de humedad
extern SemaphoreHandle_t xSemaphoreData;
extern lora_config_t lora_config;

void app_init(void);
void ErrorHandler(void);

#endif // APP_H
