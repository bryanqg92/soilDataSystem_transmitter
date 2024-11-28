# NPK Reader ESP-IDF (Aún en construcción)

Este repositorio contiene el código para el proyecto NPK Reader ESP, que es parte del trabajo de grado de la CESE en la UBA.


## Descripción

El proyecto NPK Reader ESP está diseñado para leer y transmitir datos de sensores NPK utilizando un microcontrolador ESP. Este proyecto incluye el código necesario para la configuración, lectura y transmisión de datos.

Este contiene dos modos de opoeración **Extracción** y **Predicción**. Ambos modos de operación se podrá visualizar datos en la pantalla, No obstante, los eventos indicarán que hacer con los datos.

### Modo extracción:
- Este modo de operación permitrira almacenar datos en una micro sd, en un archivo csv para pen analisis de los datos qu epermitan mejorar el modelo de ML o IA a utilizar. En este caso no interactúa con el dispositivo receptor.

### Modo Predicción:
-Para el uso en este modo se asumirá que existe un STM32 receptor que será capaz de recibir todos los datos en el mismo formato que son transmitidos desd el módulo LoRa. Aquí se podrán visualizar los datos pero al recibir una respuesta en el buffer de LoRa, Este cambiará la visualización de pantalla con la predicción realizada por el modelo receptor.

## Estructura del Proyecto

- **/api**: Contiene funciones de alto nivel para acceder a perifericos
- **/app**: Contiene la lógica del aplicativo, solo interactúa con la API
- **/peripherals**: Inicializa directamente los perifericos a utilizar

## Requisitos

- Microcontrolador ESP32s3 (este repoditorio usa la placa Wireless tracking de HELTEC)
- Sensor NPK 7 EN 1
- Prerequisitos de instalación de esp-idf de https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html
