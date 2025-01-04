#include "lora_api.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static uint8_t tx_buf[256];
static uint8_t rx_buf[256];
static long __frequency;

static void lora_write_command(lora_config_t* config, uint8_t cmd, uint8_t* data, uint8_t size)
{
    while (gpio_get_level(config->lora_busy_pin))
    {
        vTaskDelay(1);
    }

    tx_buf[0] = cmd;
    if (data && size > 0)
    {
        memcpy(&tx_buf[1], data, size);
    }

    spi_transaction_t t = {
        .flags = 0, .length = 8 * (size + 1), .tx_buffer = tx_buf, .rx_buffer = rx_buf};

    gpio_set_level(config->lora_nss_pin, 0);
    spi_device_transmit(config->spi_handle, &t);
    gpio_set_level(config->lora_nss_pin, 1);
}

static void lora_read_command(lora_config_t* config, uint8_t cmd, uint8_t* data, uint8_t size)
{
    while (gpio_get_level(config->lora_busy_pin))
    {
        vTaskDelay(1);
    }

    tx_buf[0] = cmd;
    memset(&tx_buf[1], 0, size);

    spi_transaction_t t = {
        .flags = 0, .length = 8 * (size + 1), .tx_buffer = tx_buf, .rx_buffer = rx_buf};

    gpio_set_level(config->lora_nss_pin, 0);
    spi_device_transmit(config->spi_handle, &t);
    gpio_set_level(config->lora_nss_pin, 1);

    if (data && size > 0)
    {
        memcpy(data, &rx_buf[1], size);
    }
}

void lora_reset(lora_config_t* config)
{
    gpio_set_level(config->lora_rst_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(config->lora_rst_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(20));
}

int lora_init(lora_config_t* config)
{
    // Reset del dispositivo
    lora_reset(config);

    // Esperar que el dispositivo esté listo
    vTaskDelay(pdMS_TO_TICKS(10));

    // Configurar en modo STANDBY_RC
    uint8_t standby_cfg = 0x00; // STDBY_RC
    lora_write_command(config, LORA_CMD_SET_STANDBY, &standby_cfg, 1);

    // Configurar regulador DC-DC
    uint8_t reg_cfg = 0x01; // DC-DC
    lora_write_command(config, LORA_CMD_SET_REGULATOR_MODE, &reg_cfg, 1);

    // Calibración de todos los bloques
    uint8_t cal_cfg = 0x7F;
    lora_write_command(config, LORA_CMD_CALIBRATE, &cal_cfg, 1);
    vTaskDelay(pdMS_TO_TICKS(10));

    // Configurar PA
    uint8_t pa_cfg[4] = {
        0x04, // PA_DUTY_CYCLE
        0x07, // HP_MAX
        0x00, // Device specific
        0x01  // PA_LUT
    };
    lora_write_command(config, LORA_CMD_SET_PA_CONFIG, pa_cfg, 4);

    // DIO2 como RF switch control
    uint8_t dio2_cfg = 0x01;
    lora_write_command(config, 0x9D, &dio2_cfg, 1);

    // Calibrar RX
    uint8_t img_cfg = 0x7F;
    lora_write_command(config, LORA_CMD_CALIBRATE_IMAGE, &img_cfg, 1);
    vTaskDelay(pdMS_TO_TICKS(10));

    return 1;
}

void lora_set_tx_power(lora_config_t* config, int level)
{
    if (level < -9)
        level = -9;
    if (level > 22)
        level = 22;

    uint8_t pa_cfg[4] = {
        0x04, // PA_DUTY_CYCLE
        0x07, // HP_MAX
        0x00, // Device specific
        0x01  // PA_LUT
    };
    lora_write_command(config, LORA_CMD_SET_PA_CONFIG, pa_cfg, 4);

    uint8_t tx_params[2] = {
        level + 9, // Power
        0x04       // Ramping time
    };
    lora_write_command(config, 0x8E, tx_params, 2);
}

void lora_set_frequency(lora_config_t* config, long frequency)
{
    __frequency = frequency;
    uint32_t freq = (uint32_t)((double)frequency / 32768 * (1 << 25));

    uint8_t freq_cfg[4];
    freq_cfg[0] = (freq >> 24) & 0xFF;
    freq_cfg[1] = (freq >> 16) & 0xFF;
    freq_cfg[2] = (freq >> 8) & 0xFF;
    freq_cfg[3] = freq & 0xFF;

    lora_write_command(config, 0x86, freq_cfg, 4);
}

void lora_set_spreading_factor(lora_config_t* config, int sf)
{
    if (sf < 5)
        sf = 5;
    if (sf > 12)
        sf = 12;

    uint8_t mod_params[8] = {
        0x00, // SF
        0x00, // BW
        0x01, // CR
        0x00  // LDRO
    };
    mod_params[0] = sf;

    lora_write_command(config, 0x8B, mod_params, 4);
}

void lora_set_bandwidth(lora_config_t* config, long sbw)
{
    uint8_t bw;
    if (sbw <= 7.8E3)
        bw = 0;
    else if (sbw <= 10.4E3)
        bw = 1;
    else if (sbw <= 15.6E3)
        bw = 2;
    else if (sbw <= 20.8E3)
        bw = 3;
    else if (sbw <= 31.25E3)
        bw = 4;
    else if (sbw <= 41.7E3)
        bw = 5;
    else if (sbw <= 62.5E3)
        bw = 6;
    else if (sbw <= 125E3)
        bw = 7;
    else if (sbw <= 250E3)
        bw = 8;
    else
        bw = 9;

    uint8_t mod_params[8];
    lora_read_command(config, 0x8B, mod_params, 4);
    mod_params[1] = bw;
    lora_write_command(config, 0x8B, mod_params, 4);
}

void lora_set_coding_rate(lora_config_t* config, int denominator)
{
    if (denominator < 5)
        denominator = 5;
    if (denominator > 8)
        denominator = 8;

    uint8_t mod_params[8];
    lora_read_command(config, 0x8B, mod_params, 4);
    mod_params[2] = denominator - 4;
    lora_write_command(config, 0x8B, mod_params, 4);
}

void lora_set_preamble_length(lora_config_t* config, long length)
{
    uint8_t pkt_params[9];
    lora_read_command(config, 0x8C, pkt_params, 9);
    pkt_params[1] = (length >> 8) & 0xFF;
    pkt_params[2] = length & 0xFF;
    lora_write_command(config, 0x8C, pkt_params, 9);
}

void lora_set_sync_word(lora_config_t* config, int sw)
{
    uint8_t sync_word[2] = {(sw >> 8) & 0xFF, sw & 0xFF};
    uint8_t buf[8] = {0x0, 0x0, 0x0, 0x0, sync_word[0], sync_word[1], 0x0, 0x0};
    lora_write_command(config, 0x0D, buf, 8);
}

void lora_send_packet(lora_config_t* config, uint8_t* buf, int size)
{
    // Ir a standby
    uint8_t standby_cfg = 0x00; // STDBY_RC
    lora_write_command(config, LORA_CMD_SET_STANDBY, &standby_cfg, 1);

    // Configurar buffer base
    uint8_t offset = 0x00;
    lora_write_command(config, 0x8F, &offset, 1);

    // Escribir payload
    uint8_t write_buf[size + 1];
    write_buf[0] = 0; // offset
    memcpy(&write_buf[1], buf, size);
    lora_write_command(config, LORA_CMD_WRITE_BUFFER, write_buf, size + 1);

    // Configurar tamaño del paquete
    uint8_t pkt_params[9];
    lora_read_command(config, 0x8C, pkt_params, 9);
    pkt_params[0] = 0x00; // Variable length
    pkt_params[3] = size;
    lora_write_command(config, 0x8C, pkt_params, 9);

    // Iniciar transmisión
    uint8_t tx_cfg[3] = {
        0x00, // Timeout MSB
        0x00, // Timeout LSB
        0x00  // Timeout disabled
    };
    lora_write_command(config, LORA_CMD_SET_TX, tx_cfg, 3);

    // Esperar fin de transmisión
    uint8_t irq_status[3];
    do
    {
        lora_read_command(config, LORA_CMD_GET_IRQ_STATUS, irq_status, 3);
        vTaskDelay(1);
    } while (!(irq_status[1] & 0x01)); // TX_DONE

    // Limpiar IRQ
    uint8_t clear_irq[2] = {0xFF, 0xFF};
    lora_write_command(config, LORA_CMD_CLEAR_IRQ_STATUS, clear_irq, 2);
}

int lora_receive_packet(lora_config_t* config, uint8_t* buf, int size)
{
    // Verificar si hay datos disponibles
    uint8_t rx_buffer_status[2];
    lora_read_command(config, LORA_CMD_GET_RX_BUFFER_STATUS, rx_buffer_status, 2);

    int len = rx_buffer_status[0];
    if (len > size)
        len = size;

    if (len > 0)
    {
        // Leer payload
        uint8_t offset = rx_buffer_status[1];
        uint8_t read_buf[len];
        lora_read_command(config, LORA_CMD_READ_BUFFER, read_buf, len);
        memcpy(buf, read_buf, len);

        // Limpiar IRQ
        uint8_t clear_irq[2] = {0xFF, 0xFF};
        lora_write_command(config, LORA_CMD_CLEAR_IRQ_STATUS, clear_irq, 2);
    }

    return len;
}

void lora_receive(lora_config_t* config)
{
    uint8_t rx_cfg[3] = {
        0x00, // Timeout MSB
        0x00, // Timeout LSB
        0x00  // Single mode
    };
    lora_write_command(config, LORA_CMD_SET_RX, rx_cfg, 3);
}

int lora_received(lora_config_t* config)
{
    uint8_t irq_status[3];
    lora_read_command(config, LORA_CMD_GET_IRQ_STATUS, irq_status, 3);
    return (irq_status[1] & 0x02) ? 1 : 0; // RX_DONE
}

int lora_packet_rssi(lora_config_t* config)
{
    uint8_t pkt_status[3];
    lora_read_command(config, LORA_CMD_GET_PACKET_STATUS, pkt_status, 3);
    return -pkt_status[0] / 2;
}

float lora_packet_snr(lora_config_t* config)
{
    uint8_t pkt_status[3];
    lora_read_command(config, LORA_CMD_GET_PACKET_STATUS, pkt_status, 3);
    return (int8_t)pkt_status[1] / 4.0;
}

void lora_sleep(lora_config_t* config)
{
    uint8_t sleep_cfg = 0x00;
    lora_write_command(config, LORA_CMD_SET_SLEEP, &sleep_cfg, 1);
}

void lora_wake(lora_config_t* config)
{
    gpio_set_level(config->lora_nss_pin, 0);
    vTaskDelay(1);
    gpio_set_level(config->lora_nss_pin, 1);
    vTaskDelay(1);
}

void lora_set_standby(lora_config_t* config)
{
    uint8_t standby_cfg = 0x00; // STDBY_RC
    lora_write_command(config, LORA_CMD_SET_STANDBY, &standby_cfg, 1);
}