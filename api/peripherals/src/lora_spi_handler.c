#include "lora_spi_handler.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"

const char* LORA_SPI_HANDLER = "==> LORA_SPI_HANDLER";

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

    // Configura los pines GPIO
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << lora_pins.lora_nss_pin) | (1ULL << lora_pins.lora_rst_pin),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE // Habilitar pull-up en el pin NSS
    };
    gpio_config(&io_conf);
    gpio_set_level(lora_pins.lora_nss_pin, 1); // CS/NSS en alto (inactivo)

    // Configura el pin BUSY como entrada
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << lora_pins.lora_busy_pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; // Deshabilitar pull-down
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;     // Deshabilitar pull-up
    gpio_config(&io_conf);

    // Reinicia la estructura para configurar el pin DIO1
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << lora_pins.lora_dio1_pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; // Deshabilitar pull-down
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;     // Deshabilitar pull-up
    gpio_config(&io_conf);

    // Reset del módulo LoRa
    gpio_set_level(lora_pins.lora_rst_pin, 0); // Reset en bajo
    vTaskDelay(pdMS_TO_TICKS(10));             // Espera 10 ms
    gpio_set_level(lora_pins.lora_rst_pin, 1); // Reset en alto
    vTaskDelay(pdMS_TO_TICKS(10));             // Espera 10 ms

    // Inicializa el bus SPI
    spi_bus_config_t bus = {.miso_io_num = lora_pins.lora_miso_pin,
                            .mosi_io_num = lora_pins.lora_mosi_pin,
                            .sclk_io_num = lora_pins.lora_sck_pin,
                            .quadwp_io_num = -1,
                            .quadhd_io_num = -1,
                            .max_transfer_sz = 4096};
    esp_err_t ret = spi_bus_initialize(lora_pins.host, &bus, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(LORA_SPI_HANDLER, "Error al inicializar el bus SPI: %s", esp_err_to_name(ret));
        return lora_pins;
    }

    // Configura el dispositivo SPI
    spi_device_interface_config_t dev = {.clock_speed_hz = LORA_SPI_CLOCK_SPEED,
                                         .mode = 0, // Modo SPI (0, 1, 2, 3)
                                         .spics_io_num =
                                             -1, // Usar el pin NSS automáticamente O-1 MANUAL
                                         .queue_size = 7,
                                         .flags = SPI_DEVICE_NO_DUMMY};
    spi_device_handle_t __spi;
    ret = spi_bus_add_device(lora_pins.host, &dev, &__spi);
    if (ret != ESP_OK)
    {
        ESP_LOGE(LORA_SPI_HANDLER, "Error al añadir dispositivo SPI: %s", esp_err_to_name(ret));
        return lora_pins;
    }

    lora_pins.spi_handle = __spi;

    ESP_LOGI(LORA_SPI_HANDLER, "SPI y pines GPIO configurados correctamente");
    return lora_pins;
}