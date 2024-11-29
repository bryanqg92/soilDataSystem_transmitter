/**
 * @file tft_manager.h
 * @brief Declaraciones y definiciones para la gestión de la pantalla TFT.
 *
 * Este archivo contiene las declaraciones de funciones y las inclusiones de 
 * encabezados necesarios para manejar la pantalla TFT utilizando el controlador 
 * HT_st7735 y el manejador de SPI para TFT.
 *
 * @author Leandro Quiroga
 * @date nov 2024
 */

 /**
    * @brief Tarea para actualizar la pantalla TFT.
    *
    * Esta función se ejecuta como una tarea en el sistema operativo en tiempo real 
    * (RTOS) y se encarga de actualizar el contenido mostrado en la pantalla TFT.
    *
    * @param pvParameters Puntero a los parámetros de la tarea (no utilizado).
    */
#ifndef TFT_MANAGER_H
#define TFT_MANAGER_H

#include "HT_st7735.h"
#include "tft_spi_handler.h"

void Task_TFTDisplay(void* pvParameters);

#endif /* TFT_MANAGER_H */
