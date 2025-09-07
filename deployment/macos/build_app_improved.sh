#!/bin/bash
set -e

echo "Building MouseCross macOS Application Bundle"
echo "==========================================="

# Set variables based on script location
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$SCRIPT_DIR/../.."
BUILD_DIR="$ROOT_DIR/build"
DEPLOYMENT_DIR="$ROOT_DIR/deployment"
APP_NAME="MouseCross"

# Clean and create deployment directory
if [ -d "$DEPLOYMENT_DIR" ]; then
    rm -f "$DEPLOYMENT_DIR"/*.app 2>/dev/null || true
    rm -f "$DEPLOYMENT_DIR"/*.dmg 2>/dev/null || true
    rm -f "$DEPLOYMENT_DIR"/*.zip 2>/dev/null || true
fi

echo "[1/5] Building in Release mode..."
cd "$BUILD_DIR"
cmake --build . --config Release
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "[2/5] Copying app bundle to deployment directory..."
cp -R "$BUILD_DIR/$APP_NAME.app" "$DEPLOYMENT_DIR/$APP_NAME.app"

echo "[3/5] Creating proper DMG with Applications symlink..."
cd "$DEPLOYMENT_DIR"

# Create temporary directory for DMG contents
TMP_DMG_DIR="$DEPLOYMENT_DIR/dmg_temp"
mkdir -p "$TMP_DMG_DIR"

# Copy app to temp directory
cp -R "$APP_NAME.app" "$TMP_DMG_DIR/"

# Create Applications symlink
ln -s /Applications "$TMP_DMG_DIR/Applications"

# Create the DMG with proper structure
hdiutil create -volname "$APP_NAME" -srcfolder "$TMP_DMG_DIR" -ov -format UDZO "$APP_NAME.dmg"

# Clean up temp directory
rm -rf "$TMP_DMG_DIR"

echo "[4/5] Creating ZIP archive..."
zip -r "$APP_NAME.zip" "$APP_NAME.app"

echo "[5/5] Code signing (if certificates available)..."
# Check if code signing identity is available
SIGNING_IDENTITY=$(security find-identity -v -p codesigning | grep "Developer ID Application" | head -1 | cut -d '"' -f 2 | cut -d '"' -f 1 || echo "")

if [ ! -z "$SIGNING_IDENTITY" ]; then
    echo "Found signing identity: $SIGNING_IDENTITY"
    echo "Signing application..."
    codesign --force --verify --verbose --sign "$SIGNING_IDENTITY" "$APP_NAME.app"
    
    echo "Signing DMG..."
    codesign --force --verify --verbose --sign "$SIGNING_IDENTITY" "$APP_NAME.dmg"
    
    echo "Verifying signatures..."
    codesign --verify --verbose=2 "$APP_NAME.app"
    codesign --verify --verbose=2 "$APP_NAME.dmg"
    
    echo "Code signing completed!"
else
    echo "No code signing identity found. Skipping code signing."
    echo "To enable code signing, install a valid 'Developer ID Application' certificate."
fi

echo ""
echo "==========================================="
echo "macOS deployment completed!"
echo "==========================================="
echo ""
echo "Output directory: $DEPLOYMENT_DIR"
echo "  - $APP_NAME.app (Application bundle)"
echo "  - $APP_NAME.dmg (Disk image with Applications symlink)"
echo "  - $APP_NAME.zip (ZIP archive)"
echo ""
echo "DMG structure:"
echo "  - MouseCross.app"
echo "  - Applications -> /Applications (symlink)"
echo ""
echo "Ready for distribution!"
echo ""