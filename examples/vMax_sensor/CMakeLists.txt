cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(vmax_sensor C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# rest of your project
add_executable(vmax_sensor
    # main.c
    vmax_sensor.cpp
    EPD_Device.cpp
    EPD_2in7.cpp
    GUI_Paint.c
    Fonts/font_20.c
    Fonts/font_24.c
    Fonts/font_32.c
    Fonts/font_64.c
    AccelerationSeries.cpp
    Lis3dh.cpp
)

target_include_directories(vmax_sensor PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
)

# enable usb output, disable uart output
pico_enable_stdio_usb(vmax_sensor 1)
pico_enable_stdio_uart(vmax_sensor 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(vmax_sensor)

target_link_libraries(vmax_sensor 
    pico_stdlib 
    pico_lorawan 
    hardware_rtc
    hardware_i2c
    hardware_sleep
    pico_runtime
    hardware_adc
    hardware_uart
    pico_multicore
    )
