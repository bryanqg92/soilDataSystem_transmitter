#ifndef API_DELAY_H_
#define API_DELAY_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef uint32_t tick_t;
typedef bool bool_t;

typedef struct
{
    tick_t startTime;
    tick_t duration;
    bool_t running;
} delay_t;

/**
 * @brief Inicializa una estructura de retardo.
 *
 * @param delay Puntero a la estructura de retardo.
 * @param duration Duración del retardo en ticks.
 */
void delayInit(delay_t* delay, tick_t duration);

/**
 * @brief Lee el estado actual del retardo.
 *
 * @param delay Puntero a la estructura de retardo.
 * @return true si el retardo ha finalizado, false en caso contrario.
 */
bool_t delayRead(delay_t* delay);

/**
 * @brief Establece la duración de un retardo existente.
 *
 * @param delay Puntero a la estructura de retardo.
 * @param duration Nueva duración del retardo en ticks.
 */
void delayWrite(delay_t* delay, tick_t duration);

#endif