#include "active_object.h"

static QueueHandle_t cmd_queue;

static void process_command(active_object_msg_t* msg)
{
    switch (msg->cmd)
    {
    case CMD_SAVE_CAT0:
        // Procesar categoría 0
        break;
    case CMD_SAVE_CAT1:
        // Procesar categoría 1
        break;
    }
}

static void active_object_task(void* params)
{
    active_object_msg_t msg;
    while (1)
    {
        if (xQueueReceive(cmd_queue, &msg, portMAX_DELAY))
        {
            process_command(&msg);
        }
    }
}

void active_object_init(void)
{
    cmd_queue = xQueueCreate(10, sizeof(active_object_msg_t));
    xTaskCreate(active_object_task, "AO_task", 2048, NULL, 5, NULL);
}

void active_object_send_cmd(command_t cmd, void* data)
{
    active_object_msg_t msg = {.cmd = cmd, .data = data};
    xQueueSend(cmd_queue, &msg, portMAX_DELAY);
}