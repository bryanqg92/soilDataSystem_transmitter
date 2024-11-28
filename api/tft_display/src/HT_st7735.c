#include "HT_st7735.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "logger.h"
#include <string.h>

static const char* TFT_STT35 = "ST7735";

#define DELAY 0x80

const uint8_t init_cmds1[] = {15,
                              ST7735_SWRESET,
                              DELAY,
                              150,
                              ST7735_SLPOUT,
                              DELAY,
                              255,
                              ST7735_FRMCTR1,
                              3,
                              0x01,
                              0x2C,
                              0x2D,
                              ST7735_FRMCTR2,
                              3,
                              0x01,
                              0x2C,
                              0x2D,
                              ST7735_FRMCTR3,
                              6,
                              0x01,
                              0x2C,
                              0x2D,
                              0x01,
                              0x2C,
                              0x2D,
                              ST7735_INVCTR,
                              1,
                              0x07,
                              ST7735_PWCTR1,
                              3,
                              0xA2,
                              0x02,
                              0x84,
                              ST7735_PWCTR2,
                              1,
                              0xC5,
                              ST7735_PWCTR3,
                              2,
                              0x0A,
                              0x00,
                              ST7735_PWCTR4,
                              2,
                              0x8A,
                              0x2A,
                              ST7735_PWCTR5,
                              2,
                              0x8A,
                              0xEE,
                              ST7735_VMCTR1,
                              1,
                              0x0E,
                              ST7735_INVOFF,
                              0,
                              ST7735_MADCTL,
                              1,
                              ST7735_ROTATION,
                              ST7735_COLMOD,
                              1,
                              0x05};

#ifdef ST7735_IS_160X80
const uint8_t init_cmds2[] = {3, ST7735_CASET, 4,    0x00, 0x00, 0x00,         0x4F, ST7735_RASET,
                              4, 0x00,         0x00, 0x00, 0x9F, ST7735_INVON, 0};
#endif

const uint8_t init_cmds3[] = {
    4,    ST7735_GMCTRP1, 16,    0x02, 0x1c,          0x07,  0x12, 0x37, 0x32,
    0x29, 0x2d,           0x29,  0x25, 0x2B,          0x39,  0x00, 0x01, 0x03,
    0x10, ST7735_GMCTRN1, 16,    0x03, 0x1d,          0x07,  0x06, 0x2E, 0x2C,
    0x29, 0x2D,           0x2E,  0x2E, 0x37,          0x3F,  0x00, 0x00, 0x02,
    0x10, ST7735_NORON,   DELAY, 10,   ST7735_DISPON, DELAY, 100};

/**
 * @brief Selecciona el dispositivo ST7735 para la comunicación.
 *
 * Esta función establece el nivel del pin CS (Chip Select) en bajo (0),
 * lo que indica que el dispositivo ST7735 está seleccionado y listo para
 * recibir comandos o datos.
 *
 * @param config Puntero a la estructura ST7735_Config que contiene la configuración
 *               del pin CS.
 */

static void st7735_select(ST7735_Config* config) { gpio_set_level(config->cs_pin, 0); }

/**
 * @brief Deselecciona el dispositivo ST7735.
 *
 * Esta función establece el nivel del pin CS (Chip Select) en alto (1),
 * lo que indica que el dispositivo ST7735 está deseleccionado y no recibirá
 * más comandos o datos.
 *
 * @param config Puntero a la estructura ST7735_Config que contiene la configuración
 *               del pin CS.
 */
static void st7735_unselect(ST7735_Config* config) { gpio_set_level(config->cs_pin, 1); }

static void st7735_reset(ST7735_Config* config)
{
    gpio_set_level(config->rst_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(config->rst_pin, 1);
}

static void st7735_write_cmd(ST7735_Config* config, uint8_t cmd)
{
    gpio_set_level((gpio_num_t)config->dc_pin, 0);
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &cmd,
    };
    ESP_ERROR_CHECK(spi_device_transmit(config->spi_dev, &t));
}
/**
 * @brief Escribe datos en la pantalla ST7735.
 *
 * Esta función envía un búfer de datos a la pantalla ST7735 usando SPI.
 * Establece el pin de datos/comando para indicar la transmisión de datos y luego
 * realiza la transacción SPI.
 *
 * @param config Puntero a la estructura de configuración del ST7735.
 *               No debe ser NULL.
 * @param buff Puntero al búfer que contiene los datos a enviar.
 *             No debe ser NULL.
 * @param buff_size Tamaño del búfer en bytes.
 *
 * @note Esta función registra un error y retorna si los parámetros son inválidos
 *       o si la transmisión SPI falla.
 */

static void st7735_write_data(ST7735_Config* config, uint8_t* buff, size_t buff_size)
{
    if (!config || !buff)
    {
        ESP_LOGE(TFT_STT35, "Invalid parameters");
        return;
    }

    gpio_set_level(config->dc_pin, 1);

    spi_transaction_t t = {
        .length = buff_size * 8,
        .tx_buffer = buff,
    };

    esp_err_t ret = spi_device_transmit(config->spi_dev, &t);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TFT_STT35, "SPI transmission failed: %s", esp_err_to_name(ret));
        return;
    }
}

/**
 * @brief Ejecuta una lista de comandos para el ST7735.
 *
 * Esta función toma una lista de comandos y argumentos, y los envía al
 * controlador de la pantalla ST7735. Cada comando puede tener un número
 * variable de argumentos y puede requerir un retraso después de su ejecución.
 *
 * @param config Puntero a la estructura de configuración del ST7735.
 *               No debe ser NULL.
 * @param addr Puntero a la lista de comandos y argumentos.
 *             No debe ser NULL.
 *
 * La lista de comandos comienza con un byte que indica el número de comandos.
 * Cada comando es seguido por un byte que indica el número de argumentos.
 * Si el bit más significativo de este byte está establecido (DELAY), se espera
 * un retraso después de enviar los argumentos. El valor del retraso se especifica
 * en el siguiente byte.
 */
static void st7735_execute_cmd_list(ST7735_Config* config, const uint8_t* addr)
{
    uint8_t numCommands = *addr++;
    uint8_t numArgs;
    uint16_t ms;

    while (numCommands--)
    {
        uint8_t cmd = *addr++;
        st7735_write_cmd(config, cmd);

        numArgs = *addr++;
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;

        if (numArgs)
        {
            st7735_write_data(config, (uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if (ms)
        {
            ms = *addr++;
            if (ms == 255)
                ms = 500;
            vTaskDelay(pdMS_TO_TICKS(ms));
        }
    }
}
/**
 * @brief Configura la ventana de dirección del controlador ST7735.
 *
 * Esta función establece la ventana de dirección en el controlador de pantalla ST7735,
 * especificando las coordenadas de inicio y fin en los ejes X e Y. Esto es necesario
 * para definir el área de la pantalla donde se escribirán los datos de imagen.
 *
 * @param config Puntero a la estructura de configuración del ST7735.
 * @param x0 Coordenada X inicial.
 * @param y0 Coordenada Y inicial.
 * @param x1 Coordenada X final.
 * @param y1 Coordenada Y final.
 */

static void st7735_set_address_window(ST7735_Config* config, uint8_t x0, uint8_t y0, uint8_t x1,
                                      uint8_t y1)
{
    st7735_write_cmd(config, ST7735_CASET);
    uint8_t data[] = {0x00, x0 + config->x_start, 0x00, x1 + config->x_start};
    st7735_write_data(config, data, sizeof(data));

    st7735_write_cmd(config, ST7735_RASET);
    data[1] = y0 + config->y_start;
    data[3] = y1 + config->y_start;
    st7735_write_data(config, data, sizeof(data));

    st7735_write_cmd(config, ST7735_RAMWR);
}

/**
 * @brief Inicializa la pantalla ST7735.
 *
 * Esta función configura e inicializa la pantalla ST7735. Realiza los siguientes pasos:
 * 1. Enciende el pin LED_K para iluminar la pantalla.
 * 2. Selecciona el dispositivo ST7735 para la comunicación.
 * 3. Realiza un reinicio de hardware del dispositivo.
 * 4. Ejecuta una serie de comandos de inicialización para configurar la pantalla.
 * 5. Deselecciona el dispositivo ST7735.
 *
 * @param config Puntero a la estructura de configuración del ST7735.
 *               No debe ser NULL.
 */
void st7735_init(ST7735_Config* config)
{
    ESP_LOGI(TFT_STT35, "Inicializando pantalla...");

    // Enciende el pin LED_K para iluminar la pantalla
    gpio_set_level(config->led_k_pin, 1);

    // Selecciona el dispositivo ST7735 para la comunicación
    st7735_select(config);

    // Realiza un reinicio de hardware del dispositivo
    st7735_reset(config);

    // Ejecuta la primera serie de comandos de inicialización
    ESP_LOGI(TFT_STT35, "Ejecutando init_cmds1...");
    st7735_execute_cmd_list(config, init_cmds1);

    // Ejecuta la segunda serie de comandos de inicialización
    ESP_LOGI(TFT_STT35, "Ejecutando init_cmds2...");
    st7735_execute_cmd_list(config, init_cmds2);

    // Ejecuta la tercera serie de comandos de inicialización
    ESP_LOGI(TFT_STT35, "Ejecutando init_cmds3...");
    st7735_execute_cmd_list(config, init_cmds3);

    // Deselecciona el dispositivo ST7735
    st7735_unselect(config);

    ESP_LOGI(TFT_STT35, "Pantalla inicializada");
}

/**
 * @brief Dibuja un píxel en la pantalla ST7735.
 *
 * Esta función dibuja un píxel en las coordenadas especificadas (x, y) con el color dado.
 * Primero, verifica si las coordenadas están dentro de los límites de la pantalla.
 * Luego, selecciona la pantalla, establece la ventana de dirección para el píxel,
 * escribe el color del píxel y finalmente deselecciona la pantalla.
 *
 * @param config Puntero a la configuración del ST7735.
 * @param x Coordenada x del píxel.
 * @param y Coordenada y del píxel.
 * @param color Color del píxel en formato RGB565.
 */
void st7735_draw_pixel(ST7735_Config* config, uint16_t x, uint16_t y, uint16_t color)
{
    if ((x >= config->width) || (y >= config->height))
        return;

    st7735_select(config);
    st7735_set_address_window(config, x, y, x + 1, y + 1);
    uint8_t data[] = {color >> 8, color & 0xFF};
    st7735_write_data(config, data, sizeof(data));
    st7735_unselect(config);
}

/**
 * @brief Escribe un carácter en la pantalla TFT ST7735.
 *
 * Esta función escribe un carácter específico en la pantalla TFT ST7735 en las coordenadas (x, y)
 * utilizando una fuente y colores especificados. La función configura una ventana de dirección
 * en la pantalla y luego dibuja el carácter píxel por píxel.
 *
 * @param config Puntero a la configuración del ST7735.
 * @param x Coordenada X donde se dibujará el carácter.
 * @param y Coordenada Y donde se dibujará el carácter.
 * @param ch Carácter a dibujar.
 * @param font Definición de la fuente que se utilizará para dibujar el carácter.
 * @param color Color del carácter.
 * @param bgcolor Color de fondo del carácter.
 */
void st7735_write_char(ST7735_Config* config, uint16_t x, uint16_t y, char ch, FontDef font,
                       uint16_t color, uint16_t bgcolor)
{
    uint32_t i, b, j;
    st7735_set_address_window(config, x, y, x + font.width - 1, y + font.height - 1);

    for (i = 0; i < font.height; i++)
    {
        b = font.data[(ch - 32) * font.height + i];
        for (j = 0; j < font.width; j++)
        {
            if ((b << j) & 0x8000)
            {
                uint8_t data[] = {color >> 8, color & 0xFF};
                st7735_write_data(config, data, sizeof(data));
            }
            else
            {
                uint8_t data[] = {bgcolor >> 8, bgcolor & 0xFF};
                st7735_write_data(config, data, sizeof(data));
            }
        }
    }
}

/**
 * @brief Escribe una cadena de texto en la pantalla ST7735.
 *
 * Esta función escribe una cadena de texto en la pantalla ST7735 comenzando en las coordenadas (x,
 * y) especificadas. Si la cadena de texto excede el ancho de la pantalla, la función
 * automáticamente mueve la posición de escritura a la siguiente línea. Si la cadena de texto excede
 * el alto de la pantalla, la función deja de escribir.
 *
 * @param config Puntero a la configuración del ST7735.
 * @param x Coordenada x inicial para escribir la cadena.
 * @param y Coordenada y inicial para escribir la cadena.
 * @param str Puntero a la cadena de texto a escribir.
 * @param font Estructura que define la fuente a utilizar.
 * @param color Color del texto.
 * @param bgcolor Color de fondo del texto.
 */
void st7735_write_string(ST7735_Config* config, uint16_t x, uint16_t y, const char* str,
                         FontDef font, uint16_t color, uint16_t bgcolor)
{

    while (*str)
    {

        if (x + font.width >= config->width)
        {
            x = 0;
            y += font.height;
            if (y + font.height >= config->height)
            {
                break;
            }
            if (*str == ' ')
            {
                str++;
                continue;
            }
        }
        st7735_write_char(config, x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }
}

/**
 * @brief Rellena un rectángulo en la pantalla ST7735 con un color específico.
 *
 * Esta función dibuja un rectángulo de ancho y alto especificados en las coordenadas (x, y)
 * de la pantalla ST7735, utilizando el color proporcionado. La función se asegura de que
 * el rectángulo no exceda los límites de la pantalla mediante recortes (clipping).
 *
 * @param config Puntero a la configuración del ST7735.
 * @param x Coordenada X del punto superior izquierdo del rectángulo.
 * @param y Coordenada Y del punto superior izquierdo del rectángulo.
 * @param w Ancho del rectángulo.
 * @param h Alto del rectángulo.
 * @param color Color con el que se rellenará el rectángulo.
 */
void st7735_fill_rectangle(ST7735_Config* config, uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                           uint16_t color)
{
    // clipping
    if ((x >= config->width) || (y >= config->height))
        return;
    if ((x + w - 1) >= config->width)
        w = config->width - x;
    if ((y + h - 1) >= config->height)
        h = config->height - y;

    st7735_select(config);
    st7735_set_address_window(config, x, y, x + w - 1, y + h - 1);

    uint8_t data[] = {color >> 8, color & 0xFF};
    gpio_set_level(config->dc_pin, 1);
    for (uint16_t i = 0; i < h; i++)
    {
        for (uint16_t j = 0; j < w; j++)
        {
            st7735_write_data(config, data, sizeof(data));
        }
    }

    st7735_unselect(config);
}

/**
 * @brief Llena toda la pantalla con un color específico.
 *
 * Esta función utiliza la función st7735_fill_rectangle para llenar
 * toda la pantalla con el color especificado.
 *
 * @param config Puntero a la configuración del ST7735.
 * @param color Color con el que se llenará la pantalla.
 */
void st7735_fill_screen(ST7735_Config* config, uint16_t color)
{
    st7735_fill_rectangle(config, 0, 0, config->width, config->height, color);
}

/**
 * @brief Dibuja una imagen en la pantalla ST7735.
 *
 * Esta función dibuja una imagen en la pantalla ST7735 en las coordenadas
 * especificadas (x, y) con el ancho (w) y alto (h) dados. Los datos de la
 * imagen se proporcionan como un array de valores de color de 16 bits.
 *
 * @param config Puntero a la estructura de configuración del ST7735.
 * @param x La coordenada x donde se dibujará la imagen.
 * @param y La coordenada y donde se dibujará la imagen.
 * @param w El ancho de la imagen.
 * @param h El alto de la imagen.
 * @param data Puntero al array de datos de la imagen (valores de color de 16 bits).
 *
 * @note La función realiza comprobaciones de límites para asegurar que la imagen
 *       no exceda las dimensiones de la pantalla. Si la imagen excede los límites,
 *       la función retornará sin dibujar.
 */
void st7735_draw_image(ST7735_Config* config, uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                       const uint16_t* data)
{
    if ((x >= config->width) || (y >= config->height))
        return;
    if ((x + w - 1) >= config->width)
        return;
    if ((y + h - 1) >= config->height)
        return;

    st7735_select(config);
    st7735_set_address_window(config, x, y, x + w - 1, y + h - 1);
    st7735_write_data(config, (uint8_t*)data, sizeof(uint16_t) * w * h);
    st7735_unselect(config);
}

/**
 * @brief Invierte los colores de la pantalla ST7735.
 *
 * Esta función invierte los colores de la pantalla ST7735 según el valor del parámetro `invert`.
 *
 * @param config Puntero a la configuración del ST7735.
 * @param invert Si es `true`, invierte los colores de la pantalla. Si es `false`, restaura los
 * colores originales.
 */
void st7735_invert_colors(ST7735_Config* config, bool invert)
{
    st7735_select(config);
    st7735_write_cmd(config, invert ? ST7735_INVON : ST7735_INVOFF);
    st7735_unselect(config);
}

/**
 * @brief Configura la gamma del display ST7735.
 *
 * Esta función establece la configuración de gamma para el display ST7735.
 * La gamma es una característica que afecta la representación de los colores
 * en la pantalla, ajustando la relación entre la entrada de señal y la salida
 * de brillo.
 *
 * @param config Puntero a la estructura de configuración del ST7735.
 * @param gamma Valor de la gamma a configurar. Este valor se convierte a un
 *              uint8_t antes de enviarlo al display.
 */
void st7735_set_gamma(ST7735_Config* config, GammaDef gamma)
{
    uint8_t data = (uint8_t)gamma;
    st7735_write_cmd(config, ST7735_GAMSET);
    st7735_write_data(config, &data, 1);
}
