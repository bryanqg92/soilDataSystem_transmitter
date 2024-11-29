#include "soil_sensor_reader.h"
#include "api_uart.h"
#include "app.h"
#include "esp_err.h"
#include "logger.h"
#include "shared_data.h"
#include <string.h>

static const char* TAG = "[SOIL_SENSOR_READER]";

static const uint8_t INIT_REQUEST[] = {0xFF, 0x03, 0x07, 0xD0, 0x00, 0x01, 0x91, 0x59};
static const uint8_t NPK_REQUEST[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08};

static bool send_request_and_receive_response(uart_t* uart, const uint8_t* request,
                                              size_t request_size, uint8_t* response_buffer,
                                              size_t response_size);
static bool validate_response(uint8_t* response, size_t response_size);
static void parse_soil_data(const uint8_t* response, SoilData_t* sensor_data);

/**
 * @brief Inicializa el sensor NPK.
 *
 * Esta función envía una solicitud de inicialización al sensor NPK a través de UART y 
 * espera una respuesta. Verifica si la respuesta es válida y si la inicialización fue exitosa.
 *
 * @param uart Puntero a la estructura de UART utilizada para la comunicación con el sensor.
 * @return true si la inicialización fue exitosa, false en caso contrario.
 *
 * La función realiza los siguientes pasos:
 * 1. Envía una solicitud de inicialización al sensor NPK.
 * 2. Recibe la respuesta del sensor.
 * 3. Verifica si la respuesta es válida.
 * 4. Si la respuesta no es válida, registra un mensaje de error y retorna false.
 * 5. Si la respuesta es válida, retorna true.
 */
bool NPKInit(uart_t* uart)
{
    uint8_t response_buffer[NPK_REQUEST_SIZE];

    ESP_LOGI(TAG, "Initializing NPK sensor");

    if (!send_request_and_receive_response(uart, INIT_REQUEST, sizeof(INIT_REQUEST),
                                           response_buffer, INIT_RESPONSE_SIZE))
    {
        ESP_LOGE(TAG, "DATA INITIALIZATION FAILED");
        return false;
    }

    if (response_buffer[0] != 0x01)
    {
        memmove(response_buffer, response_buffer + 1, INIT_RESPONSE_SIZE - 1);
    }

    if (!validate_response(response_buffer, INIT_RESPONSE_SIZE - 1))
    {
        ESP_LOGI(TAG, "%02x %02x %02x %02x %02x %02x %02x %02x", response_buffer[0],
                 response_buffer[1], response_buffer[2], response_buffer[3], response_buffer[4],
                 response_buffer[5], response_buffer[6], response_buffer[7]);
        ESP_LOGE(TAG, "CRC INITIALIZATION FAILED");
        return false;
    }

    return true;
}

/**
 * @brief Envía una solicitud a través de UART y recibe una respuesta.
 *
 * Esta función utiliza el manejador de UART para enviar una solicitud y luego
 * espera una respuesta. Si ocurre algún error durante el envío o la recepción,
 * se registra un mensaje de error y la función retorna false.
 *
 * @param uart Puntero al manejador de UART.
 * @param request Puntero al buffer que contiene la solicitud a enviar.
 * @param request_size Tamaño del buffer de solicitud.
 * @param response_buffer Puntero al buffer donde se almacenará la respuesta recibida.
 * @param response_size Tamaño del buffer de respuesta.
 * @return true si la solicitud y la respuesta se manejaron correctamente, false en caso de error.
 */
static bool send_request_and_receive_response(uart_t* uart, const uint8_t* request,
                                              size_t request_size, uint8_t* response_buffer,
                                              size_t response_size)
{
    esp_err_t npk_u;

    // Use the UART handler to send data
    npk_u = uart_write_data(uart, request, request_size);
    if (ESP_OK != npk_u)
    {
        ESP_LOGE(TAG, "Error sending request: %s", esp_err_to_name(npk_u));
        return false;
    }

    // Use the UART handler to read data
    npk_u = uart_read_data(uart, response_buffer, response_size, 1000 / portTICK_PERIOD_MS);

#if LOG_APP

    char log_buffer[128];
    int offset = 0;
    for (int i = 0; i < response_size; i++)
    {
        offset +=
            snprintf(log_buffer + offset, sizeof(log_buffer) - offset, "%02X ", response_buffer[i]);
    }

#endif
    ESP_LOGI(TAG, "Response bytes: %s", log_buffer);
    if (ESP_OK != npk_u)
    {
        ESP_LOGE(TAG, "Error receiving response: %s", esp_err_to_name(npk_u));

        return false;
    }

    return true;
}

/**
 * @brief Valida la respuesta recibida verificando su CRC.
 *
 * Esta función toma una respuesta y su tamaño, y verifica si el CRC de la respuesta es correcto.
 * Si el CRC es válido, la función retorna true, de lo contrario, retorna false.
 *
 * @param response Puntero al buffer que contiene la respuesta a validar.
 * @param response_size Tamaño del buffer de respuesta.
 * @return true si el CRC de la respuesta es válido, false en caso contrario.
 */
static bool validate_response(uint8_t* response, size_t response_size)
{
    if (!verifyCRC(response, response_size))
    {
        return false;
    }
    return true;
}

/**
 * @brief Parsea los datos del sensor de suelo a partir de la respuesta recibida.
 *
 * Esta función toma una respuesta en formato de arreglo de bytes y extrae los datos
 * del sensor de suelo, asignándolos a una estructura SoilData_t.
 *
 * @param response Puntero al arreglo de bytes que contiene la respuesta del sensor.
 * @param sensor_data Puntero a la estructura SoilData_t donde se almacenarán los datos parseados.
 *
 * Los datos extraídos incluyen:
 * - Humedad del suelo (moisture), en porcentaje.
 * - Temperatura del suelo (temperature), en grados Celsius.
 * - Conductividad del suelo (conductivity), en µS/cm.
 * - pH del suelo (pH).
 * - Nitrógeno en el suelo (nitrogen), en mg/kg.
 * - Fósforo en el suelo (phosphorus), en mg/kg.
 * - Potasio en el suelo (potassium), en mg/kg.
 */
static void parse_soil_data(const uint8_t* response, SoilData_t* sensor_data)
{
    sensor_data->moisture = ((response[3] << 8) | response[4]) / 10.0f;
    sensor_data->temperature = ((response[5] << 8) | response[6]) / 10.0f;
    sensor_data->conductivity = (response[7] << 8) | response[8];
    sensor_data->pH = ((response[9] << 8) | response[10]) / 10.0f;
    sensor_data->nitrogen = (response[11] << 8) | response[12];
    sensor_data->phosphorus = (response[13] << 8) | response[14];
    sensor_data->potassium = (response[15] << 8) | response[16];
}

/**
 * @brief Tarea para procesar datos del sensor de suelo.
 *
 * Esta función se ejecuta en un bucle infinito y realiza las siguientes acciones:
 * 1. Envía una solicitud a través del puerto UART del sensor y espera una respuesta.
 * 2. Verifica si la solicitud y la respuesta fueron exitosas.
 * 3. Si la solicitud y la respuesta fueron exitosas, actualiza el estado del sensor a 1.
 * 4. Analiza los datos recibidos y los almacena en la estructura de datos del sensor.
 * 5. Envía los datos procesados a una cola para su posterior uso.
 * 6. Registra los datos del sensor en el log.
 * 7. Espera un tiempo determinado antes de repetir el proceso.
 *
 * @param soilData Puntero a la estructura de datos del sensor de suelo (SoilData_t).
 */


void Task_processData(void* soilData)
{
    SoilData_t* sensor_data = (SoilData_t*)soilData;
    uint8_t response_buffer[NPK_RESPONSE_SIZE];

    while (1)
    {
        // Use the UART handler from sensor data structure
        if (!send_request_and_receive_response(&sensor_data->npk_port, NPK_REQUEST,
                                               sizeof(NPK_REQUEST), response_buffer,
                                               NPK_RESPONSE_SIZE))
        {
            ESP_LOGE(TAG, "Error sending request or receiving response");
            sensor_data->status = 0;
        }

        // if (!validate_response(response_buffer, NPK_RESPONSE_SIZE))
        //{
        //     ESP_LOGE(TAG, "Invalid response received");
        //     sensor_data->status = 0;
        // }
        else
        {
            sensor_data->status = 1;
        }

        // send data to queue
        parse_soil_data(response_buffer, sensor_data);
        ESP_LOGI(TAG,
                 "Soil Data - Moisture: %.1f%%, Temperature: %.1f°C, Conductivity: %d, pH: %.1f, "
                 "N: %d, P: %d, K: %d",
                 sensor_data->moisture, sensor_data->temperature, sensor_data->conductivity,
                 sensor_data->pH, sensor_data->nitrogen, sensor_data->phosphorus,
                 sensor_data->potassium);

        if (pdPASS != xQueueSend(xQueueSoilData, sensor_data, portMAX_DELAY))
        {
            ESP_LOGE(TAG, "Failed to send data to queue");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
