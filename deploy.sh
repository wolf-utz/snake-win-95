#!/bin/bash

# Build and deploy to Win95 QEMU
# Usage: ./deploy.sh myprogram.c

set -e

SRC_FILE="$1"
EXE_FILE="${SRC_FILE%.c}.exe"
FLOPPY_IMG="floppy.img"
MONITOR_SOCK="/tmp/qemu-monitor.sock"

if [ -z "$SRC_FILE" ]; then
    echo "Usage: ./deploy.sh <source.c>"
    exit 1
fi

# Compile
./build.sh "$SRC_FILE" "$EXE_FILE"

# Create floppy
dd if=/dev/zero of="$FLOPPY_IMG" bs=1024 count=1440 2>/dev/null
mformat -i "$FLOPPY_IMG" -f 1440 ::
mcopy -i "$FLOPPY_IMG" "$EXE_FILE" ::

# Hot-swap into QEMU
if [ -S "$MONITOR_SOCK" ]; then
    echo "change floppy0 $(pwd)/$FLOPPY_IMG" | socat - UNIX-CONNECT:"$MONITOR_SOCK"
    echo "Deployed! Run A:\\$(echo $EXE_FILE | tr '[:lower:]' '[:upper:]') in Win95"
else
    echo "QEMU monitor not found at $MONITOR_SOCK"
    echo "Floppy created: $FLOPPY_IMG"
    echo "Start QEMU with: -fda $FLOPPY_IMG"
fi
