/**
 * @file gnss_uart_handler.c
 * @brief UART handler for GNSS module communication.
 *
 * This file contains the implementation of the UART handler for the GNSS module.
 * It includes the initialization of the UART interface with the specified parameters.
 */

#include "gnss_uart_handler.h"
#include "driver/gpio.h"
#include "driver/uart.h"


/**
 * @brief Initializes the UART interface for GNSS communication.
 *
 * This function configures and installs the UART driver for the GNSS module.
 * It sets the baud rate, data bits, parity, stop bits, and flow control.
 * It also sets the TX and RX pins for the UART interface.
 *
 * @return A uart_t structure containing the UART configuration and data queue.
 */

uart_t init_gnss_uart()
{
    // Configuración de UART
    uart_config_t uart_config = {
        .baud_rate = GNSS_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // Instalar el controlador de UART
    uart_driver_install(GNSS_UART, GNSS_UART_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(GNSS_UART, &uart_config);
    uart_set_pin(GNSS_UART, GNSS_TX_PIN, GNSS_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    QueueHandle_t data_queue = xQueueCreate(10, sizeof(uint8_t));
    // Crear y devolver la estructura uart_t
    uart_t gnss_uart = {.uart_num = GNSS_UART,
                        .has_peek = false,
                        .peek_byte = 0,
                        .buffered_size = 0,
                        .data_queue = data_queue};

    return gnss_uart;
}
