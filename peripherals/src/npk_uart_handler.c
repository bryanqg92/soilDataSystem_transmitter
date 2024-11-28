/**
 * @file npk_uart_handler.c
 * @brief UART handler for NPK sensor communication.
 *
 * This file contains the implementation of the UART handler for the NPK sensor.
 * It includes the initialization of the UART interface and the configuration
 * of the UART parameters.
 *
 * @includes
 * - npk_uart_handler.h
 * - driver/uart.h
 * - driver/gpio.h
 *
 * @defines
 * - NPK_SENSOR_BAUDRATE: Baud rate for the NPK sensor communication.
 * - NPK_SENSOR_UART: UART port number used for the NPK sensor.
 * - NPK_SENSOR_RX_PIN: GPIO pin number for the RX pin of the NPK sensor.
 * - NPK_SENSOR_TX_PIN: GPIO pin number for the TX pin of the NPK sensor.
 * - UART_BUF_SIZE: Size of the UART buffer.
 *
 * @function init_npk_sensor_uart
 * @brief Initializes the UART interface for the NPK sensor.
 *
 * This function configures the UART parameters, installs the UART driver,
 * sets the UART pins, and creates a queue for storing the data read from
 * the UART interface.
 *
 * @return uart_t
 * A structure containing the UART configuration and data queue.
 */
#include "npk_uart_handler.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#define NPK_SENSOR_BAUDRATE 9600
#define NPK_SENSOR_UART UART_NUM_2
#define NPK_SENSOR_RX_PIN 46
#define NPK_SENSOR_TX_PIN 45
#define UART_BUF_SIZE 1024

uart_t init_npk_sensor_uart()
{
    // Configuración de UART
    uart_config_t uart_config = {
        .baud_rate = NPK_SENSOR_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // Instalar el controlador de UART
    uart_driver_install(NPK_SENSOR_UART, UART_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(NPK_SENSOR_UART, &uart_config);
    uart_set_pin(NPK_SENSOR_UART, NPK_SENSOR_TX_PIN, NPK_SENSOR_RX_PIN, UART_PIN_NO_CHANGE,
                 UART_PIN_NO_CHANGE);

    // Crear la cola para almacenar los datos leídos
    QueueHandle_t data_queue = xQueueCreate(10, sizeof(uint8_t)); // Tamaño de cola de 10 bytes

    // Crear y devolver la estructura uart_t
    uart_t npk_uart = {.uart_num = NPK_SENSOR_UART,
                       .has_peek = false,
                       .peek_byte = 0,
                       .buffered_size = 0,
                       .data_queue = data_queue};

    return npk_uart;
}
