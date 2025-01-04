
#ifndef __LORA_SPI_HANDLER_H__
#define __LORA_SPI_HANDLER_H__

#include "config.h"
#include "driver/spi_master.h"

typedef struct
{
    spi_device_handle_t spi_handle;
    spi_host_device_t host;
    int lora_mosi_pin;
    int lora_miso_pin;
    int lora_sck_pin;
    int lora_nss_pin;
    int lora_rst_pin;
    int lora_dio1_pin;
    int lora_busy_pin;

} lora_config_t;
lora_config_t lora_spi_init(void);
#endif