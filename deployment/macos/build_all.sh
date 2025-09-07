#!/bin/bash
set -e

echo "MouseCross - Build All macOS Deployment Packages"
echo "================================================"
echo ""

# Set root directory
ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT_DIR"

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Error: Build directory not found. Please run cmake configuration first:"
    echo "  mkdir build"
    echo "  cd build"
    echo "  cmake .."
    echo "  cd .."
    exit 1
fi

# Build the project first
echo "[1/2] Building MouseCross..."
cd build
cmake --build . --config Release
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi
cd ..

echo "[2/2] Creating .app Bundle..."
cd deployment/macos
./build_app.sh
if [ $? -ne 0 ]; then
    echo ".app bundle creation failed!"
    exit 1
fi

echo "Creating App Store Package..."
./prepare_appstore.sh
if [ $? -ne 0 ]; then
    echo "App Store package creation failed!"
    exit 1
fi

cd ../..

echo ""
echo "================================================"
echo "All macOS deployment packages created!"
echo "================================================"
echo ""
echo "Output directory: $ROOT_DIR/dist/"
echo "  - MouseCross.app"
echo "  - MouseCross.pkg"
echo ""
echo "Ready for distribution!"
echo ""
echo "Next steps:"
echo "1. Test the .app bundle thoroughly"
echo "2. Sign with Developer ID certificates"
echo "3. Notarize for distribution outside App Store"
echo "4. Submit .pkg to Mac App Store via App Store Connect"
echo ""