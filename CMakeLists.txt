cmake_minimum_required(VERSION 3.27)
include(mspm0g351x_base.cmake)

project(MSPM0_FreeRTOS_BDC_Core C CXX ASM)

file(GLOB FREERTOS_C_SOURCES "./FreeRTOS/*.c")
file(GLOB FREERTOS_MEMMANG_C_SOURCES "./FreeRTOS/portable/MemMang/heap_4.c")
file(GLOB_RECURSE FREERTOS_PORTABLE_C_SOURCES "./FreeRTOS/portable/GCC/ARM_CM0/*.c")

file(GLOB_RECURSE USER_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    User/*.c
    User/*.cpp
)

add_executable(${CMAKE_PROJECT_NAME}.elf
    ${SYSCONFIG_SOURCES}
    ${MSPM0_SDK_SOURCES}
    ${MSPM0_STARTUP}
    ${FREERTOS_C_SOURCES}
    ${FREERTOS_PORTABLE_C_SOURCES}
    ${FREERTOS_MEMMANG_C_SOURCES}
    ${USER_SOURCES}

)

include_directories(
    ./FreeRTOS/include
    ./FreeRTOS/portable/GCC/ARM_CM0/
    ./User/App
    ./User/App/task
    ./User/Bsp
    ./User/Device
    ./User/Service
    ./User/Algorithm
    ./User/Protocol
    ./User/Module
)
