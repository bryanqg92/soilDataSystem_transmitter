#include "api_gnss.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Ajusta la hora UTC a UTC-5 y realiza los ajustes necesarios en la fecha.
 *
 * Esta función toma una hora en formato UTC y la ajusta a la zona horaria UTC-5.
 * Si la hora ajustada resulta en un cambio de día, mes o año, la función realiza
 * los ajustes necesarios en la fecha.
 *
 * @param hour Puntero a la variable que contiene la hora en formato UTC (0-23).
 * @param day Puntero a la variable que contiene el día del mes (1-31).
 * @param month Puntero a la variable que contiene el mes del año (1-12).
 * @param year Puntero a la variable que contiene el año.
 *
 * @note La función considera los años bisiestos para ajustar correctamente el día
 *       en el mes de febrero.
 */
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

/**
 * @brief Convierte una coordenada NMEA en grados decimales.
 *
 * Esta función toma una cadena de caracteres que representa una coordenada en formato NMEA
 * y la convierte a un valor de grados decimales. La dirección (N, S, E, W) se utiliza para
 * determinar si el valor resultante debe ser positivo o negativo.
 *
 * @param value Cadena de caracteres que contiene la coordenada en formato NMEA.
 * @param direction Carácter que indica la dirección de la coordenada ('N' para norte, 'S' para sur,
 *                  'E' para este, 'W' para oeste).
 * @return El valor de la coordenada en grados decimales.
 */
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

/**
 * @brief Parsea una sentencia RMC (Recommended Minimum Specific GNSS Data) y extrae los datos GNSS.
 *
 * Esta función toma una sentencia RMC en formato NMEA y extrae la hora, latitud, longitud y fecha,
 * almacenando estos valores en una estructura GNSSData_t.
 *
 * @param[in] sentence La sentencia RMC en formato NMEA a ser parseada.
 * @param[out] data Estructura GNSSData_t donde se almacenarán los datos extraídos.
 * @return true si el parseo fue exitoso, false en caso contrario.
 *
 * @note La función ajusta la hora a UTC-5.
 *
 * @details
 * La función divide la sentencia RMC en tokens utilizando la coma (',') como delimitador.
 * Los campos de interés son:
 * - Campo 1: Hora (HHMMSS)
 * - Campo 3: Latitud (en formato NMEA)
 * - Campo 5: Longitud (en formato NMEA)
 * - Campo 8: Fecha (DDMMYY)
 *
 * La latitud y longitud se convierten a grados decimales utilizando la función convert_nmea_to_decimal_degrees.
 * La hora se ajusta a UTC-5 utilizando la función adjust_to_utc_minus_5.
 */
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

/**
 * @brief Parsea una sentencia GGA y extrae datos GNSS.
 *
 * Esta función toma una sentencia GGA en formato NMEA y extrae información relevante
 * como el número de satélites utilizados, la altitud y el estado de la fijación.
 *
 * @param[in] sentence Puntero a la cadena de caracteres que contiene la sentencia GGA.
 * @param[out] data Puntero a una estructura GNSSData_t donde se almacenarán los datos extraídos.
 *
 * @return true si el análisis se realizó correctamente, false si hubo un error (por ejemplo, punteros nulos).
 *
 * @note La función espera que la sentencia GGA esté separada por comas.
 * 
 * @warning La función modifica la cadena de entrada 'sentence' debido al uso de strtok.
 */
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

/**
 * @brief Parsea un buffer GNSS y extrae datos RMC y GGA.
 *
 * Esta función toma un buffer que contiene datos GNSS en formato NMEA y extrae
 * la información de las sentencias RMC y GGA, almacenándola en una estructura
 * GNSSData_t proporcionada por el usuario.
 *
 * @param buffer Puntero al buffer que contiene los datos GNSS en formato NMEA.
 * @param length Longitud del buffer.
 * @param gnss_data Puntero a la estructura GNSSData_t donde se almacenarán los datos extraídos.
 * @return true si ambas sentencias RMC y GGA fueron parseadas correctamente, false en caso contrario.
 *
 * @note La función espera que las sentencias NMEA estén separadas por "\r\n".
 * @note Si el buffer o gnss_data son NULL, la función registrará un error y retornará false.
 */
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
