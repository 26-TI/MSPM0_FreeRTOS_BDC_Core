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

# 生成按 LMA（绝对地址）编址的 Intel HEX，供烧录工具使用。
# 原因：直接烧 ELF 时 J-Link 按 VMA 加载，.data 的初始化数据会被写进 RAM 而不是
# Flash 的 LMA，复位后 Reset_Handler 从 Flash 拷贝到 .data 的全是 0xFF。
# HEX 使用 LMA；同时排除位于 SRAM 的 .TrimTable/.vtable/.bss，避免调试器写 SRAM
# 时绕过奇偶校验码导致 SRAMDED NMI。
# objcopy 优先取编译器同目录下的版本，找不到时再退回 PATH。
get_filename_component(_TOOLCHAIN_BIN "${CMAKE_C_COMPILER}" DIRECTORY)
find_program(_OBJCOPY_AUTO NAMES arm-none-eabi-objcopy
    HINTS "${_TOOLCHAIN_BIN}" NO_DEFAULT_PATH NO_CACHE)
if (_OBJCOPY_AUTO)
    set(_OBJCOPY "${_OBJCOPY_AUTO}")
else ()
    set(_OBJCOPY "${CMAKE_OBJCOPY}")
endif ()

add_custom_command(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD
    COMMAND ${_OBJCOPY} -O ihex
            --remove-section=.TrimTable
            --remove-section=.vtable
            --remove-section=.bss
            $<TARGET_FILE:${CMAKE_PROJECT_NAME}.elf>
            ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.hex
    COMMENT "Generating ${CMAKE_PROJECT_NAME}.hex (LMA-based, flash only)"
    VERBATIM
)
