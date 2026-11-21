@echo off
setlocal enabledelayedexpansion

REM ============================================================
REM  J-Link flash script (Windows)
REM
REM  Flash <project>.hex, NOT <project>.elf.  See README.md.
REM
REM  Tools are resolved from PATH by default. Optional overrides:
REM    set JLINK_HOME=C:\Program Files\SEGGER\JLink
REM    set ARM_GCC_BIN=C:\path\to\arm-gnu-toolchain\bin
REM ============================================================

REM Project directory = parent of this script's directory.
REM Absolute paths keep the script independent of the current directory.
for %%i in ("%~dp0..") do (
    set "PROJECT_DIR=%%~fi"
    set "PROJECT_NAME=%%~nxi"
)
set "BUILD_DIR=!PROJECT_DIR!\build"
set "HEX_FILE=!BUILD_DIR!\!PROJECT_NAME!.hex"
set "ELF_FILE=!BUILD_DIR!\!PROJECT_NAME!.elf"

REM Target device. Change here if the project moves to another MCU.
set "DEVICE=MSPM0G3519"

REM ---------- objcopy (only needed when the HEX is missing) ----------
set "OBJCOPY=arm-none-eabi-objcopy"
if defined ARM_GCC_BIN if exist "!ARM_GCC_BIN!\arm-none-eabi-objcopy.exe" (
    set "OBJCOPY=!ARM_GCC_BIN!\arm-none-eabi-objcopy.exe"
)

REM ---------- Ensure the HEX exists ----------
if not exist "!HEX_FILE!" (
    if not exist "!ELF_FILE!" (
        echo [ERROR] Neither HEX nor ELF found in "!BUILD_DIR!". Build first.
        goto :fail
    )
    if /i "!OBJCOPY!"=="arm-none-eabi-objcopy" (
        where arm-none-eabi-objcopy >nul 2>nul
        if errorlevel 1 (
            echo [ERROR] arm-none-eabi-objcopy not found in PATH.
            echo         Add the toolchain to PATH or set ARM_GCC_BIN.
            goto :fail
        )
    )
    echo [INFO] HEX not found. Generating from ELF...
    "!OBJCOPY!" -O ihex --remove-section=.TrimTable --remove-section=.vtable --remove-section=.bss "!ELF_FILE!" "!HEX_FILE!"
    if not exist "!HEX_FILE!" (
        echo [ERROR] Failed to generate HEX.
        goto :fail
    )
)

echo Project : !PROJECT_NAME!
echo HEX     : !HEX_FILE!

REM ---------- J-Link ----------
set "JLINK_EXE=JLink.exe"
if defined JLINK_HOME if exist "!JLINK_HOME!\JLink.exe" set "JLINK_EXE=!JLINK_HOME!\JLink.exe"
if /i "!JLINK_EXE!"=="JLink.exe" (
    where JLink.exe >nul 2>nul
    if errorlevel 1 (
        echo [ERROR] JLink.exe not found in PATH.
        echo         Add SEGGER J-Link to PATH or set JLINK_HOME.
        goto :fail
    )
)

REM J-Link Commander accepts forward slashes more reliably.
set "HEX_JLINK=!HEX_FILE:\=/!"

set "TEMP_SCRIPT=!BUILD_DIR!\jlink_flash.jlink"
(
echo device !DEVICE!
echo if SWD
echo speed 4000
echo halt
echo loadfile !HEX_JLINK!
echo r
echo go
echo exit
) > "!TEMP_SCRIPT!"

"!JLINK_EXE!" -ExitOnError 1 -CommanderScript "!TEMP_SCRIPT!"
set "RC=!ERRORLEVEL!"
del "!TEMP_SCRIPT!" 2>nul

if not "!RC!"=="0" (
    echo [ERROR] J-Link failed with exit code !RC!
    goto :fail
)

echo [OK] Flash completed.
if /i not "%~1"=="--no-pause" pause
exit /b 0

:fail
if /i not "%~1"=="--no-pause" pause
exit /b 1
