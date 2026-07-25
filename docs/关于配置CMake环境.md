# MSPM0 CMake 工程构建与开发环境配置

> 本文档涵盖工具链安装、CMake 配置、clangd、烧录与调试的完整流程。
> 目标平台：**TI MSPM0G3507 (Cortex-M0+)**，工程已预配大部分内容，本文档面向**首次搭建开发环境**的场景。

---

## 目录

- [MSPM0 CMake 工程构建与开发环境配置](#mspm0-cmake-工程构建与开发环境配置)
  - [目录](#目录)
  - [1. 前提条件](#1-前提条件)
    - [1.1 工具链清单](#11-工具链清单)
    - [1.2 版本要求](#12-版本要求)
  - [2. 工具链安装](#2-工具链安装)
    - [2.1 ARM GCC 交叉编译器](#21-arm-gcc-交叉编译器)
    - [2.2 CMake + Ninja](#22-cmake--ninja)
    - [2.3 TI MSPM0 SDK](#23-ti-mspm0-sdk)
    - [2.4 OpenOCD](#24-openocd)
    - [2.5 JLink](#25-jlink)
    - [2.6 环境变量配置](#26-环境变量配置)
  - [3. VSCode 插件](#3-vscode-插件)
    - [3.1 必装插件](#31-必装插件)
    - [3.2 推荐插件](#32-推荐插件)
  - [4. CMake 构建配置](#4-cmake-构建配置)
    - [4.1 工程 CMake 结构](#41-工程-cmake-结构)
    - [4.2 CMakePresets 说明](#42-cmakepresets-说明)
    - [4.3 构建类型](#43-构建类型)
  - [5. clangd 智能提示配置](#5-clangd-智能提示配置)
  - [6. 烧录配置](#6-烧录配置)
    - [6.1 OpenOCD 烧录](#61-openocd-烧录)
    - [6.2 JLink 烧录](#62-jlink-烧录)
  - [7. 调试配置](#7-调试配置)
    - [7.1 Cortex-Debug 设置](#71-cortex-debug-设置)
    - [7.2 launch.json 模板](#72-launchjson-模板)
  - [8. 日常使用流程](#8-日常使用流程)

---

## 1. 前提条件

### 1.1 工具链清单

| 工具 | 用途 | 必选 |
|------|------|:--:|
| ARM GCC (`arm-none-eabi-*`) | 交叉编译器，生成 Cortex-M0+ 机器码 | ✅ |
| CMake (≥ 3.27) | 跨平台构建系统 | ✅ |
| Ninja | 高速构建后端 | ✅ |
| TI MSPM0 SDK (2.10.00.04) | TI 官方驱动库 + 启动文件 + 链接脚本 | ✅ |
| OpenOCD | 开源调试器，配合 CMSIS-DAP | ✅ |
| JLink (SEGGER) | 商业调试器，更稳定（推荐烧录） | 推荐 |

### 1.2 版本要求

| 工具 | 版本 | 验证命令 |
|------|------|---------|
| ARM GCC | 14.x+ | `arm-none-eabi-gcc --version` |
| CMake | ≥ 3.27 | `cmake --version` |
| Ninja | 任意 | `ninja --version` |
| TI MSPM0 SDK | 2.10.00.04 | 检查安装路径 |
| OpenOCD | 0.12+ | `openocd --version` |

---

## 2. 工具链安装

### 2.1 ARM GCC 交叉编译器

**Windows：**
从 [ARM GNU Toolchain 官网](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain) 下载 Windows `.exe` 安装包，安装时勾选「Add to PATH」。

**Linux（Ubuntu/Debian）：**

```bash
sudo apt install gcc-arm-none-eabi
```

或从 ARM 官网下载 tar 包手动解压到 `/opt`：

```bash
tar -xf arm-gnu-toolchain-*.tar.xz -C /opt
export PATH="/opt/arm-gnu-toolchain-*/bin:$PATH"
```

### 2.2 CMake + Ninja

**Windows：**
- CMake: [cmake.org](https://cmake.org/download/) 下载 `.msi` 安装
- Ninja: [ninja-build.org](https://ninja-build.org/) 下载 `ninja.exe`，放到 PATH 中的目录

**Linux：**

```bash
sudo apt install cmake ninja-build
```

### 2.3 TI MSPM0 SDK

从 [TI MSPM0-SDK 页面](https://www.ti.com/tool/MSPM0-SDK) 下载 SDK（本工程使用 2.10.00.04），安装到本地目录，例如：

```
Linux:   /home/rh/ti/mspm0_sdk_2_10_00_04
Windows: C:/ti/mspm0_sdk_2_10_00_04
```

然后在工程根目录复制 `mspm0g350x_base.cmake.txt` → `mspm0g350x_base.cmake`，修改其中的 `MSPM0_SDK_PATH` 为实际路径。

SDK 提供5件在本工程中的使用：
- `source/ti/driverlib/*.c` → 驱动库源码（GPIO/UART/SPI/I2C/CAN 等）
- `source/ti/devices/msp/m0p/startup_system_files/gcc/startup_mspm0g350x_gcc.c` → 启动文件
- `source/third_party/CMSIS/Core/Include` → CMSIS 头文件

### 2.4 OpenOCD

**Windows：**
从 [OpenOCD GitHub Releases](https://github.com/openocd-org/openocd/releases) 下载 Windows 包，解压后将其 `bin` 目录加入 PATH。

**Linux：**

```bash
sudo apt install openocd
```

### 2.4 JLink

**Windows：**
从 [SEGGER 官网](https://www.segger.com/downloads/jlink/) 下载 JLink Software and Documentation Pack，安装即可。

**Linux：**

```bash
# 下载 .deb 包后
sudo dpkg -i JLink_Linux_*.deb
```

### 2.6 环境变量配置

**Windows：** 将以下各工具的 `bin` 目录加入系统 PATH：

```
D:\Toolchain\arm-gnu-toolchain-14.3.rel1\bin
D:\Toolchain\cmake-4.12.1\bin
D:\Toolchain\ninja
D:\Toolchain\OpenOCD-20250710\bin
```

> 路径按实际安装位置修改。配置后重启终端或 VSCode 生效。

**Linux：** 通过包管理器安装的工具通常自动在 PATH 中。手动安装的需在 `~/.bashrc` 添加：

```bash
export PATH="/opt/arm-gnu-toolchain-14.3.rel1/bin:$PATH"
export PATH="/opt/JLink:$PATH"
```

---

## 3. VSCode 插件

### 3.1 必装插件

| 插件 | ID | 用途 |
|------|-----|------|
| CMake Tools | `ms-vscode.cmake-tools` | CMake 图形化配置/编译 |
| clangd | `llvm-vs-code-extensions.vscode-clangd` | 语法高亮/补全/跳转 |
| Cortex-Debug | `marus25.cortex-debug` | ARM 调试前端 |
| LinkerScript | `zixuanwang.linkerscript` | `.ld` 链接脚本语法高亮 |

> ⚠️ clangd 与微软 C/C++ 插件冲突，二者只装其一。

### 3.2 推荐插件

| 插件 | 用途 |
|------|------|
| Clang-Format | 代码格式化 |
| MemoryView | 调试时查看内存 |
| Peripheral Viewer | 调试时查看外设寄存器 |
| RTOS Views | 调试时查看 FreeRTOS 任务 |
| Serial Monitor | 微软官方串口助手 |
| Output Colorizer | 终端输出着色 |
| Doxygen Documentation Generator | Doxygen 注释高亮 |
| VsCodeTaskButtons | 状态栏快捷按钮 |

> 本工程 MCU 为 MSPM0G3507，无需安装 STM32 设备支持包。

---

## 4. CMake 构建配置

### 4.1 工程 CMake 结构

本工程 CMake 构建分为两层：

| 文件 | 作用 |
|------|------|
| `CMakeLists.txt` | 顶层构建入口：收集源文件、设置 include 路径、生成 elf |
| `mspm0g350x_base.cmake` | 工具链配置：编译器、SDK 路径、编译选项、链接脚本 |
| `CMakePresets.json` | CMake Tools 预设：编译器路径、构建类型 |

`mspm0g350x_base.cmake` 中的关键编译选项（MSPM0G3507 = Cortex-M0+）：

```cmake
-mcpu=cortex-m0plus -march=armv6-m -mthumb -mfloat-abi=soft
```

链接脚本由 SysConfig 自动生成（`SysConfig/device_linker.lds`），无需手动编写。

### 4.2 CMakePresets 说明

工程提供 `CMakePresets.json`，预设编译器为 ARM GCC 15.2.1：

```json
{
  "configurePresets": [
    {
      "name": "Debug",
      "displayName": "GCC 15.2.1 arm-none-eabi",
      "binaryDir": "${sourceDir}/build",
      "cacheVariables": {
        "CMAKE_C_COMPILER":   "你的路径/arm-none-eabi-gcc",
        "CMAKE_CXX_COMPILER": "你的路径/arm-none-eabi-g++",
        "CMAKE_BUILD_TYPE":   "Debug"
      }
    }
  ]
}
```

> 模板文件为 `CMakePresets.json.txt`，复制后按本机编译器路径修改。

### 4.3 构建类型

| CMAKE_BUILD_TYPE | 编译选项 | 适用场景 |
|-----------------|---------|---------|
| **Debug** | `-O0 -g` | 日常开发、调试 |
| **Release** | `-Ofast` | 最终部署，最大速度优化 |
| **RelWithDebInfo** | `-Ofast -g` | 带调试信息的优化构建 |
| **MinSizeRel** | `-Os` | 最小体积优化 |

VSCode 状态栏 CMake 图标可一键切换构建类型。

---

## 5. clangd 智能提示配置

clangd 依赖 `compile_commands.json`。工程编译后自动生成在 `build/compile_commands.json`。

`.vscode/settings.json` 中指定路径：

```json
{
  "clangd.arguments": [
    "--compile-commands-dir=build"
  ]
}
```

> 若构建目录改变，需同步修改此路径。

**头文件误报处理**：clangd 可能无法识别间接使用的头文件，在 `#include` 末尾添加注释即可消除警告：

```c
#include "ti_msp_dl_config.h"  // IWYU pragma: keep
```

---

## 6. 烧录配置

> 工程已提供烧录脚本（`Flash/` 目录），可通过 VSCode Task 按钮一键烧录。
> 详见 [README.md 第 4.2 节](../README.md#42-烧录)。

### 6.1 OpenOCD 烧录

配置文件 `Flash/daplink.cfg`（CMSIS-DAP 接口，SWD 协议，MSPM0G3507 目标）。

命令行烧录示例：

```bash
openocd -f Flash/daplink.cfg \
  -c "program build/MSPM0_FreeRTOS_Ctrl.elf verify reset exit"
```

| 参数 | 说明 |
|------|------|
| `-f` | 指定配置文件 |
| `program` | 烧录 ELF 文件 |
| `verify` | 烧录后校验 |
| `reset` | 烧录后复位 MCU |
| `exit` | 完成后退出 OpenOCD |

> ⚠️ OpenOCD + MSPM0 偶发芯片锁死问题，**推荐优先使用 JLink**。

### 6.2 JLink 烧录

使用 SEGGER JLinkExe。工程提供 `Flash/JLink_flash.sh`（Linux）和 `Flash/JLink_flash.bat`（Windows），直接运行即可。需确保 `JLinkExe` 在 PATH 中。

---

## 7. 调试配置

### 7.1 Cortex-Debug 设置

在 VSCode `settings.json` 中配置工具链路径（**路径按实际修改**）：

**Windows：**

```json
{
  "cortex-debug.armToolchainPath": "D:\\Toolchain\\arm-gnu-toolchain-14.3.rel1\\bin",
  "cortex-debug.openocdPath": "D:\\Toolchain\\OpenOCD-20250710\\bin\\openocd.exe"
}
```

**Linux：** 若工具链在 PATH 中则无需配置；否则指定绝对路径：

```json
{
  "cortex-debug.armToolchainPath": "/opt/arm-gnu-toolchain-14.3.rel1/bin",
  "cortex-debug.openocdPath": "/usr/bin/openocd"
}
```

### 7.2 launch.json 模板

工程 `.vscode/launch.json` 已预配，两种调试器任选：

| 配置名 | 调试器 | 接口 |
|--------|--------|------|
| `cmsis-dap-debug` | OpenOCD + CMSIS-DAP | SWD |
| `jlink-debug` | JLink | SWD |

```json
{
  "cwd": "${workspaceRoot}",
  "type": "cortex-debug",
  "request": "launch",
  "name": "cmsis-dap-debug",
  "servertype": "openocd",
  "executable": "build/${workspaceFolderBasename}.elf",
  "runToEntryPoint": "main",
  "configFiles": [
    "Flash/daplink.cfg"
  ],
  "liveWatch": {
    "enabled": true,
    "samplesPerSecond": 20
  }
}
```

> `${workspaceFolderBasename}` 自动取工程文件夹名，当前工程为 `MSPM0_FreeRTOS_Ctrl`。

调试功能：
- 断点、单步、变量查看
- Live Watch（实时变量监视）
- 外设寄存器查看（SVD 文件: `Flash/MSPM0G350X.svd`）
- FreeRTOS 任务列表（配合 RTOS Views 插件）

---

## 8. 日常使用流程

```
1. VSCode 打开工程文件夹（文件夹名 = MSPM0_FreeRTOS_Ctrl）
       ↓
2. 状态栏 CMake 图标 → 选择 Debug preset
       ↓
3. 等待 CMake 配置完成（输出窗口无报错）
       ↓
4. F7 编译
       ↓
5. 点击状态栏烧录按钮 或 F5 调试
```

> 首次配置成功后，后续打开工程只需等待 CMake 自动配置，即可直接编译。

