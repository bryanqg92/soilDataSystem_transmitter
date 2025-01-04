/*
 * pp.c
 *
 *  Created on: Mar 14, 2024
 *      Author: lean
 */

#include "api_delay.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void Error_Handler(void)
{
    while (1)
    {
    }
}

void delayInit(delay_t* delay, tick_t duration)
{
    if (duration <= 0)
    {
        Error_Handler();
    }
    else
    {
        delay->duration = duration;
        delay->running = false;
    }
}

bool_t delayRead(delay_t* delay)
{
    bool state = false;
    if (delay != NULL)
    {
        if (!delay->running)
        {
            delay->startTime = xTaskGetTickCount();
            delay->running = true;
        }
        else
        {
            tick_t currentTime = xTaskGetTickCount();
            if (currentTime - delay->startTime >= delay->duration)
            {
                delay->running = false;
                state = true;
            }
        }
    }
    else
    {
        Error_Handler();
    }
    return state;
}

void delayWrite(delay_t* delay, tick_t duration)
{
    if (delay != NULL && duration > 0)
    {
        delay->duration = duration;
    }
    else
    {
        Error_Handler();
    }
}
