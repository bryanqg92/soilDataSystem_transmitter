#ifndef LOGGER_H
#define LOGGER_H

#include "esp_log.h"

// Define para habilitar (1) o deshabilitar (0) los logs
#define LOG_APP 1 // Cambia a 0 para deshabilitar todos los logs

#if LOG_APP
#define LOG(tag, level, fmt, ...) ESP_LOG_LEVEL(level, tag, fmt, ##__VA_ARGS__)
#else
// Si LOG_APP es 0, define la macro como una expresión vacía que no hace nada
#define LOG(tag, level, fmt, ...) ((void)0)
#endif

#endif // LOGGER_H
