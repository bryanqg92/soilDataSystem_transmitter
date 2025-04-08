#include "api_gnss.h"
#include "esp_log.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

// Tabla de días por mes (no bisiesto)
static const uint8_t DAYS_PER_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static inline bool is_leap_year(uint16_t year)
{
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

static void adjust_to_utc_minus_5(uint8_t* hour, uint8_t* day, uint8_t* month, uint16_t* year)
{
    if (*hour < 5)
    {
        *hour += 19; // Equivalente a 24 - 5
        if (*day > 1)
        {
            (*day)--;
        }
        else
        {
            // Retroceder mes
            if (*month > 1)
            {
                (*month)--;
                uint8_t days = DAYS_PER_MONTH[*month - 1];
                if (*month == 2 && is_leap_year(*year))
                    days++;
                *day = days;
            }
            else
            {
                // Retroceder año
                *month = 12;
                *day = 31;
                (*year)--;
            }
        }
    }
    else
    {
        *hour -= 5;
    }
}

static float parse_coordinate(const char* nmea_value, char direction)
{
    // Validación básica
    if (nmea_value == NULL || strlen(nmea_value) < 5)
        return 0.0f;

    const int grados_digits = (direction == 'N' || direction == 'S') ? 2 : 3;
    char grados_str[4] = {0};

    // Extraer solo los dígitos de grados
    strncpy(grados_str, nmea_value, grados_digits);
    int grados = atoi(grados_str);

    // Los minutos son el resto de la cadena después de los grados
    float minutos = atof(nmea_value + grados_digits);

    // Cálculo final
    float decimal = grados + (minutos / 60.0f);
    return (direction == 'S' || direction == 'W') ? -decimal : decimal;
}

static bool parse_time(const char* str, size_t len, uint8_t* hour, uint8_t* minute)
{
    if (len != 6)
        return false;

    *hour = (str[0] - '0') * 10 + (str[1] - '0');
    *minute = (str[2] - '0') * 10 + (str[3] - '0');
    return true;
}

static bool parse_date(const char* str, size_t len, uint8_t* day, uint8_t* month, uint16_t* year)
{
    if (len != 6)
        return false;

    *day = (str[0] - '0') * 10 + (str[1] - '0');
    *month = (str[2] - '0') * 10 + (str[3] - '0');
    *year = (str[4] - '0') * 10 + (str[5] - '0') + 2000;
    return true;
}

static bool parse_rmc(const char* sentence, size_t length, GNSSData_t* data)
{
    const char* fields[12];
    uint8_t field_count = 0;
    const char* end = sentence + length;

    // 1. Dividir en campos
    const char* field_start = sentence;
    for (const char* p = sentence; p <= end && field_count < 12; p++)
    {
        if (*p == ',' || *p == '*' || p == end)
        {
            fields[field_count++] = field_start;
            field_start = p + 1;
        }
    }

    // 2. Validar campos mínimos
    if (field_count < 10)
    {
        ESP_LOGE("GNSS", "RMC inválida: solo %d campos", field_count);
        return false;
    }

    // 3. Parsear hora (campo 1: HHMMSS)
    if (!parse_time(fields[1], 6, &data->hour, &data->minute))
    {
        ESP_LOGE("GNSS", "Hora RMC inválida: %.*s", 6, fields[1]);
        return false;
    }

    // 4. Parsear fecha (campo 9: DDMMYY)
    if (!parse_date(fields[9], 6, &data->day, &data->month, &data->year))
    {
        ESP_LOGE("GNSS", "Fecha RMC inválida: %.*s", 6, fields[9]);
        return false;
    }

    // 5. Ajuste UTC-5 (usa los valores recién parseados)
    adjust_to_utc_minus_5(&data->hour, &data->day, &data->month, &data->year);

    // 6. Coordenadas
    data->latitude = parse_coordinate(fields[3], fields[4][0]);
    data->longitude = parse_coordinate(fields[5], fields[6][0]);

    // 7. Validar coordenadas
    if (data->latitude < -90.0f || data->latitude > 90.0f || data->longitude < -180.0f ||
        data->longitude > 180.0f)
    {
        ESP_LOGE("GNSS", "Coordenadas inválidas: Lat=%.5f, Lon=%.5f", data->latitude,
                 data->longitude);
        return false;
    }

    return true;
}
static bool parse_gga(const char* sentence, size_t length, GNSSData_t* data)
{
    const char* fields[15]; // Campos de la sentencia GGA
    uint8_t field_count = 0;
    const char* end = sentence + length;

    // 1. Dividir la sentencia en campos
    const char* field_start = sentence;
    for (const char* p = sentence; p <= end && field_count < 15; p++)
    {
        if (*p == ',' || *p == '*' || p == end)
        {
            fields[field_count++] = field_start;
            field_start = p + 1;
        }
    }

    // 2. Validar campos esenciales
    if (field_count < 10)
    {
        ESP_LOGE("GNSS", "GGA inválida: solo %d campos", field_count);
        return false;
    }

    // 3. Parsear estado de fijación (campo 6)
    if (fields[6][0])
        data->fix_status = (atoi(fields[6]) > 0) ? 1 : 0;

    // 4. Satélites usados (campo 7)
    if (fields[7][0])
        data->satellites_used = atoi(fields[7]);

    // 5. Altitud (campo 9)
    if (fields[9][0])
        data->altitude = atof(fields[9]);

    // 6. Parsear latitud (campos 2 y 3)
    if (fields[2][0] && fields[3][0])
    {
        data->latitude = parse_coordinate(fields[2], fields[3][0]);
        if (data->latitude < -90.0f || data->latitude > 90.0f)
        {
            ESP_LOGE("GNSS", "Latitud GGA inválida: %.5f", data->latitude);
            return false;
        }
    }

    // 7. Parsear longitud (campos 4 y 5)
    if (fields[4][0] && fields[5][0])
    {
        data->longitude = parse_coordinate(fields[4], fields[5][0]);
        if (data->longitude < -180.0f || data->longitude > 180.0f)
        {
            ESP_LOGE("GNSS", "Longitud GGA inválida: %.5f", data->longitude);
            return false;
        }
    }

    return true;
}

bool parse_gnss_buffer(const uint8_t* buffer, uint16_t length, GNSSData_t* data)
{
    if (!buffer || !data)
        return false;

    const char* start = (const char*)buffer;
    const char* end = start + length;
    bool rmc_parsed = false, gga_parsed = false;

    while (start < end)
    {
        // Buscar fin de línea
        const char* line_end = start;
        while (line_end < end &&
               !(*line_end == '\r' && (line_end + 1) < end && *(line_end + 1) == '\n'))
        {
            line_end++;
        }

        size_t line_length = line_end - start;

        if (line_length >= 6)
        {
            if (memcmp(start, "$GNRMC", 6) == 0)
            {
                rmc_parsed = parse_rmc(start, line_length, data);
            }
            else if (memcmp(start, "$GNGGA", 6) == 0)
            {
                gga_parsed = parse_gga(start, line_length, data);
            }
        }

        // Saltar CRLF
        start = line_end + (line_end < end - 1 && line_end[1] == '\n' ? 2 : 0);

        if (rmc_parsed && gga_parsed)
            break;
    }

    return rmc_parsed && gga_parsed;
}