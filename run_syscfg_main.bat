@echo off
set SYSCONFIG_DIR=D:\CCS\sysconfig
set SDK_PATH=C:\ti\mspm0_sdk_2_10_00_04
for %%f in (%~dp0SysConfig\*.syscfg) do set "SYSCFG_FILE=%%f"
set OUTPUT_DIR=%~dp0SysConfig

echo ============================================
echo   MSPM0 SysConfig
echo ============================================
echo   SysConfig: %SYSCONFIG_DIR%
echo   SDK:       %SDK_PATH%
echo   Config:    %SYSCFG_FILE%
echo ============================================
echo   1. Edit in GUI, Ctrl+S, then close window
echo   2. CLI auto-generates ti_msp_dl_config.c/h
echo ============================================
echo.

IF NOT EXIST "%SYSCONFIG_DIR%\sysconfig_gui.bat" (
    echo [ERROR] sysconfig_gui.bat not found!
    echo Download: https://www.ti.com/tool/SYSCONFIG
    echo Or use online: https://dev.ti.com/sysconfig/
    pause
    exit /b 1
)

echo [1/2] Opening GUI...
call "%SYSCONFIG_DIR%\sysconfig_gui.bat" --product "%SDK_PATH%\.metadata\product.json" --output "%OUTPUT_DIR%" "%SYSCFG_FILE%"

echo.
echo    Edit in GUI, Ctrl+S, then close window.
echo    When done, press any key here to generate code...
pause >nul
echo [2/2] Generating code...
"%SYSCONFIG_DIR%\nodejs\node.exe" "%SYSCONFIG_DIR%\dist\cli.js" --product "%SDK_PATH%\.metadata\product.json" --output "%OUTPUT_DIR%" "%SYSCFG_FILE%"

echo.
echo Done.
pause
