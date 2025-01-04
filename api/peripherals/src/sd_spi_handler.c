#include "sd_spi_handler.h"
#include "config.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "logger.h"

sd_config_t sd_spi_init(void)
{

    sd_config_t sd_pins = {.host = SD_SPI_HOST,
                           .sd_mosi_pin = SD_MOSI_GPIO,
                           .sd_miso_pin = SD_MISO_GPIO,
                           .sd_sck_pin = SD_SCK_GPIO,
                           .sd_nss_pin = SD_CS_GPIO,
                           .spi_handle = NULL};

    esp_err_t ret;

    /*
     * Configure CPU hardware to communicate with the radio chip
     */

    spi_bus_config_t bus = {.miso_io_num = sd_pins.sd_miso_pin,
                            .mosi_io_num = sd_pins.sd_mosi_pin,
                            .sclk_io_num = sd_pins.sd_sck_pin,
                            .quadwp_io_num = -1,
                            .quadhd_io_num = -1,
                            .max_transfer_sz = 4000};

    ret = spi_bus_initialize(sd_pins.host, &bus, 0);
    assert(ret == ESP_OK);

    spi_device_handle_t __spi;
    spi_device_interface_config_t dev = {.clock_speed_hz = 9000000,
                                         .mode = 0,
                                         .spics_io_num = -1,
                                         .queue_size = 1,
                                         .flags = 0,
                                         .pre_cb = NULL};
    ret = spi_bus_add_device(sd_pins.host, &dev, &__spi);
    assert(ret == ESP_OK);

    sd_pins.spi_handle = __spi;

    return sd_pins;
}