cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(useful_pico_tiny_code_reader C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(code_reading_example
        code_reading_example.c
        )
target_link_libraries(code_reading_example pico_stdlib hardware_i2c)
pico_add_extra_outputs(code_reading_example)
pico_enable_stdio_usb(code_reading_example 1)

add_executable(scan_i2c
        scan_i2c.c
        )
target_link_libraries(scan_i2c pico_stdlib hardware_i2c)
pico_add_extra_outputs(scan_i2c)
pico_enable_stdio_usb(scan_i2c 1)

if (PICO_CYW43_SUPPORTED) # set by BOARD=pico-w
        add_executable(wifi_provisioning_example
                string_utils.c
                wifi_provisioning_example.c
                )
        target_include_directories(wifi_provisioning_example PRIVATE
                ${CMAKE_CURRENT_LIST_DIR}
                ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )
        target_link_libraries(wifi_provisioning_example 
                pico_lwip
                pico_cyw43_arch_lwip_poll
                pico_cyw43_arch
                pico_stdlib 
                hardware_i2c)
        pico_add_extra_outputs(wifi_provisioning_example)
        pico_enable_stdio_usb(wifi_provisioning_example 1)
endif()