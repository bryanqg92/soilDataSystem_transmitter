#include "api_gnss.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>

static void adjust_to_utc_minus_5(uint8_t* hour, uint8_t* day, uint8_t* month, uint16_t* year)
{
    if (*hour < 5)
    {
        *hour = *hour + 24 - 5;
        // Ajuste de fecha al día anterior
        if (*day > 1)
        {
            (*day)--;
        }
        else
        {
            // Si es primer día del mes
            if (*month > 1)
            {
                (*month)--;
                // Días por mes (1-12)
                const uint8_t days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                *day = days_in_month[*month - 1];
                // Ajuste para año bisiesto en febrero
                if (*month == 2 && *year % 4 == 0 && (*year % 100 != 0 || *year % 400 == 0))
                {
                    (*day)++;
                }
            }
            else
            {
                // Si es 1 de enero
                *month = 12;
                *day = 31;
                (*year)--;
            }
        }
    }
    else
    {
        *hour = *hour - 5;
    }
}

static float convert_nmea_to_decimal_degrees(const char* value, char direction)
{
    // Para longitud, los primeros 3 dígitos son grados (074)
    // Para latitud, los primeros 2 dígitos son grados (04)
    int deg_length = (direction == 'E' || direction == 'W') ? 3 : 2;

    char deg_str[4] = {0};
    strncpy(deg_str, value, deg_length);

    float degrees = atof(deg_str);
    float minutes = atof(value + deg_length);
    float decimal = degrees + minutes / 60.0f;

    return (direction == 'S' || direction == 'W') ? -decimal : decimal;
}

static bool parse_rmc_sentence(const char* sentence, GNSSData_t* data)
{
    char* token = strtok((char*)sentence, ",");
    char* next_token;
    int field = 0;

    while (token != NULL)
    {
        switch (field)
        {
        case 1: // Time
            if (strlen(token) >= 6)
            {
                data->hour = (token[0] - '0') * 10 + (token[1] - '0');
                data->minute = (token[2] - '0') * 10 + (token[3] - '0');
            }
            break;
        case 3: // Latitude
            if (strlen(token) > 0)
            {
                next_token = strtok(NULL, ",");
                if (next_token != NULL)
                {
                    data->latitude = convert_nmea_to_decimal_degrees(token, next_token[0]);
                }
                else
                {
                    data->latitude = 0.0f;
                }
            }
            else
            {
                data->latitude = 0.0f;
            }
            field++;
            break;
        case 5: // Longitude
            if (strlen(token) > 0)
            {
                next_token = strtok(NULL, ",");
                if (next_token != NULL)
                {
                    data->longitude = convert_nmea_to_decimal_degrees(token, next_token[0]);
                }
                else
                {
                    data->longitude = 0.0f;
                }
            }
            else
            {
                data->longitude = 0.0f;
            }
            field++;
            break;
        case 8: // Date (DDMMYY) - Ajustado a la posición correcta
            if (strlen(token) == 6)
            {
                data->day = (token[0] - '0') * 10 + (token[1] - '0');
                data->month = (token[2] - '0') * 10 + (token[3] - '0');
                data->year = (token[4] - '0') * 10 + (token[5] - '0');
            }
            break;
        }
        token = strtok(NULL, ",");
        field++;
    }

    adjust_to_utc_minus_5(&data->hour, &data->day, &data->month, &data->year);
    return true;
}

static bool parse_gga_sentence(const char* sentence, GNSSData_t* data)
{
    if (sentence == NULL || data == NULL)
    {
        ESP_LOGE("GNSS", "Null pointer in parse_gga_sentence: sentence=%p, data=%p", sentence,
                 data);
        return false;
    }
    char* token = strtok((char*)sentence, ",");
    int field = 0;

    while (token != NULL)
    {
        switch (field)
        {
        case 7: // Satellites used
            data->satellites_used = atoi(token);
            break;
        case 9: // Altitude
            if (strlen(token) > 0)
            {
                data->altitude = atof(token);
            }
            break;
        case 6: // Fix status
            data->fix_status = (atoi(token) > 0) ? 1 : 0;
            break;
        }
        token = strtok(NULL, ",");
        field++;
    }
    return true;
}

bool parse_gnss_buffer(const uint8_t* buffer, uint16_t length, GNSSData_t* gnss_data)
{
    if (buffer == NULL || gnss_data == NULL)
    {
        ESP_LOGE("GNSS", "Null pointer in parse_gnss_buffer: buffer=%p, gnss_data=%p", buffer,
                 gnss_data);
        return false;
    }
    char* line_start = (char*)buffer;
    char* line_end;
    bool rmc_parsed = false;
    bool gga_parsed = false;

    while ((line_end = strstr(line_start, "\r\n")) != NULL)
    {
        *line_end = '\0';

        if (strncmp(line_start, "$GNRMC", 6) == 0)
        {
            rmc_parsed = parse_rmc_sentence(line_start, gnss_data);
        }
        else if (strncmp(line_start, "$GNGGA", 6) == 0)
        {
            gga_parsed = parse_gga_sentence(line_start, gnss_data);
        }

        if (rmc_parsed && gga_parsed)
        {
            return true;
        }

        line_start = line_end + 2;
    }

    return false;
}
