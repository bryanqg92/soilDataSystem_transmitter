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
 * @brief Initialize the NPK sensor
 * @param uart Pointer to the UART handler
 * @return true if initialization is successful, false otherwise
 */
bool NPKInit(uart_t* uart);

/**
 * @brief Process the data received from the soil sensor
 * @param soilData Pointer to the data structure that will hold the processed soil data
 */
void Task_processData(void* soilData);

#endif // SOIL_SENSOR_READER_H
