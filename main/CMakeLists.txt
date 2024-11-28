# Encontrar todos los archivos fuente .c recursivamente
file(GLOB_RECURSE SOURCES 
    "../app/src/*.c"
    "../api/uart/src/*.c"
    "../api/gnss/src/*.c"
    "../api/tft_display/src/*.c"
    "../peripherals/src/*.c"
)

# Encontrar todos los directorios que contienen headers
file(GLOB INCLUDE_DIRS 
    "../app/inc"
    "../api/commons/inc"
    "../api/uart/inc"
    "../api/gnss/inc"
    "../api/tft_display/inc"
    "../peripherals/inc"
)

idf_component_register(
    SRCS "main.c" ${SOURCES}
    INCLUDE_DIRS "." ${INCLUDE_DIRS}
)