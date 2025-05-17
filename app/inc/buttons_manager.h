#ifndef BUTTONS_MANAGER_H
#define BUTTONS_MANAGER_H

#include "api_debounce.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DEBOUNCE_BTN_TIME 30u

// #define MODE_BUTTON_GPIO 7

typedef struct
{
    button_app_t* cat0_btn;
    button_app_t* cat1_btn;
} buttons_params_t;

void buttons_init(button_app_t* cat0_btn, button_app_t* cat1_btn);
void Task_buttons(void* pvParameters);
esp_err_t capture_and_send_data(bool category);

#endif // BUTTONS_MANAGER_H