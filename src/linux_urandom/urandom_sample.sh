#!/bin/bash
# Author: Carlton Shepherd (https://cs.gl, 2025)

# Default values
SIZE_GB=0.1
BYTES_IN_GB=$((1024 * 1024 * 1024))

# Get system info and date
OS_VERSION=$(lsb_release -si | tr '[:upper:]' '[:lower:]')-$(lsb_release -sr)
DATE=$(date +%Y-%m-%d)

# Set default output path or use provided path
if [ $# -ge 1 ]; then
    OUTPUT_DIR=$(dirname "$1")
    OUTPUT="${1}"
else
    OUTPUT_DIR="data"
    OUTPUT="${OUTPUT_DIR}/urandom_${OS_VERSION}_${DATE}.bin"
fi

# Ensure output directory exists
mkdir -p "$OUTPUT_DIR"

# Calculate total bytes
TOTAL_BYTES=$(bc <<< "$SIZE_GB * $BYTES_IN_GB")
TOTAL_BYTES=${TOTAL_BYTES%.*}  # Remove decimal part

# Use dd with status=progress for real-time progress
echo "Generating ${SIZE_GB} GB of random data using /dev/urandom..."
echo "Writing output to: $OUTPUT"

dd if=/dev/urandom of="$OUTPUT" bs=1M count=$((TOTAL_BYTES/1024/1024)) status=progress

if [ $? -eq 0 ]; then
    echo -e "\nRandom data successfully written to '$OUTPUT'"
else
    echo -e "\nError generating random data"
    exit 1
fi

# Optional: log metadata
METADATA_DIR="${OUTPUT_DIR}/metadata"
METADATA_FILE="${METADATA_DIR}/urandom_${OS_VERSION}_${DATE}.txt"
mkdir -p "$METADATA_DIR"
echo "Source: /dev/urandom" > "$METADATA_FILE"
echo "OS: $(lsb_release -sd)" >> "$METADATA_FILE"
echo "Kernel: $(uname -r)" >> "$METADATA_FILE"
echo "Date: $DATE" >> "$METADATA_FILE"
echo "Size: $((TOTAL_BYTES)) bytes" >> "$METADATA_FILE"
echo "Metadata saved to: $METADATA_FILE"
