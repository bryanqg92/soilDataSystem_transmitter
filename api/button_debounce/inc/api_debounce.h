
/*
 * API_debounce.c
 *
 *  Created on: Mar 21, 2024
 *      Author: lean
 */

#ifndef API_DEBOUNCE_H
#define API_DEBOUNCE_H

#include <stdbool.h>
#include <stdint.h>

#include "api_delay.h"

/**
 * @brief Estados del estado de la máquina de estado del debounce.
 */
typedef enum
{
    BUTTON_UP,
    BUTTON_FALLING,
    BUTTON_DOWN,
    BUTTON_RAISING
} debounceState_t;

typedef struct
{
    debounceState_t buttonState;
    delay_t debounceHandler;
    delay_t longPressHandler;
    uint8_t gpio;
    uint32_t debounceTime;
} button_app_t;

/**
 * @file API_Debounce.h
 * @brief Header file for debounce functionality.
 */

/**
 * @brief Initialize the debounce Finite State Machine (FSM).
 * @param debounceTime The debounce time in milliseconds.
 */
void debounceFSM_init(button_app_t* button);

/**
 * @brief Update the debounce FSM. This function should be called periodically.
 */
void buttonFSM_update(button_app_t* button);

#endif /* API_DEBOUNCE_H_ */