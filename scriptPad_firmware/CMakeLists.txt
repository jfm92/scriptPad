cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

############################ Specific PICO SDK Config ############################
set(FAMILY rp2040)
set(BOARD pico_sdk)

set(PICO_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libraries/pico-sdk")
set(FREERTOS_KERNEL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libraries/FreeRTOS-Kernel")


include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)
include(${PICO_SDK_PATH}/lib/tinyusb/hw/bsp/family_support.cmake)


project(scriptPad C CXX ASM)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.5.1")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.5.1 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()


pico_sdk_init()

set(TINYUSB_FAMILY_PROJECT_NAME_PREFIX "tinyusb_host_")

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
            ${CMAKE_CURRENT_SOURCE_DIR}/inputsManagement/switchManagement.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/HIDManagement/HIDManagement.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/filesManagement/filesManagement.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/configManagement/configManagement.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/usb_descriptors.c
)   

target_include_directories(scriptPad PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/inputsManagement
            ${CMAKE_CURRENT_SOURCE_DIR}/HIDManagement
            ${CMAKE_CURRENT_SOURCE_DIR}/filesManagement
            ${CMAKE_CURRENT_SOURCE_DIR}/configManagement
)

################################# External Libraries ##################################################

target_include_directories(scriptPad PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/libraries/cJSON
            ${PICO_SDK_PATH}/lib/lwip/src/include
            ${PICO_SDK_PATH}/lib/lwip/src/include/ipv4
            ${PICO_SDK_PATH}/lib/lwip/src/include/lwip/apps
            ${PICO_SDK_PATH}/lib/tinyusb/lib/networking
            )

target_sources(scriptPad PUBLIC
            ${PICO_SDK_PATH}/lib/lwip/src/core/altcp.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/altcp_alloc.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/altcp_tcp.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/def.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/dns.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/inet_chksum.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/init.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/ip.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/mem.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/memp.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/netif.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/pbuf.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/raw.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/stats.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/sys.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/tcp.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/tcp_in.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/tcp_out.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/timeouts.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/udp.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/ipv4/autoip.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/ipv4/dhcp.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/ipv4/etharp.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/ipv4/icmp.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/ipv4/igmp.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/ipv4/ip4.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/ipv4/ip4_addr.c
            ${PICO_SDK_PATH}/lib/lwip/src/core/ipv4/ip4_frag.c
            ${PICO_SDK_PATH}/lib/lwip/src/netif/ethernet.c
            ${PICO_SDK_PATH}/lib/lwip/src/netif/slipif.c
            ${PICO_SDK_PATH}/lib/lwip/src/apps/http/httpd.c
            ${PICO_SDK_PATH}/lib/lwip/src/apps/http/fs.c
            ${PICO_SDK_PATH}/lib/tinyusb/lib/networking/dhserver.c
            ${PICO_SDK_PATH}/lib/tinyusb/lib/networking/dnserver.c
            ${PICO_SDK_PATH}/lib/tinyusb/lib/networking/rndis_reports.c
            ${CMAKE_CURRENT_SOURCE_DIR}/libraries/cJSON/cJSON.c
            )

add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/libraries/pico-filesystem pfs)

################################# External Libraries End ##################################################

# We only want UART on debug mode
#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    pico_enable_stdio_usb(scriptPad 0)
    pico_enable_stdio_uart(scriptPad 1)
#endif()

target_link_libraries(scriptPad PUBLIC 
                        flash_filesystem
                        pico_stdlib 
                        pico_unique_id
                        tinyusb_device    # This is required for TinyUSB
                        tinyusb_board    # Some additional board stuff for TinyUSB
                        FreeRTOS-Kernel-Heap4
                        )



pico_add_extra_outputs(scriptPad)