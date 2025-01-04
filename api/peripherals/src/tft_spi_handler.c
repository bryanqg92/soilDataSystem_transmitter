#include "tft_spi_handler.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "logger.h"

static const char* TFT_HANDLER = "TFT_SPI";

// Bandera para rastrear si el SPI ya está inicializado
static bool is_spi_initialized = false;

/**
 * @brief Initializes the TFT SPI interface and configures the necessary GPIO pins.
 *
 * This function performs the following steps:
 * 1. Configures the GPIO pins required for the TFT display.
 * 2. Initializes the SPI bus if it has not been initialized already.
 * 3. Adds the TFT device to the SPI bus.
 * 4. Configures the backlight and reset pins.
 *
 * @return tft_config_t A structure containing the configuration of the TFT pins and SPI handle.
 *
 * @note If the SPI bus is already initialized by another part of the code, it will skip
 * re-initialization.
 * @note If there is an error during SPI bus initialization or adding the SPI device, it will return
 * a structure with an invalid host.
 */

tft_config_t tft_spi_init(void)
{
    ESP_LOGI(TFT_HANDLER, "Starting TFT SPI initialization...");

    // Configuración de pines para TFT
    tft_config_t tft_pins = {
        .host = TFT_SPI_HOST,
        .mosi_pin = TFT_MOSI_Pin,
        .sclk_pin = TFT_SCLK_Pin,
        .cs_pin = TFT_CS_Pin,
        .dc_pin = TFT_DC_Pin,
        .rst_pin = TFT_REST_Pin,
        .led_k_pin = TFT_LED_K_Pin,
        .spi_handle = NULL // Inicialmente NULL, se asignará más adelante
    };

    ESP_LOGI(TFT_HANDLER, "Configuring GPIO pins...");
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << tft_pins.cs_pin) | (1ULL << tft_pins.rst_pin) |
                           (1ULL << tft_pins.dc_pin) | (1ULL << tft_pins.led_k_pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // Inicializa los pines a un estado conocido
    gpio_set_level(tft_pins.cs_pin, 1);
    gpio_set_level(tft_pins.rst_pin, 1);
    gpio_set_level(tft_pins.dc_pin, 1);
    gpio_set_level(tft_pins.led_k_pin, 1);

    // Configuración del bus SPI
    spi_bus_config_t bus_cfg = {.mosi_io_num = tft_pins.mosi_pin,
                                .miso_io_num = -1,
                                .sclk_io_num = tft_pins.sclk_pin,
                                .quadwp_io_num = -1,
                                .quadhd_io_num = -1,
                                .max_transfer_sz = TFT_MAX_TRANSFER_SIZE,
                                .flags = SPICOMMON_BUSFLAG_MASTER};

    if (is_spi_initialized)
    {
        ESP_LOGW(TFT_HANDLER, "SPI bus already initialized. Skipping re-initialization.");
    }
    else
    {
        ESP_LOGI(TFT_HANDLER, "Initializing SPI bus...");
        esp_err_t err = spi_bus_initialize(tft_pins.host, &bus_cfg, SPI_DMA_CH_AUTO);
        if (err == ESP_ERR_INVALID_STATE)
        {
            ESP_LOGW(TFT_HANDLER, "SPI bus is already initialized by another part of the code.");
        }
        else if (err != ESP_OK)
        {
            ESP_LOGE(TFT_HANDLER, "Failed to initialize SPI bus: %s", esp_err_to_name(err));
            return (tft_config_t){.host = SPI_HOST_MAX}; // Devuelve un host inválido
        }
        else
        {
            ESP_LOGI(TFT_HANDLER, "SPI bus initialized successfully.");
            is_spi_initialized = true;
        }
    }

    // Configuración del dispositivo SPI
    spi_device_interface_config_t devcfg = {.clock_speed_hz = TFT_SPI_CLOCK_SPEED,
                                            .mode = 0,
                                            .spics_io_num = tft_pins.cs_pin,
                                            .queue_size = 7,
                                            .flags = SPI_DEVICE_NO_DUMMY,
                                            .pre_cb = NULL,
                                            .post_cb = NULL};

    ESP_LOGI(TFT_HANDLER, "Adding SPI device...");
    spi_device_handle_t spi_handle;
    esp_err_t err = spi_bus_add_device(tft_pins.host, &devcfg, &spi_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TFT_HANDLER, "Failed to add SPI device: %s", esp_err_to_name(err));
        return (tft_config_t){.host = SPI_HOST_MAX}; // Devuelve un host inválido
    }
    tft_pins.spi_handle = spi_handle; // Asigna el handle del dispositivo SPI

    // Configuración de los pines restantes
    ESP_LOGI(TFT_HANDLER, "Configuring backlight and reset...");
    gpio_set_level(tft_pins.led_k_pin, 1); // Enciende el backlight

    gpio_set_level(tft_pins.rst_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(tft_pins.rst_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(tft_pins.rst_pin, 1);

    ESP_LOGI(TFT_HANDLER, "TFT SPI initialization complete.");
    return tft_pins;
}
