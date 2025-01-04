#ifndef CONFIG_H
#define CONFIG_H

/** soil sensor reader uart configuration */
#define GNSS_BAUDRATE 115200 ///< Mandatory baud rate for GNSS communication.
#define GNSS_UART UART_NUM_1 ///< mandatory UART number for GNSS communication by heltec.
#define GNSS_RX_PIN 33       ///< Mandatory GPIO pin number for GNSS RX by Heltec.
#define GNSS_TX_PIN 34       ///< Mandatory GPIO pin number for GNSS TX by Heltec.
#define GNSS_UART_BUF_SIZE 2000

/* GNSS uart config. These parameters are mandatory por heltec board*/
#define NPK_SENSOR_BAUDRATE 9600
#define NPK_SENSOR_UART UART_NUM_2
#define NPK_SENSOR_RX_PIN 46
#define NPK_SENSOR_TX_PIN 45
#define UART_BUF_SIZE 1024

/* TFT display config. These parameters are mandatory for the Heltec board */
#define TFT_MAX_TRANSFER_SIZE (16 * 320 * 2)
#define TFT_SPI_CLOCK_SPEED (1 * 1000 * 1000) // 26 MHz
#define TFT_SPI_HOST SPI3_HOST
#define TFT_CS_Pin 38
#define TFT_REST_Pin 39
#define TFT_DC_Pin 40
#define TFT_SCLK_Pin 41
#define TFT_MOSI_Pin 42
#define TFT_LED_K_Pin 21

/* LoRa config. These parameters are mandatory for the Heltec board */
#define LORA_SPI_CLOCK_SPEED (9 * 1000 * 1000) // 9 MHz
#define LORA_SPI_HOST SPI2_HOST
#define LORA_CS_GPIO 8
#define LORA_SCK_GPIO 9
#define LORA_MOSI_GPIO 10
#define LORA_MISO_GPIO 11
#define LORA_RST_GPIO 12
#define LORA_BUSY_GPIO 13
#define LORA_DIO0_GPIO 14

/* SD card config. It uses the same SX1262 SPI peripheral. DO NOT USE at the same time for the
 * application */

#define SD_SPI_CLOCK_SPEED (20 * 1000 * 1000) // 20 MHz
#define SD_SPI_HOST SPI2_HOST
#define SD_CS_GPIO 8
#define SD_SCK_GPIO 9
#define SD_MOSI_GPIO 10
#define SD_MISO_GPIO 11

#endif /* CONFIG_H_*/