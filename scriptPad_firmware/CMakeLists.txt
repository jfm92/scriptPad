cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

############################ Specific PICO SDK Config ############################
set(FAMILY rp2040)
set(BOARD pico_sdk)

set(PICO_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk")

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(scriptPad C CXX ASM)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.5.1")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.5.1 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

pico_sdk_init()

###################################################################################

add_compile_options(-Wall
        -Wno-error=null-dereference
        -Wno-error=conversion
        -Wno-error=sign-conversion
        -Wno-error=sign-compare
        )


if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()
    
add_executable(scriptPad main.cpp)

# Source Files to include (Only specific developed for this project)
target_sources(scriptPad PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(scriptPad PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(scriptPad PUBLIC pico_stdlib)

pico_add_extra_outputs(scriptPad)