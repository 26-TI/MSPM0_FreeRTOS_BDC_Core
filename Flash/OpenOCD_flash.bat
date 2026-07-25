@echo off
REM 获取当前脚本所在目录的父目录名称（项目名称）
for %%i in ("%~dp0\..") do set "PROJECT_NAME=%%~nxi"

REM 构建 ELF 文件路径（使用正斜杠避免 OpenOCD 路径问题）
set "ELF_FILE=build/%PROJECT_NAME%.elf"
set "ELF_FILE=%ELF_FILE:\=/%"

REM 检查文件是否存在
if not exist "%ELF_FILE%" (
    echo Error: ELF file not found at %ELF_FILE%
    exit /b 1
)

REM OpenOCD 路径
set "OPENOCD_PATH=D:\Toolchain\OpenOCD-20260302\bin\openocd.exe"

REM DAPLink 配置文件路径
set "DAPLINK_CFG=%~dp0daplink.cfg"

REM 烧录
"%OPENOCD_PATH%" -f "%DAPLINK_CFG%" -c "program \"%ELF_FILE%\" verify reset exit"
pause