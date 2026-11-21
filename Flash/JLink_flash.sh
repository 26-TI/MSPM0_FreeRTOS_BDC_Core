#!/bin/bash
set -u

# ============================================================
#  J-Link flash script (Linux)
#
#  Flash <project>.hex, NOT <project>.elf.  See README.md.
#
#  Tools are resolved from PATH by default. Optional overrides:
#    export JLINK_HOME=/opt/SEGGER/JLink
#    export ARM_GCC_BIN=/opt/arm-gnu-toolchain/bin
# ============================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
PROJECT_NAME="$(basename "$PROJECT_DIR")"

BUILD_DIR="$PROJECT_DIR/build"
HEX_FILE="$BUILD_DIR/$PROJECT_NAME.hex"
ELF_FILE="$BUILD_DIR/$PROJECT_NAME.elf"

# Target device. Change here if the project moves to another MCU.
DEVICE="MSPM0G3519"

# objcopy: PATH by default, ARM_GCC_BIN override.
OBJCOPY="arm-none-eabi-objcopy"
if [ -n "${ARM_GCC_BIN:-}" ] && [ -x "$ARM_GCC_BIN/arm-none-eabi-objcopy" ]; then
    OBJCOPY="$ARM_GCC_BIN/arm-none-eabi-objcopy"
fi

# ---------- Ensure the HEX exists ----------
if [ ! -f "$HEX_FILE" ]; then
    if [ ! -f "$ELF_FILE" ]; then
        echo "[ERROR] Neither HEX nor ELF found in $BUILD_DIR. Build first."
        exit 1
    fi
    if ! command -v "$OBJCOPY" >/dev/null 2>&1; then
        echo "[ERROR] arm-none-eabi-objcopy not found in PATH."
        echo "        Add the toolchain to PATH or set ARM_GCC_BIN."
        exit 1
    fi
    echo "[INFO] HEX not found. Generating from ELF..."
    "$OBJCOPY" -O ihex \
        --remove-section=.TrimTable \
        --remove-section=.vtable \
        --remove-section=.bss \
        "$ELF_FILE" "$HEX_FILE"
fi

echo "Project : $PROJECT_NAME"
echo "HEX     : $HEX_FILE"

# ---------- J-Link ----------
if [ -n "${JLINK_HOME:-}" ] && [ -x "$JLINK_HOME/JLinkExe" ]; then
    JLINK_EXE="$JLINK_HOME/JLinkExe"
elif command -v JLinkExe >/dev/null 2>&1; then
    JLINK_EXE=JLinkExe
elif command -v JLink >/dev/null 2>&1; then
    JLINK_EXE=JLink
else
    echo "[ERROR] JLinkExe/JLink not found in PATH."
    echo "        Install SEGGER J-Link software or set JLINK_HOME."
    exit 1
fi

TEMP_SCRIPT="$BUILD_DIR/jlink_flash.jlink"
trap 'rm -f "$TEMP_SCRIPT"' EXIT

cat > "$TEMP_SCRIPT" <<EOF
device $DEVICE
if SWD
speed 4000
halt
loadfile $HEX_FILE
r
go
exit
EOF

"$JLINK_EXE" -ExitOnError 1 -CommanderScript "$TEMP_SCRIPT"
RC=$?

if [ "$RC" -ne 0 ]; then
    echo "[ERROR] J-Link failed with exit code $RC"
    exit "$RC"
fi

echo "[OK] Flash completed."
