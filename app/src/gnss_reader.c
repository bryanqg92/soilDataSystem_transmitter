#include "api_uart.h"
#include "app.h"
#include "logger.h"
#include <string.h>

const char* GNSS_READER = "[GNSS_READER]";
void Task_GNSSData(void* pvParameters)
{
    GNSSElements_t gnssContext = *(GNSSElements_t*)pvParameters;
    uint8_t gnss_buffer[GNSS_MAX_MESSAGE_SIZE];
    esp_err_t err;

    while (1)
    {
        memset(&gnssContext.gnssData, 0, sizeof(GNSSData_t));
        err = uart_read_data(&gnssContext.gnss_port, gnss_buffer, GNSS_MAX_MESSAGE_SIZE - 1,
                             GNSS_TIMEOUT_MS);
        // ESP_LOGI(GNSS_READER, "%s", (char*)gnss_buffer);
        if (err == ESP_OK)
        {
            if (parse_gnss_buffer(gnss_buffer, strlen((char*)gnss_buffer), &gnssContext.gnssData))
            {

                // enviar a la cola
                if (xQueueSend(xQueueGNSSData, &gnssContext.gnssData, 0) != pdPASS)
                {
                    ESP_LOGE(GNSS_READER, "Failed to send GNSS data to queue");
                }
                ESP_LOGI(GNSS_READER, "Lat: %.6f, Lon: %.6f, Alt: %.2f",
                         gnssContext.gnssData.latitude, gnssContext.gnssData.longitude,
                         gnssContext.gnssData.altitude);
                ESP_LOGI(GNSS_READER, "Date: %02d/%02d/%d Time: %02d:%02d, Sats: %d, Fix: %d",
                         gnssContext.gnssData.day, gnssContext.gnssData.month,
                         gnssContext.gnssData.year, gnssContext.gnssData.hour,
                         gnssContext.gnssData.minute, gnssContext.gnssData.satellites_used,
                         gnssContext.gnssData.fix_status);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
