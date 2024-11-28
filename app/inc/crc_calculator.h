#ifndef CRC_CALCULATOR_H
#define CRC_CALCULATOR_H

#include <stdbool.h>
#include <stdint.h>

uint16_t ModbusCRC(uint8_t* buf, int len);

bool verifyCRC(uint8_t* buf, int len);
void appendCRC(uint8_t* buf, int len);

#endif
