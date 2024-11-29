/**
 * @file gnss_reader.h
 * @brief Cabecera para la lectura de datos GNSS.
 *
 * Este archivo contiene las declaraciones necesarias para la lectura de datos GNSS
 * utilizando la API UART.
 */

 /**
 * @brief Tarea para procesar datos GNSS.
 *
 * Esta función se encarga de procesar los datos GNSS recibidos a través de la UART.
 *
 * @param pvParameters Puntero a los parámetros de la tarea (no utilizado).
 */
#ifndef GNSS_READER_H
#define GNSS_READER_H

#include "api_uart.h"

void Task_GNSSData(void* pvParameters);

#endif // GNSS_READER_H
