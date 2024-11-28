

#ifndef NPK_UART_HANDLER_H
#define NPK_UART_HANDLER_H

#include "driver/uart.h"
#include "uart_handler.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Initializes the UART interface for the NPK sensor.
     *
     * This function sets up the UART communication parameters and prepares the
     * interface for communication with the NPK sensor. It returns a uart_t
     * structure that contains the necessary information for UART operations.
     *
     * @return uart_t A structure containing UART configuration and state information.
     */
    uart_t init_npk_sensor_uart();

#ifdef __cplusplus
}
#endif

#endif // NPK_UART_HANDLER_H
