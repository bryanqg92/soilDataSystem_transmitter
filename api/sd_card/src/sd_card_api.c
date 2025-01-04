// sd_card_api.c
#include "sd_card_api.h"
#include <string.h>
#include <sys/stat.h>

esp_err_t sd_create_file(sd_card_t* card, const char* path)
{
    FILE* f = fopen(path, "w");
    if (!f)
        return ESP_FAIL;
    fclose(f);
    return ESP_OK;
}

bool sd_file_exists(sd_card_t* card, const char* path)
{
    struct stat st;
    return (stat(path, &st) == 0);
}

esp_err_t sd_read_file(sd_card_t* card, const char* path, char* buffer, size_t max_size)
{
    FILE* f = fopen(path, "r");
    if (!f)
        return ESP_FAIL;

    size_t bytes = fread(buffer, 1, max_size - 1, f);
    buffer[bytes] = '\0';
    fclose(f);
    return ESP_OK;
}

esp_err_t sd_write_file(sd_card_t* card, const char* path, const char* data)
{
    FILE* f = fopen(path, "w");
    if (!f)
        return ESP_FAIL;

    fprintf(f, "%s", data);
    fclose(f);
    return ESP_OK;
}

esp_err_t sd_append_file(sd_card_t* card, const char* path, const char* data)
{
    FILE* f = fopen(path, "a");
    if (!f)
        return ESP_FAIL;

    fprintf(f, "%s", data);
    fclose(f);
    return ESP_OK;
}

int sd_count_lines(sd_card_t* card, const char* path)
{
    FILE* f = fopen(path, "r");
    if (!f)
        return -1;

    int lines = 0;
    int c;
    while ((c = fgetc(f)) != EOF)
    {
        if (c == '\n')
            lines++;
    }
    fclose(f);
    return lines + 1;
}