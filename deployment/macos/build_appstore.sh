#!/bin/bash
set -e

echo "MouseCross - Build for Mac App Store (Static Qt)"
echo "================================================"
echo ""

# Set root directory
ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT_DIR"

# Clean previous builds
echo "[1/5] Cleaning previous builds..."
rm -rf build dist

# Create fresh build directory
echo "[2/5] Setting up build directory..."
mkdir build
cd build

# Configure for App Store static build
echo "[3/5] Configuring for App Store with static Qt..."
cmake -DAPPSTORE_BUILD=ON \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="/Users/stefan/Qt/6.9.0/macos" \
      ..

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project
echo "[4/5] Building MouseCross..."
cmake --build . --config Release --parallel 8

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

cd ..

# Bundle Qt libraries
echo "[5/7] Bundling Qt libraries with macdeployqt..."
cd build
/Users/stefan/Qt/6.9.0/macos/bin/macdeployqt MouseCross.app -verbose=2

if [ $? -ne 0 ]; then
    echo "macdeployqt failed!"
    exit 1
fi

# Copy to dist
echo "[6/7] Copying app to dist directory..."
cd ..
mkdir -p dist
cp -R build/MouseCross.app dist/

# Create App Store package
echo "[7/7] Creating App Store package..."
cd build
productbuild --component MouseCross.app /Applications MouseCross-0.1.0-appstore.pkg

if [ $? -ne 0 ]; then
    echo "Package creation failed!"
    exit 1
fi

mv MouseCross-0.1.0-appstore.pkg ../dist/

cd ..

echo ""
echo "================================================"
echo "App Store build completed successfully!"
echo "================================================"
echo ""
echo "Output directory: $ROOT_DIR/dist/"
echo "  - MouseCross.app (fully bundled for App Store)"
echo "  - MouseCross-0.1.0-appstore.pkg"
echo ""
echo "Verification commands:"
echo "  otool -L dist/MouseCross.app/Contents/MacOS/MouseCross"
echo "  file dist/MouseCross.app/Contents/MacOS/MouseCross"
echo ""
echo "Ready for App Store submission!"
echo ""
echo "Next steps:"
echo "1. Sign with Mac App Store distribution certificate"
echo "2. Upload to App Store Connect"
echo ""