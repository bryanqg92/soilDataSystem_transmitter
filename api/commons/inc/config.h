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
#define LORA_SPI_CLOCK_SPEED (2 * 1000 * 1000) // 2 MHz
#define LORA_SPI_HOST SPI2_HOST
#define LORA_CS_GPIO 8
#define LORA_SCK_GPIO 9
#define LORA_MOSI_GPIO 10
#define LORA_MISO_GPIO 11
#define LORA_RST_GPIO 12
#define LORA_BUSY_GPIO 13
#define LORA_DIO0_GPIO 14
#define LORA_SF_7 0x07
#define LORA_SF_8 0x08
#define LORA_SF_9 0x09
#define LORA_SF_10 0x0A
#define LORA_SF_11 0x0B
#define LORA_SF_12 0x0C
#define LORA_BW 4 // 125E3
#define LORA_FREQ 433E6
#define LORA_TX_POWER 22
#define LORA_PREAMBLE_LENGTH 8
#define LORA_PAYLOAD_LENGTH 0
#define LORA_CODING_RATE 0x01
#define LORA_SYNC_WORD 0x34
#define LORA_CMD_SET_DIO2_AS_RF_SWITCH_CTRL 0x9D
#define LORA_CMD_CALIBRATE_IMAGE 0x98
#define LORA_CRC_ON true
#define LORA_TXCO_VOLTAGE 3.3

#endif /* CONFIG_H_*/