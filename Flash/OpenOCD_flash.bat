@echo off
setlocal enabledelayedexpansion

REM ============================================================
REM  OpenOCD flash script (Windows, CMSIS-DAP)
REM
REM  Prefer <project>.hex (LMA-based, no SRAM sections).
REM  OpenOCD handles ELF LMA correctly, so ELF is a safe fallback.
REM
REM  OpenOCD is resolved from PATH by default. Optional override:
REM    set OPENOCD_HOME=C:\path\to\openocd\bin
REM ============================================================

for %%i in ("%~dp0..") do (
    set "PROJECT_DIR=%%~fi"
    set "PROJECT_NAME=%%~nxi"
)
set "BUILD_DIR=!PROJECT_DIR!\build"
set "HEX_FILE=!BUILD_DIR!\!PROJECT_NAME!.hex"
set "ELF_FILE=!BUILD_DIR!\!PROJECT_NAME!.elf"

set "TARGET_FILE="
if exist "!HEX_FILE!" (
    set "TARGET_FILE=!HEX_FILE!"
) else if exist "!ELF_FILE!" (
    echo [WARN] HEX not found. Using ELF instead: !ELF_FILE!
    set "TARGET_FILE=!ELF_FILE!"
)

if "!TARGET_FILE!"=="" (
    echo [ERROR] Neither HEX nor ELF found in "!BUILD_DIR!". Build first.
    goto :fail
)

REM OpenOCD prefers forward slashes.
set "TARGET_OCD=!TARGET_FILE:\=/!"
set "DAPLINK_CFG=%~dp0daplink.cfg"
set "DAPLINK_CFG=!DAPLINK_CFG:\=/!"

REM ---------- OpenOCD ----------
set "OPENOCD=openocd"
if defined OPENOCD_HOME if exist "!OPENOCD_HOME!\openocd.exe" set "OPENOCD=!OPENOCD_HOME!\openocd.exe"
if /i "!OPENOCD!"=="openocd" (
    where openocd >nul 2>nul
    if errorlevel 1 (
        echo [ERROR] openocd not found in PATH.
        echo         Add OpenOCD to PATH or set OPENOCD_HOME.
        goto :fail
    )
)

echo Project : !PROJECT_NAME!
echo Target  : !TARGET_FILE!

"!OPENOCD!" -f "!DAPLINK_CFG!" -c "program \"!TARGET_OCD!\" verify reset exit"
set "RC=!ERRORLEVEL!"

if not "!RC!"=="0" (
    echo [ERROR] OpenOCD failed with exit code !RC!
    goto :fail
)

echo [OK] Flash completed.
if /i not "%~1"=="--no-pause" pause
exit /b 0

:fail
if /i not "%~1"=="--no-pause" pause
exit /b 1
