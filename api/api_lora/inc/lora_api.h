
#ifndef __API_lora_H__
#define __API_lora_H__

#include "driver/spi_master.h"
#include "esp_system.h"
#include "lora_spi_handler.h"

// Comandos operacionales del SX1262
#define LORA_CMD_SET_SLEEP 0x84
#define LORA_CMD_SET_STANDBY 0x80
#define LORA_CMD_SET_TX 0x83
#define LORA_CMD_SET_RX 0x82
#define LORA_CMD_STOP_TIMER_ON_PREAMBLE 0x9F
#define LORA_CMD_SET_RX_DUTY_CYCLE 0x94
#define LORA_CMD_SET_CAD 0xC5
#define LORA_CMD_SET_TX_CONTINUOUS_WAVE 0xD1
#define LORA_CMD_SET_TX_INFINITE_PREAMBLE 0xD2
#define LORA_CMD_SET_REGULATOR_MODE 0x96
#define LORA_CMD_CALIBRATE 0x89
#define LORA_CMD_CALIBRATE_IMAGE 0x98
#define LORA_CMD_SET_PA_CONFIG 0x95
#define LORA_CMD_SET_RX_TX_FALLBACK_MODE 0x93

// Comandos de registro
#define LORA_CMD_WRITE_REGISTER 0x0D
#define LORA_CMD_READ_REGISTER 0x1D
#define LORA_CMD_WRITE_BUFFER 0x0E
#define LORA_CMD_READ_BUFFER 0x1E

// Comandos de estado
#define LORA_CMD_GET_STATUS 0xC0
#define LORA_CMD_GET_RSSI_INST 0x15
#define LORA_CMD_GET_RX_BUFFER_STATUS 0x13
#define LORA_CMD_GET_PACKET_STATUS 0x14
#define LORA_CMD_GET_DEVICE_ERRORS 0x17
#define LORA_CMD_CLEAR_DEVICE_ERRORS 0x07
#define LORA_CMD_GET_IRQ_STATUS 0x12
#define LORA_CMD_CLEAR_IRQ_STATUS 0x02

// Funciones de inicialización y control
int lora_init(lora_config_t* config);
void lora_reset(lora_config_t* config);
void lora_set_standby(lora_config_t* config);
void lora_sleep(lora_config_t* config);

// Funciones de configuración RF
void lora_set_frequency(lora_config_t* config, long frequency);
void lora_set_tx_power(lora_config_t* config, int level);
void lora_set_spreading_factor(lora_config_t* config, int sf);
void lora_set_bandwidth(lora_config_t* config, long sbw);
void lora_set_coding_rate(lora_config_t* config, int denominator);
void lora_set_preamble_length(lora_config_t* config, long length);
void lora_set_sync_word(lora_config_t* config, int sw);

// Funciones de transmisión/recepción
void lora_send_packet(lora_config_t* config, uint8_t* buf, int size);
int lora_receive_packet(lora_config_t* config, uint8_t* buf, int size);
int lora_received(lora_config_t* config);
int lora_packet_rssi(lora_config_t* config);
float lora_packet_snr(lora_config_t* config);

#endif
