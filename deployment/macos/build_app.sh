#!/bin/bash
set -e

echo "Building MouseCross macOS .app Bundle..."
echo "====================================="

# Set variables based on script location
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$SCRIPT_DIR/../.."
BUILD_DIR="$ROOT_DIR/build"
OUTPUT_DIR="$ROOT_DIR/dist"
APP_NAME="MouseCross"

# Clean and create directories
if [ -d "$OUTPUT_DIR" ]; then
    rm -rf "$OUTPUT_DIR"
fi
mkdir -p "$OUTPUT_DIR"

# Build in Release mode
cd "$BUILD_DIR"
cmake --build . --config Release
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi
cd ..

# Create .app bundle using CPack
cd "$BUILD_DIR"
cpack -G Bundle
if [ $? -ne 0 ]; then
    echo "Bundle creation failed!"
    exit 1
fi

# Move the .app to dist
mv *.app "../$OUTPUT_DIR/"

echo ""
echo "macOS .app bundle created in: $OUTPUT_DIR/"
echo "Done!"