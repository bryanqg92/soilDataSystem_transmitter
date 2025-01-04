/*
 * API_debounce.c
 *
 *  Created on: Mar 21, 2024
 *      Author: lean
 */

/**
 * @file API_debounce.c
 * @brief Implementación del módulo de debounce.
 */

#include <stdbool.h>
#include <stdint.h>

#include "api_debounce.h"
#include "driver/gpio.h"

/**
 * @brief Inicializa la máquina de estado del debounce.
 *
 * Esta función inicializa la máquina de estado del debounce para un botón específico.
 *
 * @param debounceTime Tiempo de debounce en milisegundos.
 * @param button Puntero a la estructura del botón.
 * @param longPressedTime Tiempo para considerar una pulsación larga en milisegundos. Si es NULL, no
 * se inicializa el temporizador de pulsación larga.
 */
void debounceFSM_init(button_app_t* button)
{
    button->buttonState = BUTTON_UP;
    delayInit(&button->debounceHandler, button->debounceTime);
}

/**
 * @brief Actualiza la máquina de estado del debounce.
 */
void buttonFSM_update(button_app_t* button)
{

    bool_t buttonState = gpio_get_level(button->gpio);

    switch (button->buttonState)
    {

    case BUTTON_UP:

        if (buttonState == 0)
        {
            delayRead(&button->debounceHandler);
            button->buttonState = BUTTON_FALLING;
        }
        break;

    case BUTTON_FALLING:

        if (delayRead(&button->debounceHandler))
        {
            if (buttonState == 0)
            {
                button->buttonState = BUTTON_DOWN;
            }
            else
            {
                button->buttonState = BUTTON_UP;
            }
        }

        break;

    case BUTTON_DOWN:

        if (buttonState == 1)
        {
            delayRead(&button->debounceHandler);
            button->buttonState = BUTTON_RAISING;
        }
        break;

    case BUTTON_RAISING:

        if (delayRead(&button->debounceHandler))
        {
            if (buttonState == 1)
            {
                button->buttonState = BUTTON_UP;
            }
            else
            {
                button->buttonState = BUTTON_DOWN;
            }
        }
        break;

    default:
        break;
    }
}
