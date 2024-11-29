/**
 * @file crc_calculator.h
 * @brief Declaraciones de funciones para el cálculo y verificación de CRC.
 *
 * Este archivo contiene las declaraciones de funciones utilizadas para calcular y verificar
 * el CRC (Cyclic Redundancy Check) en el contexto de la comunicación Modbus.
 *
 * @author Leandro Quiroga
 * @date nov 2024
 */

#ifndef CRC_CALCULATOR_H
#define CRC_CALCULATOR_H

#include <stdbool.h>
#include <stdint.h>

/*@brief Calcula el CRC Modbus para un buffer dado.
 *
 * Esta función toma un buffer de datos y su longitud, y calcula el CRC Modbus correspondiente.
 *
 * @param buf Puntero al buffer de datos.
 * @param len Longitud del buffer de datos.
 * @return El valor del CRC calculado.
 */
uint16_t ModbusCRC(uint8_t* buf, int len);

/** * @brief Verifica el CRC de un buffer dado. *
 * Esta función toma un buffer de datos y su longitud, y verifica si el CRC incluido en el buffer es correcto.
 *
 * @param buf Puntero al buffer de datos.
 * @param len Longitud del buffer de datos.
 * @return true si el CRC es correcto, false en caso contrario.
 */
bool verifyCRC(uint8_t* buf, int len);
/** * @brief Añade el CRC a un buffer dado.
 *
 * Esta función toma un buffer de datos y su longitud, calcula el CRC correspondiente y lo añade al final del buffer.
 *
 * @param buf Puntero al buffer de datos.
 * @param len Longitud del buffer de datos.
 */
void appendCRC(uint8_t* buf, int len);

#endif /* CRC_CALCULATOR_H */