#!/bin/bash
set -e

echo "MouseCross - Build for GitHub Release (Developer ID)"
echo "=================================================="
echo ""

# Set root directory
ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT_DIR"

# Configuration
VERSION="0.1.12"
APP_NAME="MouseCross"
DMG_NAME="MouseCross-${VERSION}-macOS"

# Developer ID certificate (for notarization and outside App Store distribution)
DEVELOPER_ID_CERT="Developer ID Application: Stefan Lohmaier (9KDRP4Q2D3)"

# Clean previous builds
echo "[1/6] Cleaning previous builds..."
rm -rf build dist

# Create fresh build directory
echo "[2/6] Setting up build directory..."
mkdir build
cd build

# Configure for regular build (not App Store)
echo "[3/6] Configuring for GitHub release..."
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="/Users/stefan/Qt/6.9.2/macos" \
      ..

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project
echo "[4/6] Building MouseCross..."
cmake --build . --config Release --parallel 8

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

# Bundle Qt libraries with macdeployqt
echo "[5/6] Bundling Qt libraries..."
/Users/stefan/Qt/6.9.2/macos/bin/macdeployqt MouseCross.app -verbose=2

if [ $? -ne 0 ]; then
    echo "macdeployqt failed!"
    exit 1
fi

cd ..

# Copy to dist and sign with Developer ID
echo "[6/6] Creating signed DMG for GitHub release..."
mkdir -p dist

# Copy app bundle
cp -R build/MouseCross.app dist/

# Sign the app with Developer ID for distribution outside App Store
echo "Signing app with Developer ID certificate..."

# Check if Developer ID certificate is available
if security find-identity -v -p codesigning | grep -q "Developer ID Application.*Stefan Lohmaier"; then
    echo "Developer ID certificate found, signing app..."

    # Sign all Qt frameworks first
    for framework in dist/MouseCross.app/Contents/Frameworks/*.framework; do
        if [[ -d "$framework" ]]; then
            echo "  Signing $(basename "$framework")..."
            codesign --force --verify --verbose \
                --sign "$DEVELOPER_ID_CERT" \
                --timestamp \
                "$framework"
        fi
    done

    # Sign all plugins
    find dist/MouseCross.app/Contents/PlugIns -name "*.dylib" -exec \
        codesign --force --verify --verbose \
        --sign "$DEVELOPER_ID_CERT" \
        --timestamp \
        {} \;

    # Sign the main executable
    codesign --force --verify --verbose \
        --sign "$DEVELOPER_ID_CERT" \
        --timestamp \
        dist/MouseCross.app/Contents/MacOS/MouseCross

    # Sign the app bundle
    codesign --force --verify --verbose \
        --sign "$DEVELOPER_ID_CERT" \
        --timestamp \
        dist/MouseCross.app

    echo "App signed successfully with Developer ID"
    SIGNED_STATUS="(Signed with Developer ID)"
else
    echo "No Developer ID certificate found, creating unsigned build"
    echo "Note: For distribution, you should sign with a Developer ID certificate"
    SIGNED_STATUS="(Unsigned)"
fi

# Create DMG
echo "Creating DMG..."
cd dist

# Create a temporary directory for DMG contents
mkdir -p dmg_contents
cp -R MouseCross.app dmg_contents/

# Create Applications symlink
ln -s /Applications dmg_contents/Applications

# Create DMG
hdiutil create -volname "$APP_NAME $VERSION" \
    -srcfolder dmg_contents \
    -ov -format UDZO \
    "$DMG_NAME.dmg"

# Clean up
rm -rf dmg_contents

cd ..

echo ""
echo "=================================================="
echo "GitHub Release build completed successfully!"
echo "=================================================="
echo ""
echo "Output directory: $ROOT_DIR/dist/"
echo "  - MouseCross.app $SIGNED_STATUS"
echo "  - $DMG_NAME.dmg"
echo ""
echo "DMG size: $(du -h "dist/$DMG_NAME.dmg" | cut -f1)"
echo ""
echo "Ready for GitHub release!"
echo ""
echo "Upload instructions:"
echo "1. Go to GitHub releases"
echo "2. Create new release for v$VERSION"
echo "3. Upload $DMG_NAME.dmg as release asset"
echo "4. Include RELEASE_NOTES.md content in release description"
echo ""