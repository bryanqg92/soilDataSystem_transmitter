#ifndef API_UART_H
#define API_UART_H

#include "esp_err.h"
#include "uart_handler.h"

/**
 * @brief Escribe datos a través de UART
 * @param uart Puntero a la estructura UART que contiene el puerto UART y otros ajustes
 * @param request Puntero al buffer de datos a escribir
 * @param request_size Tamaño de los datos a escribir en bytes
 * @return ESP_OK si es exitoso, ESP_FAIL en caso contrario
 */
esp_err_t uart_write_data(uart_t* uart, const uint8_t* request, size_t request_size);

/**
 * @brief Lee datos desde UART y los almacena en el buffer proporcionado
 * @param uart Puntero a la estructura UART que contiene el puerto UART y otros ajustes
 * @param response Puntero al buffer donde se almacenarán los datos leídos
 * @param response_size Tamaño del buffer en bytes
 * @param timeout Tiempo máximo para esperar datos en ticks
 * @return ESP_OK si es exitoso, ESP_FAIL en caso contrario
 */
esp_err_t uart_read_data(uart_t* uart, uint8_t* response, size_t response_size, TickType_t timeout);

/**
 * @brief Devuelve el número de bytes disponibles en el buffer de recepción del UART
 * @param uart Puntero a la estructura UART que contiene el puerto UART y otros ajustes
 * @return El número de bytes disponibles en el buffer de recepción
 */
int uart_available(uart_t* uart);

/**
 * @brief Mira el primer byte en el buffer de recepción del UART sin eliminarlo
 * @param uart Puntero a la estructura UART que contiene el puerto UART y otros ajustes
 * @return El primer byte en el buffer de recepción o -1 si no hay datos disponibles
 */
int uart_peek(uart_t* uart);

/**
 * @brief Lee y elimina el siguiente byte del buffer de recepción del UART
 * @param uart Puntero a la estructura UART que contiene el puerto UART y otros ajustes
 * @return El siguiente byte en el buffer de recepción o -1 si no hay datos disponibles
 */
int uart_read_byte(uart_t* uart);

/**
 * @brief Espera a que la transmisión de datos seriales salientes se complete
 * @param uart Puntero a la estructura UART que contiene el puerto UART y otros ajustes
 * @return ESP_OK si es exitoso, ESP_FAIL en caso contrario
 */
esp_err_t port_uart_flush(uart_t* uart);

/**
 * @brief Lee datos desde UART y los almacena en el buffer proporcionado (versión asíncrona)
 * @param uart Puntero a la estructura UART que contiene el puerto UART y otros ajustes
 * @param response Puntero al buffer donde se almacenarán los datos leídos
 * @param response_size Tamaño del buffer en bytes
 * @param timeout Tiempo máximo para esperar datos en ticks
 * @return ESP_OK si es exitoso, ESP_FAIL en caso contrario
 */
esp_err_t uart_read_bytes_a(uart_t* uart, uint8_t* response, size_t response_size,
                            TickType_t timeout);

#endif /* API_UART_H */
