# Encontrar todos los archivos fuente .c recursivamente
file(GLOB_RECURSE SOURCES 
    "../app/src/*.c"
    "../api/api_lora/src/*.c"
    "../api/button_debounce/*.c"
    "../api/uart/src/*.c"
    "../api/gnss/src/*.c"
    "../api/tft_display/src/*.c"
    "../api/peripherals/src/*.c"
    "../api/sd_card/src/*.c"
)

# Encontrar todos los directorios que contienen headers
file(GLOB INCLUDE_DIRS 
    "../app/inc"
    "../api/api_lora/inc"
    "../api/button_debounce/inc"
    "../api/commons/inc"
    "../api/uart/inc"
    "../api/gnss/inc"
    "../api/tft_display/inc"
    "../api/peripherals/inc"
    "../api/sd_card/inc"
)

idf_component_register(
    SRCS "main.c" ${SOURCES}
    INCLUDE_DIRS "." ${INCLUDE_DIRS}
)