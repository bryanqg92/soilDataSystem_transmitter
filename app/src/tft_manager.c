#include "tft_manager.h"
#include "HT_st7735.h"
#include "app.h"
#include "logger.h"

#define ICON_WIDTH 7
#define ICON_HEIGHT 10

typedef struct
{
    int x1;
    int y1;
    int x2;
    int y2;
} tft_cords_t;

static const uint16_t SOIL_SENSOR_ICON[] = {0X0D, 0X69, 0X5A, 0X2C, 0X08, 0X08, 0X08, 0X2A, 0X7F};
static const uint16_t GPS_ICON[] = {0x1C, 0x3E, 0x7F, 0x63, 0x63, 0X77, 0X3E, 0X1C, 0X08, 0X08};
// static const uint16_t LORA_ICON[] = {0X22,0X49,0X49,0X22,0X00,0X08,0X08,0X08,0X1C,0X3E};

typedef enum
{
    MODE_REGION,
    GPS_ICON_REGION,
    SOIL_SENSOR_ICON_REGION,
    LORA_ICON_REGION,
    DATE_REGION,
    TIME_REGION,
    BATTERY_REGION,
    ALTITUDE_REGION,
    LATITUDE_REGION,
    LONGITUDE_REGION,
    TEMPERATURE_REGION,
    HUMIDITY_REGION,
    CONDUCTIVITY_REGION,
    PH_REGION,
    NITROGEN_REGION,
    PHOSPHORUS_REGION,
    POTASSIUM_REGION,
    TFT_REGION_COUNT
} tft_regions;

const tft_cords_t tft_region_coords[TFT_REGION_COUNT] = {
    [MODE_REGION] = {.x1 = 1, .y1 = 1, .x2 = 30, .y2 = 20},
    [GPS_ICON_REGION] = {.x1 = 31, .y1 = 1, .x2 = 44, .y2 = 20},
    [SOIL_SENSOR_ICON_REGION] = {.x1 = 45, .y1 = 1, .x2 = 58, .y2 = 20},
    [LORA_ICON_REGION] = {.x1 = 59, .y1 = 1, .x2 = 72, .y2 = 20},
    [DATE_REGION] = {.x1 = 70, .y1 = 1, .x2 = 150, .y2 = 10},
    [TIME_REGION] = {.x1 = 70, .y1 = 11, .x2 = 150, .y2 = 20},
    [BATTERY_REGION] = {.x1 = 151, .y1 = 1, .x2 = 160, .y2 = 20},
    [ALTITUDE_REGION] = {.x1 = 1, .y1 = 21, .x2 = 60, .y2 = 30},
    [PH_REGION] = {.x1 = 1, .y1 = 31, .x2 = 60, .y2 = 40},
    [LATITUDE_REGION] = {.x1 = 61, .y1 = 21, .x2 = 160, .y2 = 30},
    [LONGITUDE_REGION] = {.x1 = 61, .y1 = 31, .x2 = 160, .y2 = 40},
    [TEMPERATURE_REGION] = {.x1 = 1, .y1 = 41, .x2 = 50, .y2 = 50},
    [HUMIDITY_REGION] = {.x1 = 1, .y1 = 51, .x2 = 50, .y2 = 60},
    [CONDUCTIVITY_REGION] = {.x1 = 1, .y1 = 61, .x2 = 50, .y2 = 80},
    [NITROGEN_REGION] = {.x1 = 55, .y1 = 45, .x2 = 140, .y2 = 54},
    [PHOSPHORUS_REGION] = {.x1 = 55, .y1 = 55, .x2 = 140, .y2 = 64},
    [POTASSIUM_REGION] = {.x1 = 55, .y1 = 65, .x2 = 140, .y2 = 80}};

static void draw_icon(ST7735_Config* config, uint16_t x, uint16_t y, const uint16_t* icon,
                      uint16_t color);
static void write_tft_data(ST7735_Config* config, const char* data, const tft_cords_t* cords,
                           uint16_t color, uint16_t bgcolor, FontDef font);
static void GNSSDataToTFT(GNSSData_t* gnss_data, TFTElements_t* tft_elements);
static void SoilDataToTFT(SoilData_t* soil_data, TFTElements_t* tft_elements);

void Task_TFTDisplay(void* pvParameters)
{

    TFTElements_t* tft_elements = (TFTElements_t*)pvParameters;

    tft_elements->tft_config = (ST7735_Config){.width = ST7735_WIDTH,
                                               .height = ST7735_HEIGHT,
                                               .x_start = ST7735_XSTART,
                                               .y_start = ST7735_YSTART,
                                               .spi_dev = tft_elements->tft_host.spi_handle,
                                               .cs_pin = tft_elements->tft_host.cs_pin,
                                               .dc_pin = tft_elements->tft_host.dc_pin,
                                               .led_k_pin = tft_elements->tft_host.led_k_pin,
                                               .rst_pin = tft_elements->tft_host.rst_pin

    };

    GNSSData_t gnss_task_data;
    SoilData_t soil_task_data;

    st7735_init(&tft_elements->tft_config);
    st7735_fill_screen(&tft_elements->tft_config, ST7735_BLACK);

    // char temp_data_buffer[20];
    while (1)
    {

        // recibir de las colas
        if (xQueueReceive(xQueueGNSSData, &gnss_task_data, 0) == pdTRUE)
        {
            GNSSDataToTFT(&gnss_task_data, tft_elements);
        }

        if (xQueueReceive(xQueueSoilData, &soil_task_data, 0) == pdTRUE)
        {
            SoilDataToTFT(&soil_task_data, tft_elements);
        }
        // write_tft_data(&tft_elements->tft_config, "EXT", &tft_region_coords[MODE_REGION],
        // ST7735_WHITE, ST7735_BLACK, Font_7x10);
        //// Draw GPS icon
        //
        //// Draw Soil Sensor icon
        // draw_icon(&tft_elements->tft_config, tft_region_coords[SOIL_SENSOR_ICON_REGION].x1,
        // tft_region_coords[SOIL_SENSOR_ICON_REGION].y1, SOIL_SENSOR_ICON, ST7735_GREEN);
        //// Draw LoRa icon
        // draw_icon(&tft_elements->tft_config, tft_region_coords[LORA_ICON_REGION].x1,
        // tft_region_coords[LORA_ICON_REGION].y1, LORA_ICON, ST7735_CYAN);

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }
}

// Function to draw an icon at a specific position
static void draw_icon(ST7735_Config* config, uint16_t x, uint16_t y, const uint16_t* icon,
                      uint16_t color)
{
    for (int i = 0; i < ICON_HEIGHT; i++)
    {
        uint16_t row = icon[i];
        for (int j = 0; j < ICON_WIDTH; j++)
        {
            uint16_t pixel_color = (row & (1 << (ICON_WIDTH - 1 - j)))
                                       ? color
                                       : 0x0000; // Use provided color for 1, Black for 0
            // Draw the pixel at (x + j, y + i)
            st7735_draw_pixel(config, x + j, y + i, pixel_color);
        }
    }
}

static void write_tft_data(ST7735_Config* config, const char* data, const tft_cords_t* cords,
                           uint16_t color, uint16_t bgcolor, FontDef font)
{
    st7735_write_string(config, cords->x1, cords->y1, data, font, color, bgcolor);
}

static void GNSSDataToTFT(GNSSData_t* gnss_data, TFTElements_t* tft_elements)
{
    char temp_data_buffer[40];

    if (gnss_data->fix_status == 1)
    {
        draw_icon(&tft_elements->tft_config, tft_region_coords[GPS_ICON_REGION].x1,
                  tft_region_coords[GPS_ICON_REGION].y1, GPS_ICON, ST7735_GREEN);

        // Write date
        sprintf(temp_data_buffer, "%02d/%02d/%d ", gnss_data->day, gnss_data->month,
                gnss_data->year);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer, &tft_region_coords[DATE_REGION],
                       ST7735_WHITE, ST7735_BLACK, Font_7x10);
        // Write time
        sprintf(temp_data_buffer, "%02d:%02d    ", gnss_data->hour, gnss_data->minute);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer, &tft_region_coords[TIME_REGION],
                       ST7735_WHITE, ST7735_BLACK, Font_7x10);
        // Write altitude
        sprintf(temp_data_buffer, "A: %d", (uint16_t)gnss_data->altitude);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                       &tft_region_coords[ALTITUDE_REGION], ST7735_WHITE, ST7735_BLACK, Font_7x10);
        // Write latitude
        sprintf(temp_data_buffer, " Lt: %.6f", gnss_data->latitude);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                       &tft_region_coords[LATITUDE_REGION], ST7735_WHITE, ST7735_BLACK, Font_7x10);
        // Write longitude
        sprintf(temp_data_buffer, " Ln: %.5f", gnss_data->longitude);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                       &tft_region_coords[LONGITUDE_REGION], ST7735_WHITE, ST7735_BLACK, Font_7x10);
    }
    else
    {
        static TickType_t last_toggle_time = 0;
        TickType_t current_time = xTaskGetTickCount();
        static bool toggle = false;

        if ((current_time - last_toggle_time) >= pdMS_TO_TICKS(100))
        {
            toggle = !toggle;
            last_toggle_time = current_time;
        }

        uint16_t color = toggle ? ST7735_RED : ST7735_BLACK;
        draw_icon(&tft_elements->tft_config, tft_region_coords[GPS_ICON_REGION].x1,
                  tft_region_coords[GPS_ICON_REGION].y1, GPS_ICON, color);

        // Write default date
        write_tft_data(&tft_elements->tft_config, "00/00/00", &tft_region_coords[DATE_REGION],
                       ST7735_WHITE, ST7735_BLACK, Font_7x10);
        // Write default time
        write_tft_data(&tft_elements->tft_config, "00:00", &tft_region_coords[TIME_REGION],
                       ST7735_WHITE, ST7735_BLACK, Font_7x10);
        // Write default altitude
        write_tft_data(&tft_elements->tft_config, "A: 0   ", &tft_region_coords[ALTITUDE_REGION],
                       ST7735_WHITE, ST7735_BLACK, Font_7x10);
        // Write default latitude
        write_tft_data(&tft_elements->tft_config, "Lt: 000.00000",
                       &tft_region_coords[LATITUDE_REGION], ST7735_WHITE, ST7735_BLACK, Font_7x10);
        // Write default longitude
        write_tft_data(&tft_elements->tft_config, "Ln: 000.00000",
                       &tft_region_coords[LONGITUDE_REGION], ST7735_WHITE, ST7735_BLACK, Font_7x10);
    }
}

static void SoilDataToTFT(SoilData_t* soil_data, TFTElements_t* tft_elements)
{
    char temp_data_buffer[40];
    if (soil_data->status == 1)
    {
        draw_icon(&tft_elements->tft_config, tft_region_coords[SOIL_SENSOR_ICON_REGION].x1,
                  tft_region_coords[SOIL_SENSOR_ICON_REGION].y1, SOIL_SENSOR_ICON, ST7735_CYAN);
    }
    else
    {
        static TickType_t last_toggle_time = 0;
        TickType_t current_time = xTaskGetTickCount();
        static bool toggle = false;

        if ((current_time - last_toggle_time) >= pdMS_TO_TICKS(100))
        {
            toggle = !toggle;
            last_toggle_time = current_time;
        }

        uint16_t color = toggle ? ST7735_RED : ST7735_BLACK;
        draw_icon(&tft_elements->tft_config, tft_region_coords[SOIL_SENSOR_ICON_REGION].x1,
                  tft_region_coords[SOIL_SENSOR_ICON_REGION].y1, SOIL_SENSOR_ICON, ST7735_RED);
    }
    // Write temperature
    sprintf(temp_data_buffer, "T: %.1f", soil_data->temperature);
    write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                   &tft_region_coords[TEMPERATURE_REGION], ST7735_WHITE, ST7735_BLACK, Font_7x10);
    // Write humidity
    sprintf(temp_data_buffer, "H: %d", (uint8_t)soil_data->moisture);
    write_tft_data(&tft_elements->tft_config, temp_data_buffer, &tft_region_coords[HUMIDITY_REGION],
                   ST7735_WHITE, ST7735_BLACK, Font_7x10);
    // Write conductivity
    sprintf(temp_data_buffer, "C: %d", (uint16_t)soil_data->conductivity);
    write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                   &tft_region_coords[CONDUCTIVITY_REGION], ST7735_WHITE, ST7735_BLACK, Font_7x10);
    // Write pH
    sprintf(temp_data_buffer, "pH: %.1f", soil_data->pH);
    write_tft_data(&tft_elements->tft_config, temp_data_buffer, &tft_region_coords[PH_REGION],
                   ST7735_WHITE, ST7735_BLACK, Font_7x10);
    // Write nutrients
    sprintf(temp_data_buffer, "N: %d", soil_data->nitrogen);
    write_tft_data(&tft_elements->tft_config, temp_data_buffer, &tft_region_coords[NITROGEN_REGION],
                   ST7735_WHITE, ST7735_BLACK, Font_7x10);
    sprintf(temp_data_buffer, "P: %d", soil_data->phosphorus);
    write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                   &tft_region_coords[PHOSPHORUS_REGION], ST7735_WHITE, ST7735_BLACK, Font_7x10);
    sprintf(temp_data_buffer, "K: %d", soil_data->potassium);
    write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                   &tft_region_coords[POTASSIUM_REGION], ST7735_WHITE, ST7735_BLACK, Font_7x10);
}
