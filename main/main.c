#include <stdio.h>
#include <string.h>
#include "logger.h"
#include "app.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"

#define RX_PIN GPIO_NUM_33 // Cambiar según tu configuración
#define TX_PIN GPIO_NUM_34 // Cambiar según tu configuración
#define UART_NUM UART_NUM_1

static const char *MAIN_TAG = "[main]";


void app_main(void)
{
    ESP_LOGI(MAIN_TAG, "Iniciando aplicación");
    app_init();
}


















/*

static const char *TAG = "UC6580";

void limpiar_buffer() {
    uint8_t data[2000];
    int len = 0;

    while ((len = uart_read_bytes(UART_NUM, data, sizeof(data), pdMS_TO_TICKS(100))) > 0) {
        ESP_LOGI(TAG, "Limpiando buffer: %d bytes", len);

    }
}

void enviar_comando(const char* comando) {
    // Limpiar el buffer de recepción antes de enviar el comando
    uart_flush(UART_NUM);

    // Enviar el comando de configuración
    uart_write_bytes(UART_NUM, comando, strlen(comando));
    ESP_LOGI(TAG, "Comando enviado: %s", comando);

    // Esperar respuesta
    uint8_t data[2000];
    int len = uart_read_bytes(UART_NUM, data, sizeof(data) - 1, pdMS_TO_TICKS(500)); // Limita el tiempo de espera a 500 ms
    ESP_LOGI(TAG, "LEN: %d", len);

    if (len > 0) {
        data[len] = '\0'; // Asegurarse de que la cadena termine correctamente
        ESP_LOGI(TAG, "Respuesta: %s", data);
    } else {
        ESP_LOGI(TAG, "No se recibió respuesta");
    }
}

void solo_leer(void) {
    // Limpiar el buffer de recepción antes de enviar el comando
    uart_flush(UART_NUM);
    uint8_t data[2000];
    int len = uart_read_bytes(UART_NUM, data, sizeof(data) - 1, pdMS_TO_TICKS(500)); // Limita el tiempo de espera a 500 ms
    ESP_LOGI(TAG, "LEN: %d", len);

    if (len > 0) {
        data[len] = '\0'; // Asegurarse de que la cadena termine correctamente
        ESP_LOGI(TAG, "Respuesta: %s", data);
    } else {
        ESP_LOGI(TAG, "No se recibió respuesta");
    }
}

void app_main(void)
{
    // Configuración del UART
    // app_init();
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    // Instalar UART driver y configurar pines
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, 4096, 0, 0, NULL, 0);

    gpio_set_direction(3, GPIO_MODE_OUTPUT);
    gpio_set_level(3, 1);

    // Enviar comandos al módulo UC6580
    //const char *cmd1 = "$CFGMSG,0,7\r\n";
    //const char *cmd1 = "$CFGNMEA\r\n";
    const char *cmd1 = "$RESET,0,HFF\r\n";//"$CFGSYS\r\n";
    // const char *cmd1 = "$CFGMSG,0,1,1*2E\r\n";   // Habilitar mensajes NMEA
    // const char *cmd2 = "$CFGSYS,1,1,1,1*32\r\n"; // Múltiples constelaciones
    
    //limpiar_buffer();
    //enviar_comando(cmd1);

    //int i = 0; // Empezamos desde 0
    while (1)
    {
        //ESP_LOGI(TAG, "Enviando comando: %s", cmd1);
        //enviar_comando(cmd1);
        solo_leer();
        vTaskDelay(pdMS_TO_TICKS(1000)); // Espera de 5 segundos entre envíos
    }
}


$CFGNMEA,H52*26
$CFGMSG,0,1,0*06
$CFGMSG,0,2,1*04
$CFGMSG,0,3,1*05
$CFGMSG,0,4,1*02
$CFGMSG,0,5,0*02
$CFGMSG,0,6,0*01
$CFGMSG,0,7,0*00

$GNRMC,061533.00,V,,,,,,,121024,,,N,V*1F
$GNGGA,061533.00,,,,,0,00,99.99,,,,,,*7A
$GNGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99,1*33
$GNGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99,2*30
$GNGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99,4*36
$GNGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99,3*31
$GNGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99,5*37
$GPGSV,2,1,07,10,63,158,,12,16,045,,23,22,149,,25,45,075,,1*64
$GPGSV,2,2,07,26,20,191,,28,69,289,,32,59,021,,1*54
$GPGSV,2,1,07,10,63,158,,12,16,045,,23,22,149,,25,45,075,,8*6D
$GPGSV,2,2,07,26,20,191,,28,69,289,,32,59,021,,8*5D
$GLGSV,2,1,05,66,40,088,,67,52,003,,76,24,031,,77,70,055,,1*7D
$GLGSV,2,2,05,78,40,195,,1*4B
$GBGSV,3,1,10,03,54,167,,08,56,168,,13,68,188,,16,71,343,,1*7B
$GBGSV,3,2,10,21,73,043,,22,31,115,,26,63,205,,39,67,001,,1*7D
$GBGSV,3,3,10,45,54,049,,65,37,125,,1*7B
$GBGSV,3,1,10,03,54,167,,08,56,168,,13,68,188,,16,71,343,,5*7F
$GBGSV,3,2,10,21,73,043,,22,31,115,,26,63,205,,39,67,001,,5*79
$GBGSV,3,3,10,45,54,049,,65,37,125,,5*7F
$GAGSV,2,1,06,06,30,077,,09,48,056,,24,47,140,,31,37,065,,7*70
$GAGSV,2,2,06,40,06,260,,43,33,238,,7*7D
$GAGSV,2,1,06,06,30,077,,09,48,056,,24,47,140,,31,37,065,,1*76
$GAGSV,2,2,06,40,06,260,,43,33,238,,1*7B
$GQGSV,1,1,02,42,37,125,,50,46,140,,1*61
$GQGSV,1,1,02,42,37,125,,50,46,140,,8*68
$GNTXT,01,01,01,0,000002,0000,0020,0020,144.194,0,0001F7FF01BA0200*2D
*/

