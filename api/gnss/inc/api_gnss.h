#ifndef API_GNSS_H
#define API_GNSS_H

#include <stdbool.h>
#include <stdint.h>

#pragma pack(push, 1)
typedef struct
{
    float latitude;
    float longitude;
    uint8_t hour;
    uint8_t minute;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t satellites_used;
    float altitude;
    uint8_t fix_status; // 0: No fix, 1: Fix
} GNSSData_t;
#pragma pack(pop)

bool parse_gnss_buffer(const uint8_t* buffer, uint16_t length, GNSSData_t* gnss_data);
#endif // API_GNSS_H
