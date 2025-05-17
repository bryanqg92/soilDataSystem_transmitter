#include "tft_manager.h"
#include "HT_st7735.h"
#include "app.h"
#include "battery_monitor.h"
#include "esp_log.h"

#define ICON_WIDTH 7
#define ICON_HEIGHT 10

typedef struct
{
    int x1;
    int y1;
    int x2;
    int y2;
} tft_cords_t;

const char* TFT_DISPLAY = "==> TFT DISPLAY";
static const uint16_t SOIL_SENSOR_ICON[] = {0X0D, 0X69, 0X5A, 0X2C, 0X08, 0X08, 0X08, 0X2A, 0X7F};
static const uint16_t GPS_ICON[] = {0x1C, 0x3E, 0x7F, 0x63, 0x63, 0X77, 0X3E, 0X1C, 0X08, 0X08};

static const uint16_t ICON_BATTERY_EMPTY[10] = {
    0b0111111111111100, 0b1100000000000100, 0b1100000000000100, 0b1100000000000111,
    0b1100000000000111, 0b1100000000000111, 0b1100000000000100, 0b1100000000000100,
    0b1100000000000100, 0b0111111111111100};

static const uint16_t ICON_BATTERY_FULL[10] = {
    0b0111111111111100, 0b1100000000000100, 0b1101101101100100, 0b1101101101100111,
    0b1101101101100111, 0b1101101101100111, 0b1101101101100100, 0b1101101101100100,
    0b1100000000000100, 0b0111111111111100};

static const uint16_t ICON_BATTERY_MEDIUM[10] = {
    0b0111111111111100, 0b1100000000000100, 0b1101101100000100, 0b1101101100000111,
    0b1101101100000111, 0b1101101100000111, 0b1101101100000100, 0b1101101100000100,
    0b1100000000000100, 0b0111111111111100};

static const uint16_t ICON_BATTERY_LOW[10] = {
    0b0111111111111100, 0b1100000000000100, 0b1101100000000100, 0b1101100000000111,
    0b1101100000000111, 0b1101100000000111, 0b1101100000000100, 0b1101100000000100,
    0b1100000000000100, 0b0111111111111100};

// static const uint16_t ICON_BATTERY_CHARGING[10] = {
//     0b0111111111111100, 0b1100000000000100, 0b1100000001100100, 0b1100000110000111,
//     0b1100011000000111, 0b1100000110000111, 0b1100011000000100, 0b1101100000000100,
//     0b1100000000000100, 0b0111111111111100};

// static const uint16_t LORA_ICON[] = {0X22,0X49,0X49,0X22,0X00,0X08,0X08,0X08,0X1C,0X3E};

/**
 * @file tft_manager.c
 * @brief Gestión de las regiones de la pantalla TFT.
 *
 * Este archivo contiene la definición de las regiones de la pantalla TFT y sus coordenadas.
 * Las regiones están definidas en la enumeración `tft_regions` y sus coordenadas están
 * almacenadas en el array `tft_region_coords`.
 *
 * Enumeración `tft_regions`: Regiones de la pantalla TFT. De los datos de la aplicación
 *
 * Estructura `tft_cords_t`:
 * - x1: Coordenada x inicial de la región.
 * - y1: Coordenada y inicial de la región.
 * - x2: Coordenada x final de la región.
 * - y2: Coordenada y final de la región.
 *
 * Array `tft_region_coords`:
 * - Almacena las coordenadas de cada región definida en `tft_regions`.
 */
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
    [BATTERY_REGION] = {.x1 = 145, .y1 = 1, .x2 = 160, .y2 = 20},
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
                      uint16_t color, uint16_t width, uint16_t height);
static void write_tft_data(ST7735_Config* config, const char* data, const tft_cords_t* cords,
                           uint16_t color, uint16_t bgcolor, FontDef font);
static void GNSSDataToTFT(GNSSData_t* gnss_data, TFTElements_t* tft_elements, bool blink_state);
static void SoilDataToTFT(SoilData_t* soil_data, TFTElements_t* tft_elements, bool blink_state);
static bool validate_soil_data(SoilData_t* soil_data);
static bool validate_gnss_data(GNSSData_t* gnss_data);
static void battery_icon(ST7735_Config* config);

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
                                               .rst_pin = tft_elements->tft_host.rst_pin};

    GNSSData_t gnss_task_data;
    SoilData_t soil_task_data;
    bool has_gnss_data = false;
    bool has_soil_data = false;
    bool blink_state = false;
    TickType_t last_blink_time = xTaskGetTickCount();

    ESP_LOGI(TFT_DISPLAY, "Initializing TFT display");
    st7735_init(&tft_elements->tft_config);
    st7735_fill_screen(&tft_elements->tft_config, ST7735_BLACK);
    ESP_LOGI(TFT_DISPLAY, "TFT display initialized successfully");
    uint8_t refresh_counter = 100;

    while (1)
    {
        if (++refresh_counter >= 60)
        {
            st7735_clear(&tft_elements->tft_config);
            refresh_counter = 0;
            battery_icon(&tft_elements->tft_config);
        }

        // Actualizar estado de parpadeo cada 500ms
        TickType_t current_time = xTaskGetTickCount();
        if ((current_time - last_blink_time) >= pdMS_TO_TICKS(500))
        {
            blink_state = !blink_state;
            last_blink_time = current_time;
        }

        // Intentar recibir datos GNSS sin bloqueo prolongado
        if (xQueueReceive(xQueueGNSSData, &gnss_task_data, 0) == pdTRUE)
        {
            has_gnss_data = validate_gnss_data(&gnss_task_data);
            ESP_LOGD(TFT_DISPLAY, "Received GNSS data, valid: %d", has_gnss_data);
        }

        // Intentar recibir datos del suelo sin bloqueo
        if (xQueueReceive(xQueueSoilData, &soil_task_data, 0) == pdTRUE)
        {
            has_soil_data = validate_soil_data(&soil_task_data);
            ESP_LOGD(TFT_DISPLAY, "Received Soil data, valid: %d", has_soil_data);
        }

        // Actualizar display con datos GNSS
        if (has_gnss_data)
        {
            GNSSDataToTFT(&gnss_task_data, tft_elements, blink_state);
        }
        else
        {
            // Pasar NULL para indicar que no tenemos datos válidos
            GNSSDataToTFT(NULL, tft_elements, blink_state);
        }

        // Actualizar display con datos de suelo
        if (has_soil_data)
        {
            SoilDataToTFT(&soil_task_data, tft_elements, blink_state);
        }
        else
        {
            // Pasar NULL para indicar que no tenemos datos válidos
            SoilDataToTFT(NULL, tft_elements, blink_state);
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Más responsivo pero sin sobrecargar el SPI
    }
}

/**
 * @brief Valida los datos del sensor de suelo
 * @param soil_data Puntero a los datos del sensor de suelo
 * @return true si los datos son válidos, false en caso contrario
 */
static bool validate_soil_data(SoilData_t* soil_data)
{
    if (soil_data == NULL)
        return false;

    // Validar rangos de los datos del sensor
    if (soil_data->temperature < -20.0f || soil_data->temperature > 80.0f)
        return false;
    if (soil_data->moisture < 0.0f || soil_data->moisture > 100.0f)
        return false;
    if (soil_data->conductivity < 0 || soil_data->conductivity > 20000)
        return false;
    if (soil_data->pH < 0.0f || soil_data->pH > 14.0f)
        return false;

    return true;
}

/**
 * @brief Valida los datos GNSS
 * @param gnss_data Puntero a los datos GNSS
 * @return true si los datos son válidos, false en caso contrario
 */
static bool validate_gnss_data(GNSSData_t* gnss_data)
{
    if (gnss_data == NULL)
        return false;

    // Comprobar rango básico de coordenadas
    if (gnss_data->latitude < -90.0f || gnss_data->latitude > 90.0f)
        return false;
    if (gnss_data->longitude < -180.0f || gnss_data->longitude > 180.0f)
        return false;

    // Comprobar valores razonables para fecha/hora
    if (gnss_data->year < 2000 || gnss_data->year > 2100)
        return false;
    if (gnss_data->month < 1 || gnss_data->month > 12)
        return false;
    if (gnss_data->day < 1 || gnss_data->day > 31)
        return false;
    if (gnss_data->hour > 23 || gnss_data->minute > 59)
        return false;

    return true;
}

/**
 * @brief Dibuja un ícono en una posición específica en la pantalla.
 *
 * Esta función toma una configuración de pantalla ST7735, coordenadas x e y,
 * un ícono representado como un array de uint16_t y un color, y dibuja el ícono
 * en la posición especificada en la pantalla.
 *
 * @param config Puntero a la configuración de la pantalla ST7735.
 * @param x Coordenada x donde se dibujará el ícono.
 * @param y Coordenada y donde se dibujará el ícono.
 * @param icon Puntero al array que representa el ícono a dibujar.
 * @param color Color que se usará para los píxeles del ícono.
 */
static void draw_icon(ST7735_Config* config, uint16_t x, uint16_t y, const uint16_t* icon,
                      uint16_t color, uint16_t width, uint16_t height)
{
    for (int i = 0; i < height; i++)
    {
        uint16_t line = icon[i];
        for (int j = 0; j < width; j++)
        {
            if (line & (1 << (width - 1 - j)))
            {
                st7735_draw_pixel(config, x + j, y + i, color);
            }
            else
            {
                st7735_draw_pixel(config, x + j, y + i, ST7735_BLACK);
            }
        }
    }
}

/**
 * @brief Escribe datos en la pantalla TFT.
 *
 * Esta función utiliza la configuración del controlador ST7735 para escribir
 * una cadena de texto en la pantalla TFT en las coordenadas especificadas,
 * con el color de texto y de fondo proporcionados, y utilizando la fuente
 * especificada.
 *
 * @param config Puntero a la configuración del controlador ST7735.
 * @param data Cadena de texto a escribir en la pantalla.
 * @param cords Puntero a una estructura que contiene las coordenadas (x1, y1)
 *              donde se escribirá el texto.
 * @param color Color del texto.
 * @param bgcolor Color de fondo del texto.
 * @param font Fuente a utilizar para el texto.
 */
static void write_tft_data(ST7735_Config* config, const char* data, const tft_cords_t* cords,
                           uint16_t color, uint16_t bgcolor, FontDef font)
{
    st7735_write_string(config, cords->x1, cords->y1, data, font, color, bgcolor);
}

/**
 * @brief Actualiza los elementos del TFT con los datos GNSS proporcionados.
 *
 * Esta función toma los datos GNSS y los muestra en la pantalla TFT. Si el estado de fijación
 * GNSS es válido (fix_status == 1), se muestran los datos reales de fecha, hora, altitud,
 * latitud y longitud. Si el estado de fijación no es válido, se muestra un icono de GPS en
 * rojo que parpadea y se muestran valores predeterminados.
 *
 * @param gnss_data Puntero a la estructura que contiene los datos GNSS. Puede ser NULL.
 * @param tft_elements Puntero a la estructura que contiene la configuración y elementos del TFT.
 * @param blink_state Estado actual del parpadeo (true=visible, false=invisible)
 */
static void GNSSDataToTFT(GNSSData_t* gnss_data, TFTElements_t* tft_elements, bool blink_state)
{
    char temp_data_buffer[40];

    if (gnss_data != NULL && gnss_data->fix_status == 1)
    {
        // Dibujar ícono GPS fijo (verde)
        draw_icon(&tft_elements->tft_config, tft_region_coords[GPS_ICON_REGION].x1,
                  tft_region_coords[GPS_ICON_REGION].y1, GPS_ICON, ST7735_GREEN, ICON_WIDTH,
                  ICON_HEIGHT);

        // Mostrar fecha y hora
        sprintf(temp_data_buffer, "%02d/%02d/%d", gnss_data->day, gnss_data->month,
                gnss_data->year);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer, &tft_region_coords[DATE_REGION],
                       ST7735_WHITE, ST7735_NAVY, Font_7x10);

        sprintf(temp_data_buffer, "%02d:%02d", gnss_data->hour, gnss_data->minute);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer, &tft_region_coords[TIME_REGION],
                       ST7735_WHITE, ST7735_NAVY, Font_7x10);

        // Mostrar altitud
        sprintf(temp_data_buffer, "A: %d", (uint16_t)gnss_data->altitude);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                       &tft_region_coords[ALTITUDE_REGION], ST7735_WHITE, ST7735_NAVY, Font_7x10);

        // Mostrar latitud
        sprintf(temp_data_buffer, "Lat: %.6f", gnss_data->latitude);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                       &tft_region_coords[LATITUDE_REGION], ST7735_WHITE, ST7735_NAVY, Font_7x10);

        // Mostrar longitud
        sprintf(temp_data_buffer, "Lon:%.6f", gnss_data->longitude);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                       &tft_region_coords[LONGITUDE_REGION], ST7735_WHITE, ST7735_NAVY, Font_7x10);
    }
    else
    {
        // Sin fix GPS, parpadear ícono rojo
        uint16_t color = blink_state ? ST7735_RED : ST7735_BLACK;
        draw_icon(&tft_elements->tft_config, tft_region_coords[GPS_ICON_REGION].x1,
                  tft_region_coords[GPS_ICON_REGION].y1, GPS_ICON, color, ICON_WIDTH, ICON_HEIGHT);

        // Mostrar valores predeterminados
        write_tft_data(&tft_elements->tft_config, "--/--/----", &tft_region_coords[DATE_REGION],
                       ST7735_RED, ST7735_BLACK, Font_7x10);
        write_tft_data(&tft_elements->tft_config, "--:--", &tft_region_coords[TIME_REGION],
                       ST7735_RED, ST7735_BLACK, Font_7x10);
        write_tft_data(&tft_elements->tft_config, "A: ---", &tft_region_coords[ALTITUDE_REGION],
                       ST7735_RED, ST7735_BLACK, Font_7x10);
        write_tft_data(&tft_elements->tft_config, "Lat: ---", &tft_region_coords[LATITUDE_REGION],
                       ST7735_RED, ST7735_BLACK, Font_7x10);
        write_tft_data(&tft_elements->tft_config, "Lon: ---", &tft_region_coords[LONGITUDE_REGION],
                       ST7735_RED, ST7735_BLACK, Font_7x10);
    }
}

/**
 * @brief Actualiza los elementos del TFT con los datos del suelo.
 *
 * Esta función toma los datos del suelo proporcionados y actualiza los elementos
 * correspondientes en la pantalla TFT.
 *
 * @param soil_data Puntero a la estructura SoilData_t que contiene los datos del suelo.
 * @param tft_elements Puntero a la estructura TFTElements_t.
 * @param blink_state Estado actual del parpadeo (true=visible, false=invisible)
 */
static void SoilDataToTFT(SoilData_t* soil_data, TFTElements_t* tft_elements, bool blink_state)
{
    char temp_data_buffer[40];

    if (soil_data != NULL && soil_data->status == 1)
    {
        // Sensor de suelo activo y funcionando
        draw_icon(&tft_elements->tft_config, tft_region_coords[SOIL_SENSOR_ICON_REGION].x1,
                  tft_region_coords[SOIL_SENSOR_ICON_REGION].y1, SOIL_SENSOR_ICON, ST7735_GREEN,
                  ICON_WIDTH, ICON_HEIGHT);

        // Mostrar todos los datos del suelo con el formato original
        sprintf(temp_data_buffer, "T: %.1f", soil_data->temperature);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                       &tft_region_coords[TEMPERATURE_REGION], ST7735_BLACK, ST7735_LIME,
                       Font_7x10);

        sprintf(temp_data_buffer, "H: %.1f", soil_data->moisture);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                       &tft_region_coords[HUMIDITY_REGION], ST7735_BLACK, ST7735_LIME, Font_7x10);

        sprintf(temp_data_buffer, "C: %d", (uint16_t)soil_data->conductivity);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                       &tft_region_coords[CONDUCTIVITY_REGION], ST7735_BLACK, ST7735_LIME,
                       Font_7x10);

        sprintf(temp_data_buffer, "pH: %.1f", soil_data->pH);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer, &tft_region_coords[PH_REGION],
                       ST7735_WHITE, ST7735_PURPLE, Font_7x10);

        sprintf(temp_data_buffer, "N: %d", soil_data->nitrogen);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                       &tft_region_coords[NITROGEN_REGION], ST7735_BLACK, ST7735_GOLD, Font_7x10);

        sprintf(temp_data_buffer, "P: %d", soil_data->phosphorus);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                       &tft_region_coords[PHOSPHORUS_REGION], ST7735_BLACK, ST7735_GOLD, Font_7x10);

        sprintf(temp_data_buffer, "K: %d", soil_data->potassium);
        write_tft_data(&tft_elements->tft_config, temp_data_buffer,
                       &tft_region_coords[POTASSIUM_REGION], ST7735_BLACK, ST7735_GOLD, Font_7x10);
    }
    else
    {
        // Sensor de suelo inactivo o sin datos válidos
        uint16_t color = blink_state ? ST7735_RED : ST7735_BLACK;
        draw_icon(&tft_elements->tft_config, tft_region_coords[SOIL_SENSOR_ICON_REGION].x1,
                  tft_region_coords[SOIL_SENSOR_ICON_REGION].y1, SOIL_SENSOR_ICON, color,
                  ICON_WIDTH, ICON_HEIGHT);

        // Mostrar valores predeterminados con el formato original
        write_tft_data(&tft_elements->tft_config, "T: ---", &tft_region_coords[TEMPERATURE_REGION],
                       ST7735_RED, ST7735_BLACK, Font_7x10);

        write_tft_data(&tft_elements->tft_config, "H: ---", &tft_region_coords[HUMIDITY_REGION],
                       ST7735_RED, ST7735_BLACK, Font_7x10);

        write_tft_data(&tft_elements->tft_config, "C: ---", &tft_region_coords[CONDUCTIVITY_REGION],
                       ST7735_RED, ST7735_BLACK, Font_7x10);

        write_tft_data(&tft_elements->tft_config, "pH: ---", &tft_region_coords[PH_REGION],
                       ST7735_RED, ST7735_BLACK, Font_7x10);

        write_tft_data(&tft_elements->tft_config, "N: ---", &tft_region_coords[NITROGEN_REGION],
                       ST7735_RED, ST7735_BLACK, Font_7x10);

        write_tft_data(&tft_elements->tft_config, "P: ---", &tft_region_coords[PHOSPHORUS_REGION],
                       ST7735_RED, ST7735_BLACK, Font_7x10);

        write_tft_data(&tft_elements->tft_config, "K: ---", &tft_region_coords[POTASSIUM_REGION],
                       ST7735_RED, ST7735_BLACK, Font_7x10);
    }
}

static void battery_icon(ST7735_Config* config)
{
    battery_level_t level = battery_monitor_update();
    switch (level)
    {
    case BATTERY_VERY_LOW:
        draw_icon(config, tft_region_coords[BATTERY_REGION].x1,
                  tft_region_coords[BATTERY_REGION].y1, ICON_BATTERY_EMPTY, ST7735_RED, 16,
                  ICON_HEIGHT);
        break;
    case BATTERY_LOW:
        draw_icon(config, tft_region_coords[BATTERY_REGION].x1,
                  tft_region_coords[BATTERY_REGION].y1, ICON_BATTERY_LOW, ST7735_ORANGE, 16,
                  ICON_HEIGHT);
        break;
    case BATTERY_MEDIUM:
        draw_icon(config, tft_region_coords[BATTERY_REGION].x1,
                  tft_region_coords[BATTERY_REGION].y1, ICON_BATTERY_MEDIUM, ST7735_YELLOW, 16,
                  ICON_HEIGHT);
        break;
    case BATTERY_FULL:
        draw_icon(config, tft_region_coords[BATTERY_REGION].x1,
                  tft_region_coords[BATTERY_REGION].y1, ICON_BATTERY_FULL, ST7735_GREEN, 16,
                  ICON_HEIGHT);
        break;
    default:
        break;
    }
}