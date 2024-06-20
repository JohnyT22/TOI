FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/main/*.*)

set(EXTRA_COMPONENT_DIRS ../../components/)

idf_component_register(SRCS ${app_sources}
        REQUIRES driver
        PRIV_REQUIRES nvs_flash mqtt esp32-owb esp32-ds18b20 esp32-DHT11 tflite-lib)
