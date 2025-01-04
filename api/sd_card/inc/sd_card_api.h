// sd_card_api.h
#ifndef SD_CARD_API_H
#define SD_CARD_API_H

#include "driver/spi_master.h"
#include "esp_err.h"

typedef struct
{
    spi_device_handle_t spi;
    const char* mount_point;
} sd_card_t;

esp_err_t sd_create_file(sd_card_t* card, const char* path);
bool sd_file_exists(sd_card_t* card, const char* path);
esp_err_t sd_read_file(sd_card_t* card, const char* path, char* buffer, size_t max_size);
esp_err_t sd_write_file(sd_card_t* card, const char* path, const char* data);
esp_err_t sd_append_file(sd_card_t* card, const char* path, const char* data);
int sd_count_lines(sd_card_t* card, const char* path);

#endif