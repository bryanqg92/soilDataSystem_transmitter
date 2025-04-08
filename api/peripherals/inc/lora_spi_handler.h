
#ifndef __LORA_SPI_HANDLER_H__
#define __LORA_SPI_HANDLER_H__

#include "config.h"
#include "driver/spi_master.h"

typedef struct
{
    spi_device_handle_t spi_handle;
    spi_host_device_t host;
    uint8_t lora_mosi_pin;
    uint8_t lora_miso_pin;
    uint8_t lora_sck_pin;
    uint8_t lora_nss_pin;
    uint8_t lora_rst_pin;
    uint8_t lora_dio1_pin;
    uint8_t lora_busy_pin;

} lora_config_t;
lora_config_t lora_spi_init(void);
#endif