# Flash 烧录脚本

本目录包含 MSPM0G3519 的烧录脚本与配置文件。

## 文件说明

| 文件 | 用途 |
|------|------|
| `JLink_flash.bat` / `.sh` | J-Link 烧录（Windows / Linux），**烧 `.hex`** |
| `OpenOCD_flash.bat` / `.sh` | OpenOCD + CMSIS-DAP 烧录，优先 `.hex`，可回退 `.elf` |
| `daplink.cfg` | OpenOCD 配置文件（CMSIS-DAP + SWD） |
| `MSPM0G351X.svd` | 外设寄存器描述文件（调试用） |

## 构建产物

每次编译（F7）后，`build/` 下会同时生成：

| 文件 | 用途 |
|------|------|
| `<项目名>.elf` | **调试用**：F5 / Cortex-Debug，包含完整符号 |
| `<项目名>.hex` | **烧录用**：由 `CMakeLists.txt` 的 POST_BUILD 自动 `objcopy` 生成 |

## 为什么烧录必须用 `.hex`

**不要用 J-Link 直接烧 `.elf`。**

工程链接脚本中 `.data` 的 VMA（运行地址）在 SRAM `0x202000C0`，LMA（装载地址）在 Flash
`0xCF70`。J-Link 的 `loadfile` 按 VMA 装载 ELF，会：

1. 把 `.data` 初值写进 RAM（掉电丢失），**Flash `0xCF70` 从未被编程**；
2. 复位后 `Reset_Handler` 从 Flash 拷贝到 `.data` 的全是 `0xFF`，
   `FACTORYVALUE` 等全局指针变成 `0xFFFFFFFF`，程序在 `SYSCFG_DL_init()` 中 HardFault；
3. ELF 中还有一个指向 SRAM 的 LOAD 段（`.TrimTable` 引起），调试口写 SRAM 时
   不会更新奇偶校验码（`0x20300000`），可能触发 SRAMDED NMI。

`.hex` 只带绝对地址（LMA），并且生成时排除了 `.TrimTable/.vtable/.bss`，
因此既能把 `.data` 初值正确写进 Flash，也不会写 SRAM。

## 使用方式

1. **F7 编译**：生成 `.elf` + `.hex`。
2. **烧录**：运行对应的烧录脚本（VSCode 任务按钮或直接执行）。
   - 脚本使用绝对路径定位 `build/`，不依赖当前工作目录。
   - HEX 缺失时会自动用 `objcopy` 从 ELF 生成。
3. **调试**：F5 使用 `.elf`（GDB `load` 按 LMA 装载，`.data` 正确）。
   若只想连接、不烧录，使用 `jlink-attach (只调试不烧录)` 配置。

## 前置条件

- `.sh` 脚本需先授权：`chmod +x Flash/*.sh`（仅一次）。
- 工具默认从 PATH 查找：
  - `JLink.exe` / `JLinkExe`（SEGGER J-Link）
  - `openocd`（OpenOCD）
  - `arm-none-eabi-objcopy`（仅 HEX 缺失时用于自动生成）
- 如果不在 PATH，可用环境变量指定所在目录：

  | 环境变量 | 对应工具 |
  |---|---|
  | `JLINK_HOME` | 包含 `JLink.exe` / `JLinkExe` 的目录 |
  | `OPENOCD_HOME` | 包含 `openocd(.exe)` 的目录 |
  | `ARM_GCC_BIN` | 包含 `arm-none-eabi-objcopy` 的目录 |

- 目标芯片在 `JLink_flash.*` 里由 `DEVICE` 变量指定（默认 `MSPM0G3519`），换芯片时只改这一处。

## 推荐

**优先使用 JLink**。OpenOCD + MSPM0 偶发芯片锁死，解锁困难。
