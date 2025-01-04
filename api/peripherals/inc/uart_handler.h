#ifndef UART_HANDLER_H
#define UART_HANDLER_H

#include <stdbool.h>
#include <stdint.h>

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "config.h"

typedef uart_port_t uart_port_type;

typedef struct
{
    uart_port_type uart_num;  // El puerto UART
    bool has_peek;            // Indica si se ha hecho un "peek"
    uint8_t peek_byte;        // El byte "peeked"
    size_t buffered_size;     // Tamaño del buffer
    QueueHandle_t data_queue; // Cola para almacenar datos leídos
} uart_t;

#endif /* UART_HANDLER_H */
