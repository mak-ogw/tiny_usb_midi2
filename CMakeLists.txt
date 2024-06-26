cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)

set(PROJ "tiny_usb_midi2")

project(${PROJ} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(FAMILY rp2040)

# set tinyusb path
set(PICO_TINYUSB_PATH ${PROJECT_SOURCE_DIR}/../pico-sdk/lib/tinyusb)

include(${PICO_TINYUSB_PATH}/hw/bsp/family_support.cmake)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/freertos)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/)

family_get_project_name(PROJECT ${CMAKE_CURRENT_LIST_DIR})
family_initialize_project(${PROJECT} ${CMAKE_CURRENT_LIST_DIR})

add_compile_options(-Wall
        -Wno-error=shadow
        -g3 -O0
        -Wno-error=format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-error=unused-function # we have some for the docs that aren't called
        -Wno-error=maybe-uninitialized
        -Wno-fatal-errors # -Wfatal-errors
        -Wno-error=unused-variable
        -Wno-error=unused-parameter
        -Wno-error=cast-align
        -Wno-error=conversion
        -Wno-error=implicit-fallthrough
        -DLOG=1
        )

add_executable(${PROJ}
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/usb_descriptors.cpp
        ${CMAKE_CURRENT_LIST_DIR}/lib/tusb_ump/ump_device.cpp
)

target_link_libraries(${PROJ} PRIVATE freertos libmidi2 microshell)

include_directories(${PROJ} PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${CMAKE_CURRENT_LIST_DIR}/lib
        )

pico_enable_stdio_usb(${PROJ} 1)
pico_enable_stdio_uart(${PROJ} 1)

pico_add_extra_outputs(${PROJ})
family_configure_device_example(${PROJ})

