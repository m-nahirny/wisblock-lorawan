cmake_minimum_required(VERSION 3.12)

# rest of your project
add_executable(wisblock_lorawan_hello_abp
    main.c
)

target_link_libraries(wisblock_lorawan_hello_abp pico_lorawan)

# enable usb output, disable uart output
pico_enable_stdio_usb(wisblock_lorawan_hello_abp 1)
pico_enable_stdio_uart(wisblock_lorawan_hello_abp 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(wisblock_lorawan_hello_abp)
