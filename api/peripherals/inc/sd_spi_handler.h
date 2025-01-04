#ifndef _SD_SPI_HANDLER_H_
#define _SD_SPI_HANDLER_H_

#include "driver/spi_master.h"
#include "logger.h"
typedef struct
{
    spi_device_handle_t spi_handle;
    spi_host_device_t host;
    int sd_mosi_pin;
    int sd_miso_pin;
    int sd_sck_pin;
    int sd_nss_pin;
    int sd_rst_pin;
    int sd_dio1_pin;
    int sd_busy_pin;

} sd_config_t;

sd_config_t sd_spi_init(void);

#endif
