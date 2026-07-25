# MSPM0_FreeRTOS_Ctrl

> TI MSPM0G3507 多层架构主控工程 — 基于 FreeRTOS，面向竞赛与机器人控制场景
>
> MCU：MSPM0G3507 | SDK：2.10.00.04 | SysConfig：1.27.0 | 编译器：ARM GCC (arm-none-eabi)

---

## 目录

- [MSPM0\_FreeRTOS\_Ctrl](#mspm0_freertos_ctrl)
  - [目录](#目录)
  - [0. 分层架构](#0-分层架构)
  - [1. 工程结构](#1-工程结构)
  - [2. 快速上手](#2-快速上手)
    - [2.1 配置 SDK 路径](#21-配置-sdk-路径)
    - [2.2 配置 CMake Presets](#22-配置-cmake-presets)
    - [2.3 首次打开工程](#23-首次打开工程)
    - [2.4 前提条件 Checklist](#24-前提条件-checklist)
  - [3. 代码架构说明](#3-代码架构说明)
  - [4. 编译 / 烧录 / 调试](#4-编译--烧录--调试)
    - [4.1 编译](#41-编译)
    - [4.2 烧录](#42-烧录)
    - [4.3 调试](#43-调试)
  - [5. SysConfig 引脚配置](#5-sysconfig-引脚配置)
    - [5.1 打开方式](#51-打开方式)
    - [5.2 当前引脚分配](#52-当前引脚分配)
    - [5.3 修改后](#53-修改后)
  - [6. 开发文档](#6-开发文档)
  - [2. 快速上手](#2-快速上手-1)
    - [2.1 配置 TI SDK 路径](#21-配置-ti-sdk-路径)
    - [2.2 配置 CMake Presets](#22-配置-cmake-presets-1)
    - [2.3 首次打开工程](#23-首次打开工程-1)
    - [2.4 前提条件 Checklist 等等](#24-前提条件-checklist-等等)
  - [3. 代码 驱动手册](#3-代码-驱动手册)
  - [4. 编译 / 烧录 / 调试](#4-编译--烧录--调试-1)
    - [4.1 编译](#41-编译-1)
    - [4.2 烧录](#42-烧录-1)
    - [4.3 调试](#43-调试-1)
  - [5. SysConfig 引脚配置](#5-sysconfig-引脚配置-1)
    - [5.1 打开方式](#51-打开方式-1)
    - [5.2 当前引脚分配](#52-当前引脚分配-1)
    - [5.3 修改后](#53-修改后-1)

---

## 0. 分层架构

![分层规划图](docs/分层规划图.png)

```
┌─────────────────────────────────────────────────┐
│                  应用层  App                     │
│            main.cpp / 各 FreeRTOS 任务入口        │
├──────────┬──────────┬──────────┬────────────────┤
│  模块层   │  算法层   │  协议层   │    服务层       │
│  Module  │Algorithm │ Protocol │   Service      │
│ 底盘控制  │ PID/滤波  │ CAN协议  │ FreeRTOSConfig │
│ 预留      │ 轨迹规划  │ 帧编解码  │ 内存管理        │
├──────────┴──────────┴──────────┴────────────────┤
│                 设备层  Device                   │
│     外设驱动代理: 电机/陀螺仪/OLED/PCA9555/...     │
├─────────────────────────────────────────────────┤
│              板级支持层  Bsp                      │
│    UART / PWM / QEI / I2C / SPI / CAN / GPIO     │
├─────────────────────────────────────────────────┤
│         硬件驱动层  TI DriverLib + SysConfig       │
│       MSPM0G3507 (Cortex-M0+) + 外设引脚          │
└─────────────────────────────────────────────────┘
```

> **依赖方向**: 上层依赖下层，下层不感知上层。App → Device/Bsp → DriverLib。
> **Service 层贯穿各层**: 为所有层提供 FreeRTOS 任务调度、内存管理、系统配置等基础设施。

---

## 1. 工程结构

```txt
root
│  .clang-format             代码格式化配置
│  .gitignore                Git 忽略规则
│  CMakeLists.txt            顶层 CMake（自动发现 User/ 下所有 .c/.cpp）
│  CMakePresets.json         编译预设（Debug / Release）
│  mspm0g350x_base.cmake     MSPM0 基础配置（编译器 + SDK 路径 + 链接脚本）
│  README.md                 本文件
│
├─.vscode/                   VSCode 配置（launch.json, tasks.json, settings.json）
├─build/                     构建输出（含 compile_commands.json → clangd 智能提示）
├─docs/                      开发文档
│   ├─ 分层规划图.png          分层架构图
│   ├─ 关于配置CMake环境.md    工具链安装 + CMake + clangd 完整配置指南
│   └─ 编码风格.md             命名规范、缩进、注释等编码约定
├─Flash/                     烧录脚本（OpenOCD / JLink）+ Ozone 配置
├─FreeRTOS/                  FreeRTOS 内核源码（V11.1.0）
├─SysConfig/                 TI SysConfig 生成文件（自动生成，勿手动改）
├─example/                   参考例程
└─User/
    ├─App/                   应用层 — main.cpp, 各任务入口
    ├─Bsp/                   板级支持层 — UART / PWM / QEI / I2C / SPI / CAN
    ├─Device/                设备驱动层 — 电机/陀螺仪/OLED/PCA9555 等代理
    ├─Module/                模块层 — 底盘控制等业务模块
    ├─Algorithm/             算法层 — PID / 滤波 / 轨迹规划
    ├─Protocol/              协议层 — CAN 帧编解码
    └─Service/               服务层 — FreeRTOSConfig.h / 内存管理
```

**User/ 下所有 `.c/.cpp` 会被 CMake 自动发现**，新增文件无需手动修改 CMakeLists.txt。

---

## 2. 快速上手

> 前提：已安装 ARM GCC 交叉编译器 + CMake + Ninja + VSCode（含 CMake Tools 插件）。
> 完整环境搭建指南见 **[docs/关于配置CMake环境.md](docs/关于配置CMake环境.md)**。

### 2.1 配置 SDK 路径

复制docs下的 `mspm0g350x_base.cmake.txt` → 重命名为 `mspm0g350x_base.cmake`，按本机路径修改：

```cmake
set(MSPM0_SDK_PATH  /home/rh/ti/mspm0_sdk_2_10_00_04)   # ← 改成你的路径
```

### 2.2 配置 CMake Presets

复制docs下的 `CMakePresets.json.txt` → 重命名为 `CMakePresets.json`，按你的编译器路径修改 `CMAKE_C_COMPILER` / `CMAKE_CXX_COMPILER`。

### 2.3 首次打开工程

1. **删除** 原有 `build/` 目录（如果有）
2. VSCode 状态栏 CMake 图标 → 选择 **Debug** preset → 配置
3. 等待 CMake 配置完成 → `F7` 编译
4. `project()` 名称必须与根文件夹名一致（当前为 `MSPM0_FreeRTOS_Ctrl`）

### 2.4 前提条件 Checklist

| 项目 | 版本要求 | 验证命令 |
|------|---------|---------|
| ARM GCC | 14.x+ | `arm-none-eabi-gcc --version` |
| CMake | ≥ 3.27 | `cmake --version` |
| Ninja | 任意 | `ninja --version` |
| TI MSPM0 SDK | 2.10.00.04 | 检查 `mspm0g350x_base.cmake` 中路径 |
| VSCode 插件 | CMake Tools, clangd, Cortex-Debug | — |
| OpenOCD | 0.12+ (烧录/调试) | `openocd --version` |
| JLink | 任意版本 (推荐烧录) | `JLinkExe -Version` |

---

## 3. 代码架构说明

所有外设**硬件初始化**由 `SysConfig` 生成的 `SYSCFG_DL_init()` → `SYSCFG_DL_xxx_init()` 完成。
BSP 类只封装运行时操作，**不重复配置硬件寄存器**。

| 概念 | 配置文件 | 说明 |
|------|---------|------|
| BSP 全局实例声明 | `User/Bsp/bsp_cfg.hpp` | 各外设对象声明 |
| BSP 全局实例初始化 | `User/Bsp/bsp_cfg.cpp` 的 `bsp_init()` | 构造时传引脚/参数 |
| Device 层注册 | Device 对应 cfg 文件 | 设备驱动初始化与绑定 |

> 编码规范参见 **[docs/编码风格.md](docs/编码风格.md)**。

---

## 4. 编译 / 烧录 / 调试

### 4.1 编译

VSCode 状态栏按钮操作，或使用 Tasks：

| 操作 | 方式 |
|------|------|
| 编译 | `F7` 或底部 Build 按钮 |
| 清理重建 | `Ctrl+Shift+P` → `CMake: Clean Rebuild` |

CMake 自动发现 `User/` 下所有 `.c/.cpp`，无需手动维护文件列表。

### 4.2 烧录

> **推荐 JLink**（OpenOCD + MSPM0 偶发芯片锁死，解锁困难）。
> Linux `.sh` 脚本需先 `chmod +x Flash/*.sh`（仅一次）。

| 工具 | Linux | Windows | 可靠性 |
|------|-------|---------|:-----:|
| JLink | `Flash/JLink_flash.sh` | `Flash/JLink_flash.bat` | ⭐ 推荐 |
| OpenOCD | `Flash/OpenOCD_flash.sh` | `Flash/OpenOCD_flash.bat` | ⚠️ 可能锁芯片 |

也可通过 VSCode Task：`Terminal` → `Run Task` → 选择 `JLink_flash_linux` 等。

### 4.3 调试

`F5` 启动调试，已预配 OpenOCD / JLink 两种调试器（`.vscode/launch.json`）。

---

## 5. SysConfig 引脚配置

### 5.1 打开方式

用 TI SysConfig 工具打开 `SysConfig/MSPM0_FreeRTOS_Ctrl.syscfg`。

### 5.2 当前引脚分配

| 外设 | 实例 | 引脚 | 备注 |
|------|------|------|------|
| UART1 | UART1 | PB7(RX) PB6(TX) | 陀螺仪 |
| UART2 | UART2 | PB18(RX) PB17(TX) | 调试输出 (115200, 8N1) |
| UART3 | UART3 | PA13(RX) PA14(TX) | 通用 |
| SPI0 | SPI0 | PA9(PICO) PA10(POCI) PA11(SCLK) PA8(CS0) | Mode 0, 8MHz |
| I2C0 | I2C0 | PA0(SDA) PA1(SCL) | OLED (0x3C) |
| I2C1 | I2C1 | PA30(SDA) PA29(SCL) | PCA9555 (巡线) |
| MCAN0 | CANFD0 | PA26(TX) PA27(RX) | 1Mbps, 标准帧 |
| PWM_A0 | TIMA0 | PB8(CH0) PB9(CH1) | 80MHz, period=10000 |
| PWM_G0 | TIMG0 | PB10(CH0) PB11(CH1) | 40MHz, period=10000 |
| QEI | TIMG8 | — | 编码器 |
| LED | GPIO | PA17, PA15 | 调试 LED |
| KEY | GPIO | PB13 ~ PB16 | 按键 |
| IR | GPIO | PB21~PB24, PA23~PA25, PB25 | 红外巡线 |
| IO | GPIO | PA7, PB2 ~ PB4 | 扩展 IO |
| BUZZ | GPIO | PA16 | 蜂鸣器 |
| ENP | GPIO | PA15 | 使能 |
| PWMEN | GPIO | PB12 | PWM 使能 |

### 5.3 修改后

修改 SysConfig → `Ctrl+S` 保存 → 自动重新生成 `ti_msp_dl_config.c/h` → 重新编译。

> ⚠️ `ti_msp_dl_config.c/h` 是自动生成的，**禁止手动编辑**。

---

## 6. 开发文档

| 文档 | 说明 |
|------|------|
| [docs/关于配置CMake环境.md](docs/关于配置CMake环境.md) | 工具链安装 + CMake + clangd + 烧录调试 完整指南 |
| [docs/编码风格.md](docs/编码风格.md) | C/C++ 命名规范、缩进、注释、文件组织约定 |
| [Flash/README.md](Flash/README.md) | 烧录脚本使用说明 |
| [example/](example/) | 参考例程（已验证的协议实现） |

---

## 2. 快速上手

> 前提：已安装 ARM GCC 交叉编译器 + CMake + VSCode（含 CMake Tools 插件）。

### 2.1 配置 TI SDK 路径

复制 `mspm0g350x_base.cmake.txt`，按你本机的 SDK 路径修改其中一行，然后重命名为 `mspm0g350x_base.cmake`：

```cmake
# 这一行改成你自己的 SDK 安装路径
set(MSPM0_SDK_PATH  /home/rh/ti/mspm0_sdk_2_10_00_04)
```

### 2.2 配置 CMake Presets

复制 `CMakePresets.json.txt`，按你的编译器路径修改，然后重命名为 `CMakePresets.json`。

### 2.3 首次打开工程

1. **删除** 原有的 `build/` 目录（如果有）
2. VSCode 状态栏点击 CMake 图标 → 点击配置为 **arm-none-eabi GCC**
3. 等待 CMake 配置完成，`F7` 编译
4. CMakeLists.txt 中的 `project(XXX C CXX ASM)` 名称必须和根文件夹名一致。

### 2.4 前提条件 Checklist 等等

| 项目 | 说明 |
|------|------|
| ARM GCC | `arm-none-eabi-gcc --version` 能输出版本 |
| CMake | ≥ 3.27 |
| Ninga | 编译用 |
| TI MSPM0 SDK | 路径需正确 |
| VSCode 插件 | CMake Tools、C/C++、Cortex-Debug 等等|
| OpenOCD 驱动 | 烧录 / 调试需要|
| JLink 驱动 | 烧录 / 调试需要 |

---

## 3. 代码 驱动手册

所有外设的**硬件初始化**（时钟/引脚/模式）由 `SYSCFG_DL_init()` → `SYSCFG_DL_xxx_init()` 完成（SysConfig 自动生成代码）。BSP 类只封装运行时操作，**不重复配置硬件寄存器**，其实也可以手动配置。

全局实例声明在 `User/Bsp/bsp_cfg.hpp`，初始化在 `User/Bsp/bsp_cfg.cpp` 的 `bsp_init()` 中。

同理，设备层也是在CFG中初始化，各个内容的配置改那里就可以了

---

## 4. 编译 / 烧录 / 调试

### 4.1 编译

下面有按钮可以按

| 操作 | 方式 |
|------|------|
| 编译 | `F7` 或点击 VSCode 底部 Build 按钮 |
| 清理重建 | `Ctrl+Shift+P` → `CMake: Clean Rebuild` |

CMake 自动发现 `User/` 下所有 `.c/.cpp`，无需手动维护文件列表。

### 4.2 烧录

下面有按钮可以按

| 工具 | Linux | Windows | 说明 |
|------|-------|---------|------|
| JLink | `Flash/JLink_flash.sh` | `Flash/JLink_flash.bat` | 推荐，稳定 |
| OpenOCD | `Flash/OpenOCD_flash.sh` | `Flash/OpenOCD_flash.bat` | 可能锁芯片 |

> **推荐用 JLink**。OpenOCD + MSPM0 偶发芯片锁死问题。调试未结束点烧录大概率会锁芯片，解锁不容易
>
> Linux `.sh` 脚本需要先 `chmod +x Flash/*.sh`（只需一次）。
>
> 需提前配置 JLink 环境变量，确保 `JLinkExe` 在 PATH 中。

也可以使用 VSCode Tasks：`Terminal` → `Run Task` → 选择 `JLink_flash_linux` 或 `OpenOCD_flash_linux`。

### 4.3 调试

下面按钮也可以按

`F5` 启动调试，已配置好 OpenOCD 和 JLink 两种调试器模板。

| 文件 | 调试器 |
|------|--------|
| `.vscode/launch.json` | Cortex-Debug + OpenOCD / JLink |

---

## 5. SysConfig 引脚配置

### 5.1 打开方式

把 `SysConfig/MSPM0_FreeRTOS_Ctrl.syscfg` 拖到外面的SYSCFG的文件图标上。或者直接通过SYSCFG软件打开这个配置界面

### 5.2 当前引脚分配

| 外设 | 实例 | 引脚 | 备注 |
|------|------|------|------|
| UART1 | UART1 | PB7(RX) PB6(TX) | 陀螺仪 |
| UART2 | UART2 | PB18(RX) PB17(TX) | 调试输出 (115200, 8N1) |
| UART3 | UART3 | PA13(RX) PA14(TX) | 通用 |
| SPI0 | SPI0 | PA9(PICO) PA10(POCI) PA11(SCLK) PA8(CS0) | Mode 0, 8MHz |
| I2C0 | I2C0 | PA0(SDA) PA1(SCL) | OLED (0x3C) |
| I2C1 | I2C1 | PA30(SDA) PA29(SCL) | 巡线 PCA9555 |
| MCAN0 | CANFD0 | PA26(TX) PA27(RX) | 1Mbps, 标准帧 |
| PWM_A0 | TIMA0 | PB8(CH0) PB9(CH1) | 80MHz, period=10000 |
| PWM_G0 | TIMG0 | PB10(CH0) PB11(CH1) | 40MHz, period=10000 |
| QEI | TIMG8 | — | 编码器 |
| LED | GPIO | PA17, PA15 | 调试 LED |
| KEY | GPIO | PB13 ~ PB16 | 按键 |
| IR | GPIO | PB21 ~ PB24, PA23 ~ PA25, PB25 | 红外巡线 |
| IO | GPIO | PA7, PB2 ~ PB4 | 扩展 IO |
| BUZZ | GPIO | PA16 | 蜂鸣器 |
| ENP | GPIO | PA15 | 使能 |
| PWMEN | GPIO | PB12 | PWM 使能 |

### 5.3 修改后

修改 SysConfig 文件后保存（Ctrl+S），SysConfig 会自动重新生成 `SysConfig/ti_msp_dl_config.c` 和 `SysConfig/ti_msp_dl_config.h`。然后重新编译即可。

> ⚠️ `ti_msp_dl_config.c/h` 是自动生成的，**不要手动编辑**，不要引其他的库。
