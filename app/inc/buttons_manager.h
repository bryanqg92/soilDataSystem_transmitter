#ifndef BUTTONS_MANAGER_H
#define BUTTONS_MANAGER_H

#include "api_debounce.h"

#define DEBOUNCE_BTN_TIME 50u

// #define MODE_BUTTON_GPIO 7
#define CAT0_BUTTON_GPIO 6
#define CAT1_BUTTON_GPIO 5

typedef struct
{
    button_app_t* cat0_btn;
    button_app_t* cat1_btn;
} buttons_params_t;

void buttons_init(button_app_t* cat0_btn, button_app_t* cat1_btn);

void Task_buttons(void* pvParameters);

#endif // BUTTONS_MANAGER_H