#ifndef CONFIG_H_
#define CONFIG_H_

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

#endif /* CONFIG_H_ */