#include "crc_calculator.h"
#include "logger.h"

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

bool verifyCRC(uint8_t* buf, int len)
{
    uint16_t calculatedCRC = ModbusCRC(buf, len - 2);
    uint16_t receivedCRC = (buf[len - 1] << 8) | buf[len - 2];
    return calculatedCRC == receivedCRC;
}

void appendCRC(uint8_t* buf, int len)
{
    uint16_t crc = ModbusCRC(buf, len);
    buf[len] = crc & 0xFF;
    buf[len + 1] = (crc >> 8) & 0xFF;
}
