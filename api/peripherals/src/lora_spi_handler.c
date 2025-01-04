#include "lora_spi_handler.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "logger.h"

lora_config_t lora_spi_init(void)
{

    lora_config_t lora_pins = {.host = LORA_SPI_HOST,
                               .lora_mosi_pin = LORA_MOSI_GPIO,
                               .lora_miso_pin = LORA_MISO_GPIO,
                               .lora_sck_pin = LORA_SCK_GPIO,
                               .lora_nss_pin = LORA_CS_GPIO,
                               .lora_rst_pin = LORA_RST_GPIO,
                               .lora_dio1_pin = LORA_DIO0_GPIO,
                               .lora_busy_pin = LORA_BUSY_GPIO,
                               .spi_handle = NULL};

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << lora_pins.lora_nss_pin) | (1ULL << lora_pins.lora_rst_pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    esp_err_t ret;

    /*
     * Configure CPU hardware to communicate with the radio chip
     */

    spi_bus_config_t bus = {.miso_io_num = lora_pins.lora_miso_pin,
                            .mosi_io_num = lora_pins.lora_mosi_pin,
                            .sclk_io_num = lora_pins.lora_sck_pin,
                            .quadwp_io_num = -1,
                            .quadhd_io_num = -1,
                            .max_transfer_sz = 0};

    ret = spi_bus_initialize(SPI2_HOST, &bus, 0);
    assert(ret == ESP_OK);

    spi_device_handle_t __spi;
    spi_device_interface_config_t dev = {.clock_speed_hz = 9000000,
                                         .mode = 0,
                                         .spics_io_num = -1,
                                         .queue_size = 1,
                                         .flags = 0,
                                         .pre_cb = NULL};
    ret = spi_bus_add_device(lora_pins.host, &dev, &__spi);
    assert(ret == ESP_OK);

    lora_pins.spi_handle = __spi;

    return lora_pins;
}