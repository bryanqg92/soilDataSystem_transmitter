/**
 * @file soil_sensor_reader.h
 * @brief Definiciones y funciones para la lectura del sensor de suelo NPK.
 * 
 * Este archivo contiene las definiciones de constantes y las declaraciones de funciones
 * necesarias para inicializar y procesar los datos del sensor de suelo NPK.
 * 
 * @author Leandro Quiroga
 * @date nov 2024
 *
 * 
 * @dependencies
 * - api_uart.h: Interfaz para la comunicación UART.
 * - crc_calculator.h: Utilidad para el cálculo de CRC.

 * 
 * @functions
 * - bool NPKInit(uart_t* uart): Inicializa el sensor NPK.
 * - void Task_processData(void* soilData): Procesa los datos recibidos del sensor de suelo.
 */


#ifndef SOIL_SENSOR_READER_H
#define SOIL_SENSOR_READER_H

#define TASK_PROCESS_DATA_TICKS (pdMS_TO_TICKS(1300ul))
#define NPK_RESPONSE_SIZE 19U
#define NPK_REQUEST_SIZE 8U
#define INIT_RESPONSE_SIZE 8U
#define SOIL_SENSOR_BAUDRATE 9600

#include "api_uart.h"
#include "crc_calculator.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Inicializa el sensor NPK
 * @param uart Puntero al manejador UART
 * @return true si la inicialización es exitosa, false en caso contrario
 */
bool NPKInit(uart_t* uart);

/**
 * @brief Procesa los datos recibidos del sensor de suelo
 * @param soilData Puntero a la estructura de datos que contendrá los datos procesados del suelo
 */
void Task_processData(void* soilData);

#endif // SOIL_SENSOR_READER_H
