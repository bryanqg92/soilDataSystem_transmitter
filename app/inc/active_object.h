#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef enum {
   CMD_SAVE_CAT0,
   CMD_SAVE_CAT1
} command_t;

typedef struct {
   command_t cmd;
   void* data;
} active_object_msg_t;

void active_object_init(void);
void active_object_send_cmd(command_t cmd, void* data);