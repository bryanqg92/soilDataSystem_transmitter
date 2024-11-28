#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include "api_uart.h"

/**
 * @struct SoilData_t
 * @brief Structure to hold various soil sensor measurements
 */
typedef struct
{
    float temperature;
    float moisture;
    uint16_t conductivity;
    float pH;
    uint16_t nitrogen;
    uint16_t phosphorus;
    uint16_t potassium;
    uart_t npk_port;
    uint8_t status;

} SoilData_t;

#endif // SHARED_DATA_H
/*
$CFGMSG,0,5,1*2B
*/
