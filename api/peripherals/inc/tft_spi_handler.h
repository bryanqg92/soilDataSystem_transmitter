/**
 * @file tft_spi_handler.h
 * @brief Header file for TFT SPI handler.
 *
 * This file contains the definitions and function declarations for initializing
 * and configuring the TFT display using SPI communication.
 *
 * @details
 * The tft_config_t structure holds the configuration parameters for the TFT display,
 * including the SPI handle, host, and pin assignments for MOSI, SCLK, CS, DC, RST, and LED_K.
 * The file also defines constants for the maximum transfer size, SPI clock speed, and pin
 * assignments.
 *
 * @typedef tft_config_t
 * @brief Structure to hold TFT SPI configuration parameters.
 *
 * @var tft_config_t::spi_handle
 * SPI device handle.
 *
 * @var tft_config_t::host
 * SPI host device.
 *
 * @var tft_config_t::mosi_pin
 * Pin number for MOSI.
 *
 * @var tft_config_t::sclk_pin
 * Pin number for SCLK.
 *
 * @var tft_config_t::cs_pin
 * Pin number for Chip Select (CS).
 *
 * @var tft_config_t::dc_pin
 * Pin number for Data/Command (DC).
 *
 * @var tft_config_t::rst_pin
 * Pin number for Reset (RST).
 *
 * @var tft_config_t::led_k_pin
 * Pin number for LED backlight (LED_K).
 *
 * @def TFT_MAX_TRANSFER_SIZE
 * Maximum transfer size for SPI communication.
 *
 * @def TFT_SPI_CLOCK_SPEED
 * SPI clock speed in Hz.
 *
 * @def TFT_SPI_HOST
 * SPI host device.
 *
 * @def TFT_CS_Pin
 * Pin number for Chip Select (CS).
 *
 * @def TFT_REST_Pin
 * Pin number for Reset (RST).
 *
 * @def TFT_DC_Pin
 * Pin number for Data/Command (DC).
 *
 * @def TFT_SCLK_Pin
 * Pin number for SCLK.
 *
 * @def TFT_MOSI_Pin
 * Pin number for MOSI.
 *
 * @def TFT_LED_K_Pin
 * Pin number for LED backlight (LED_K).
 *
 * @fn tft_config_t tft_spi_init(void)
 * @brief Initializes the TFT SPI configuration.
 *
 * This function initializes the TFT SPI configuration and returns a tft_config_t structure
 * with the configured parameters.
 *
 * @return tft_config_t structure with the initialized configuration.
 */
#ifndef TFT_SPI_HANDLER_H_
#define TFT_SPI_HANDLER_H_

#include "driver/spi_master.h"
#include "config.h"

typedef struct
{
    spi_device_handle_t spi_handle;
    spi_host_device_t host;
    int mosi_pin;
    int sclk_pin;
    int cs_pin;
    int dc_pin;
    int rst_pin;
    int led_k_pin;
} tft_config_t;



tft_config_t tft_spi_init(void);

#endif /* TFT_SPI_HANDLER_H_ */
