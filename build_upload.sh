#!/usr/bin/env bash

# Show help if no parameters are provided
if [ -z "$1" ] || [ "$1" == "help" ] || [ "$1" == "-h" ] || [ "$1" == "--help" ]; then
  echo "Usage: sh build_upload.sh <target>"
  echo "       sh build_upload.sh list"
  echo "Example: sh build_upload.sh RAK_4631_repeater"
  echo ""
  echo "This script wraps build.sh to compile the firmware and then perform the upload."
  exit 1
fi

# If the user requests the list, pass the command directly to build.sh
if [ "$1" == "list" ] || [ "$1" == "-l" ]; then
  sh build.sh list
  exit 0
fi

TARGET=$1

echo "=========================================="
echo "1. Compiling firmware via build.sh..."
echo "=========================================="
sh build.sh build-firmware $TARGET

# Check if build.sh compilation failed
if [ $? -ne 0 ]; then
  echo "Error: Compilation failed or environment variables are missing."
  echo "Make sure you have exported FIRMWARE_VERSION and LUSOFW_FIRMWARE_VERSION."
  exit 1
fi

echo "=========================================="
echo "2. Starting Upload to the radio..."
echo "=========================================="
pio run -e $TARGET -t upload
