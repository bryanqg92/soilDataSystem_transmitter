#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include "api_uart.h"
#include "tft_spi_handler.h"
#define GNSS_MAX_MESSAGE_SIZE 2000
#define GNSS_TIMEOUT_MS 1000

/**
 * @struct SoilData_t
 * @brief Structure to hold various soil sensor measurements
 */
typedef struct
{
    float temperature;
    float moisture;
    uint16_t conductivity;
    float pH;
    uint16_t nitrogen;
    uint16_t phosphorus;
    uint16_t potassium;
    uart_t npk_port;
    uint8_t status;

} SoilData_t;

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

#endif // SHARED_DATA_H
/*
$CFGMSG,0,5,1*2B
*/
