#include "api_uart.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "logger.h"

#define UART_TAG "UART_API"

/**
 * @brief Escribe datos en la UART especificada.
 *
 * Esta función envía los datos proporcionados a la interfaz UART especificada por el parámetro
 * uart.
 *
 * @param uart Puntero a la estructura uart_t que especifica la interfaz UART.
 * @param request Puntero al buffer de datos que se va a escribir en la UART.
 * @param request_size Tamaño del buffer de datos que se va a escribir.
 *
 * @return
 *     - ESP_OK: Los datos se escribieron correctamente en la UART.
 *     - ESP_FAIL: Ocurrió un error al escribir datos en la UART.
 */
esp_err_t uart_write_data(uart_t* uart, const uint8_t* request, size_t request_size)
{
    int bytes_written = uart_write_bytes(uart->uart_num, (const char*)request, request_size);
    if (bytes_written < 0)
    {
        ESP_LOGE(UART_TAG, "Error al escribir datos en UART %d", uart->uart_num);
        return ESP_FAIL;
    }
    ESP_LOGI(UART_TAG, "%d bytes escritos en UART %d", bytes_written, uart->uart_num);
    return ESP_OK;
}

/**
 * @brief Lee datos de la interfaz UART especificada.
 *
 * Esta función lee datos de la interfaz UART especificada por el parámetro `uart`.
 * Vacía el buffer UART antes de leer y almacena los datos leídos en el buffer `response`.
 * La función leerá hasta `response_size - 1` bytes y añadirá un terminador nulo al final de los
 * datos leídos.
 *
 * @param uart Puntero a la estructura de la interfaz UART.
 * @param response Puntero al buffer donde se almacenarán los datos leídos.
 * @param response_size Tamaño del buffer de respuesta.
 * @param timeout Tiempo máximo para esperar a que se lean los datos, en ticks.
 *
 * @return
 *      - ESP_OK: Los datos se leyeron correctamente de la interfaz UART.
 *      - ESP_FAIL: Ocurrió un error al leer datos de la interfaz UART.
 */
esp_err_t uart_read_data(uart_t* uart, uint8_t* response, size_t response_size, TickType_t timeout)
{

    port_uart_flush(uart);

    int len = uart_read_bytes(uart->uart_num, response, response_size - 1, timeout);

    if (len > 0)
    {

        response[len] = '\0';
        ESP_LOGI(UART_TAG, "%d bytes leídos de UART %d", len, uart->uart_num);
    }
    else
    {

        ESP_LOGE(UART_TAG, "Error al leer datos de UART %d", uart->uart_num);
        return ESP_FAIL;
    }

    return ESP_OK;
}

/**
 * @brief Devuelve la cantidad de bytes disponibles en la cola de UART.
 *
 * Esta función consulta la cantidad de datos que están actualmente en el
 * buffer de la UART especificada y devuelve ese valor.
 *
 * @param uart Puntero a la estructura uart_t que representa la UART a consultar.
 * @return int Cantidad de bytes disponibles en la cola de la UART.
 */

int uart_available(uart_t* uart)
{
    size_t available_bytes;
    uart_get_buffered_data_len(uart->uart_num, &available_bytes);
    return available_bytes;
}

// Peek (ver pero no remover el siguiente byte en el buffer UART)
int uart_peek(uart_t* uart)
{
    if (uart == NULL)
    {
        return -1; // Error: puntero inválido
    }

    if (uart->has_peek)
    {
        return uart->peek_byte; // Retorna el byte almacenado
    }

    uint8_t byte;
    int len = uart_read_bytes(uart->uart_num, &byte, 1, 0);
    if (len > 0)
    {
        uart->has_peek = true;
        uart->peek_byte = byte;
        return byte;
    }

    return -1; // No hay datos disponibles
}

// Lee y elimina el siguiente byte del buffer UART
int uart_read_byte(uart_t* uart)
{
    if (uart == NULL)
    {
        return -1; // Error: puntero inválido
    }

    // Si tenemos un byte "peekeado", lo retornamos y lo eliminamos
    if (uart->has_peek)
    {
        uart->has_peek = false; // Restablecemos el estado de "peek"
        return uart->peek_byte; // Retorna el byte "peekeado"
    }

    uint8_t byte;
    int length = uart_read_bytes(uart->uart_num, &byte, 1, 0); // No bloquea
    if (length == 1)
    {
        return byte;
    }

    return -1; // No hay bytes disponibles
}

// Flush del UART - Espera que se complete la transmisión de datos
esp_err_t port_uart_flush(uart_t* uart) { return uart_flush(uart->uart_num); }
