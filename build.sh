#!/bin/bash

# Win95 C Compiler Script
# Usage: ./build.sh myprogram.c [output.exe]

set -e

IMAGE_NAME="win95cc"
SRC_FILE="$1"
OUT_FILE="${2:-${SRC_FILE%.c}.exe}"

if [ -z "$SRC_FILE" ]; then
    echo "Usage: ./build.sh <source.c> [output.exe]"
    exit 1
fi

if [ ! -f "$SRC_FILE" ]; then
    echo "Error: $SRC_FILE not found"
    exit 1
fi

# Build image if it doesn't exist
if ! docker image inspect "$IMAGE_NAME" &>/dev/null; then
    echo "Building $IMAGE_NAME image (one-time)..."
    docker build -t "$IMAGE_NAME" "$(dirname "$0")"
fi

# Compile
echo "Compiling $SRC_FILE -> $OUT_FILE"
docker run --rm \
    -v "$(pwd):/src" \
    -w /src \
    "$IMAGE_NAME" \
    -fe="/src/$OUT_FILE" "/src/$SRC_FILE"

echo "Done! $(ls -lh "$OUT_FILE" | awk '{print $5}') - $OUT_FILE"
