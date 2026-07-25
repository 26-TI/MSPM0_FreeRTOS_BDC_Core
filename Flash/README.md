# Flash 烧录脚本

本目录包含 MSPM0G3507 的烧录脚本与配置文件。

## 文件说明

| 文件 | 用途 |
|------|------|
| `JLink_flash.sh` | JLink 烧录脚本（Linux） |
| `JLink_flash.bat` | JLink 烧录脚本（Windows） |
| `OpenOCD_flash.sh` | OpenOCD 烧录脚本（Linux） |
| `OpenOCD_flash.bat` | OpenOCD 烧录脚本（Windows） |
| `daplink.cfg` | OpenOCD 配置文件（CMSIS-DAP + SWD） |
| `MSPM0G350X.svd` | 外设寄存器描述文件（调试用） |

## 前置条件

- `.sh` 脚本需先授权：`chmod +x Flash/*.sh`（仅一次）
- 确保 `JLinkExe` 或 `openocd` 在系统 PATH 中

## 推荐

**优先使用 JLink**。OpenOCD + MSPM0 偶发芯片锁死，解锁困难。
