cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(wisblock_lorawan_otaa_temperature_led C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# rest of your project
add_executable(wisblock_lorawan_otaa_temperature_led
    main.c
)

# enable usb output, disable uart output
pico_enable_stdio_usb(wisblock_lorawan_otaa_temperature_led 1)
pico_enable_stdio_uart(wisblock_lorawan_otaa_temperature_led 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(wisblock_lorawan_otaa_temperature_led)

target_link_libraries(wisblock_lorawan_otaa_temperature_led pico_stdlib pico_lorawan hardware_adc)
