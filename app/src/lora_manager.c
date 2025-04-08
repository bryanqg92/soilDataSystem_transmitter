#include "lora_manager.h"
#include "config.h"
#include "esp_log.h"
#include "lora_api.h"
#include "stdio.h"
#include "string.h"

esp_err_t lora_set_config(lora_config_t* dev)
{
    LoRaDebugPrint(true);
    if (LoRaBegin(&dev->spi_handle, LORA_FREQ, LORA_TX_POWER, LORA_TXCO_VOLTAGE, false) != 0)
    {
        ESP_LOGE("LORA", "Does not recognize the module");
        return ESP_FAIL;
    }

    LoRaConfig(LORA_SF_10, SX126X_LORA_BW_125_0, SX126X_LORA_CR_4_6, LORA_PREAMBLE_LENGTH,
               LORA_PAYLOAD_LENGTH, SX126X_LORA_CRC_OFF, SX126X_LORA_IQ_STANDARD);

    return ESP_OK;
}

esp_err_t lora_send_all_data(lora_config_t* dev, SoilData_t* soilData, GNSSData_t* gnssData,
                             bool category)
{
    if (soilData == NULL || gnssData == NULL)
    {
        ESP_LOGE("LORA", "Invalid data pointers");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t data_buffer[40]; // Tamaño exacto
    int offset = 0;

    // 📌 SoilData
    memcpy(&data_buffer[offset], &soilData->nitrogen, 2);
    offset += 2;
    memcpy(&data_buffer[offset], &soilData->phosphorus, 2);
    offset += 2;
    memcpy(&data_buffer[offset], &soilData->potassium, 2);
    offset += 2;
    memcpy(&data_buffer[offset], &soilData->temperature, 4);
    offset += 4;
    memcpy(&data_buffer[offset], &soilData->moisture, 4);
    offset += 4;
    memcpy(&data_buffer[offset], &soilData->pH, 4);
    offset += 4;
    memcpy(&data_buffer[offset], &soilData->conductivity, 2);
    offset += 2;

    // 📌 GNSSData
    memcpy(&data_buffer[offset], &gnssData->altitude, 4);
    offset += 4;
    memcpy(&data_buffer[offset], &gnssData->latitude, 4);
    offset += 4;
    memcpy(&data_buffer[offset], &gnssData->longitude, 4);
    offset += 4;
    memcpy(&data_buffer[offset], &gnssData->year, 2);
    offset += 2;
    memcpy(&data_buffer[offset], &gnssData->month, 1);
    offset += 1;
    memcpy(&data_buffer[offset], &gnssData->day, 1);
    offset += 1;
    memcpy(&data_buffer[offset], &gnssData->hour, 1);
    offset += 1;
    memcpy(&data_buffer[offset], &gnssData->minute, 1);
    offset += 1;

    // 📌 Categoría (1 byte)
    data_buffer[offset++] = category ? 1 : 0;

    // 📌 Log the data buffer
    char log_buffer[3 * sizeof(data_buffer) + 1] = {0}; // Buffer for hex representation
    for (int i = 0; i < offset; i++)
    {
        sprintf(&log_buffer[i * 3], "%02X ", data_buffer[i]);
    }
    ESP_LOGI("LORA", "Data buffer: %s", log_buffer);

    // 📌 Enviar los datos
    if (!LoRaSend(data_buffer, offset, SX126x_TXMODE_SYNC))
    {
        ESP_LOGE("LORA", "Error al enviar datos por LoRa");
        return ESP_FAIL;
    }
    ESP_LOGI("LORA", "LoRa packet sent successfully (%d bytes)", offset);
    return ESP_OK;
}
