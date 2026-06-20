#!/usr/bin/env bash

# Show help if no parameters are provided
if [ -z "$1" ] || [ "$1" == "help" ] || [ "$1" == "-h" ] || [ "$1" == "--help" ]; then
  echo "Usage: bash build_upload.sh <target>"
  echo "       bash build_upload.sh list"
  echo "Example: bash build_upload.sh RAK_4631_repeater"
  echo ""
  echo "This script wraps build.sh to compile the firmware and then perform the upload."
  exit 1
fi

# If the user requests the list, pass the command directly to build.sh
if [ "$1" == "list" ] || [ "$1" == "-l" ]; then
  bash build.sh list
  exit 0
fi

TARGET=$1

# get git commit sha
COMMIT_HASH=$(git rev-parse --short HEAD)

# full git tag for firmware build, e.g: main-abcdef-dirty
FIRMWARE_GIT_TAG=$(git rev-parse --abbrev-ref HEAD)-${COMMIT_HASH}$(if [ -n "$(git status --porcelain)" ]; then echo "-dirty"; fi)

export FIRMWARE_VERSION=$(grep '^[[:space:]]*#define[[:space:]]\+FIRMWARE_VERSION[[:space:]]\+"' examples/simple_repeater/MyMesh.h | \
  sed -E 's/^[[:space:]]*#define[[:space:]]+FIRMWARE_VERSION[[:space:]]+"v([0-9.]+)".*/v\1/')
export LUSOFW_FIRMWARE_VERSION=$(grep '^[[:space:]]*#define[[:space:]]\+LUSOFW_FIRMWARE_VERSION[[:space:]]\+"' examples/simple_repeater/MyMesh.h | \
  sed -E 's/^[[:space:]]*#define[[:space:]]+LUSOFW_FIRMWARE_VERSION[[:space:]]+"([^"]+)".*/\1/')

echo "=========================================="
echo "1. Compiling firmware via build.sh..."
echo "=========================================="
bash build.sh build-firmware $TARGET

# Check if build.sh compilation failed
if [ $? -ne 0 ]; then
  echo "Error: Compilation failed or environment variables are missing."
  echo "Make sure you have exported FIRMWARE_VERSION and LUSOFW_FIRMWARE_VERSION."
  exit 1
fi

echo "=========================================="
echo "2. Starting Upload to the radio..."
echo "=========================================="
FIRMWARE_VERSION_STRING="${LUSOFW_FIRMWARE_VERSION}-lusofw"
FIRMWARE_BUILD_DATE_STRING="${FIRMWARE_GIT_TAG}"
export PLATFORMIO_BUILD_FLAGS="${PLATFORMIO_BUILD_FLAGS} -DFIRMWARE_BUILD_DATE='\"${FIRMWARE_BUILD_DATE_STRING}\"' -DFIRMWARE_VERSION='\"${FIRMWARE_VERSION_STRING}\"'"
pio run -e $TARGET -t upload
