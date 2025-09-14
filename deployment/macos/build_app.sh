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

echo "[3/5] Creating professional DMG with background and layout..."
cd "$DEPLOYMENT_DIR"

# Check if create-dmg is available
if ! command -v create-dmg &> /dev/null; then
    echo "Warning: create-dmg not found. Using basic hdiutil instead."
    echo "Install create-dmg with: brew install create-dmg"
    
    # Fallback to basic DMG creation
    TMP_DMG_DIR="$DEPLOYMENT_DIR/dmg_temp"
    mkdir -p "$TMP_DMG_DIR"
    cp -R "$APP_NAME.app" "$TMP_DMG_DIR/"
    ln -s /Applications "$TMP_DMG_DIR/Applications"
    hdiutil create -volname "$APP_NAME" -srcfolder "$TMP_DMG_DIR" -ov -format UDZO "$APP_NAME.dmg"
    rm -rf "$TMP_DMG_DIR"
else
    # Use create-dmg for professional DMG with background
    SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
    
    # Generate background if needed
    if [ ! -f "$SCRIPT_DIR/dmg_background.png" ]; then
        echo "Generating DMG background..."
        python3 "$SCRIPT_DIR/create_dmg_background.py"
    fi
    
    # Remove any existing DMG
    rm -f "$APP_NAME.dmg"
    
    # Create professional DMG with create-dmg
    create-dmg \
        --volname "$APP_NAME" \
        --volicon "$SCRIPT_DIR/../../resources/icons/app_icon.icns" \
        --background "$SCRIPT_DIR/dmg_background.png" \
        --window-pos 200 120 \
        --window-size 640 400 \
        --icon-size 80 \
        --icon "$APP_NAME.app" 160 200 \
        --hide-extension "$APP_NAME.app" \
        --app-drop-link 480 200 \
        --codesign "Developer ID Application" \
        "$APP_NAME.dmg" \
        "$APP_NAME.app"
fi

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