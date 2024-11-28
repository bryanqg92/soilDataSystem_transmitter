/**
 * @file gnss_uart_handler.h
 * @brief Header file for GNSS UART handler.
 *
 * This file contains the function declarations and necessary includes
 * for initializing and handling GNSS UART communication.
 *
 * @author Leandro Quiroga
 * @date 2023-11-01
 */

#ifndef GNSS_UART_HANDLER_H
#define GNSS_UART_HANDLER_H

#include "driver/uart.h"
#include "uart_handler.h"
#include "config.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Initializes the GNSS UART.
     *
     * This function sets up the UART configuration for GNSS communication.
     *
     * @return uart_t The initialized UART configuration for GNSS.
     */
    uart_t init_gnss_uart();

#ifdef __cplusplus
}
#endif

#endif // GNSS_UART_HANDLER_H
