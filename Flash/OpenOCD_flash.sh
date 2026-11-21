#!/bin/bash
set -u

# ============================================================
#  OpenOCD flash script (Linux, CMSIS-DAP)
#
#  Prefer <project>.hex (LMA-based, no SRAM sections).
#  OpenOCD handles ELF LMA correctly, so ELF is a safe fallback.
#
#  OpenOCD is resolved from PATH by default. Optional override:
#    export OPENOCD_HOME=/opt/openocd/bin
# ============================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
PROJECT_NAME="$(basename "$PROJECT_DIR")"

BUILD_DIR="$PROJECT_DIR/build"
HEX_FILE="$BUILD_DIR/$PROJECT_NAME.hex"
ELF_FILE="$BUILD_DIR/$PROJECT_NAME.elf"

if [ -f "$HEX_FILE" ]; then
    TARGET_FILE="$HEX_FILE"
elif [ -f "$ELF_FILE" ]; then
    echo "[WARN] HEX not found. Using ELF instead: $ELF_FILE"
    TARGET_FILE="$ELF_FILE"
else
    echo "[ERROR] Neither HEX nor ELF found in $BUILD_DIR. Build first."
    exit 1
fi

# ---------- OpenOCD ----------
OPENOCD="openocd"
if [ -n "${OPENOCD_HOME:-}" ] && [ -x "$OPENOCD_HOME/openocd" ]; then
    OPENOCD="$OPENOCD_HOME/openocd"
fi
if ! command -v "$OPENOCD" >/dev/null 2>&1; then
    echo "[ERROR] openocd not found in PATH."
    echo "        Add OpenOCD to PATH or set OPENOCD_HOME."
    exit 1
fi

echo "Project : $PROJECT_NAME"
echo "Target  : $TARGET_FILE"

"$OPENOCD" -f "$SCRIPT_DIR/daplink.cfg" \
    -c "program \"$TARGET_FILE\" verify reset exit"
RC=$?

if [ "$RC" -ne 0 ]; then
    echo "[ERROR] OpenOCD failed with exit code $RC"
    exit "$RC"
fi

echo "[OK] Flash completed."
