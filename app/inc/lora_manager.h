#ifndef LORA_MANAGER_H
#define LORA_MANAGER_H

#include "esp_log.h"
#include "lora_api.h"
#include "shared_data.h"
#include <stdbool.h>

esp_err_t lora_set_config(lora_config_t* dev);
esp_err_t lora_send_all_data(lora_config_t* dev, SoilData_t* soilData, GNSSData_t* gnssData,
                             bool category);
#endif // LORA_MANAGER_H