#include "crc_calculator.h"
#include "logger.h"

/**
 * @brief Calcula el CRC (Cyclic Redundancy Check) utilizando el algoritmo Modbus.
 *
 * Esta función toma un buffer de datos y su longitud, y calcula el valor CRC de 16 bits
 * correspondiente utilizando el algoritmo Modbus. El CRC es utilizado para detectar errores
 * en la transmisión de datos.
 *
 * @param buf Puntero al buffer de datos sobre el cual se calculará el CRC.
 * @param len Longitud del buffer de datos.
 * @return El valor CRC de 16 bits calculado.
 */
uint16_t ModbusCRC(uint8_t* buf, int len)
{
    uint16_t crc = 0xFFFF;

    for (int pos = 0; pos < len; pos++)
    {
        crc ^= (uint16_t)buf[pos];

        for (int i = 8; i != 0; i--)
        {
            if ((crc & 0x0001) != 0)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
                crc >>= 1;
        }
    }

    return crc;
}

/**
 * @brief Verifica el CRC de un buffer de datos.
 *
 * Esta función calcula el CRC de los datos proporcionados en el buffer y lo compara
 * con el CRC recibido que se encuentra en los últimos dos bytes del buffer.
 *
 * @param buf Puntero al buffer de datos que contiene los datos y el CRC recibido.
 * @param len Longitud del buffer de datos, incluyendo los dos bytes del CRC recibido.
 * @return true si el CRC calculado coincide con el CRC recibido, false en caso contrario.
 */
bool verifyCRC(uint8_t* buf, int len)
{
    uint16_t calculatedCRC = ModbusCRC(buf, len - 2);
    uint16_t receivedCRC = (buf[len - 1] << 8) | buf[len - 2];
    return calculatedCRC == receivedCRC;
}

/**
 * @brief Añade el CRC al final de un buffer.
 *
 * Esta función calcula el CRC utilizando el algoritmo Modbus y lo añade
 * al final del buffer proporcionado. El CRC se almacena en los dos últimos
 * bytes del buffer.
 *
 * @param buf Puntero al buffer de datos al que se le añadirá el CRC.
 * @param len Longitud del buffer de datos (sin incluir el espacio para el CRC).
 */
void appendCRC(uint8_t* buf, int len)
{
    uint16_t crc = ModbusCRC(buf, len);
    buf[len] = crc & 0xFF;
    buf[len + 1] = (crc >> 8) & 0xFF;
}
