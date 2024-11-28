#ifndef API_GNSS_H
#define API_GNSS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{

    float latitude;
    float longitude;
    float altitude;

    uint8_t hour;
    uint8_t minute;

    uint8_t day;
    uint8_t month;
    uint16_t year;

    uint8_t fix_status;
    uint8_t satellites_used;

} GNSSData_t;

bool parse_gnss_buffer(const uint8_t* buffer, uint16_t length, GNSSData_t* gnss_data);
#endif // API_GNSS_H
