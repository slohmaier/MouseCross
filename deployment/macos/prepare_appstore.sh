#!/bin/bash
set -e

echo "Preparing MouseCross for App Store Submission..."
echo "=============================================="

# Set variables
BUILD_DIR="../../build"
OUTPUT_DIR="../../dist"
APP_NAME="MouseCross"

# Ensure we have a built app
if [ ! -d "$OUTPUT_DIR/$APP_NAME.app" ]; then
    echo "Error: $APP_NAME.app not found. Please run build_app.sh first."
    exit 1
fi

# Create App Store package using productbuild generator
cd "$BUILD_DIR"
cpack -G productbuild
if [ $? -ne 0 ]; then
    echo "App Store package creation failed!"
    exit 1
fi

# Move the package to dist
mv *.pkg "../$OUTPUT_DIR/"

echo ""
echo "App Store package created in: $OUTPUT_DIR/"
echo ""
echo "Next steps for App Store submission:"
echo "1. Sign the app with your Developer ID certificate"
echo "2. Upload to App Store Connect using:"
echo "   xcrun altool --upload-app --type osx --file MouseCross.pkg --username [email] --password [app-password]"
echo "3. Or use Transporter app for upload"
echo ""
echo "Required certificates and provisioning:"
echo "- Mac App Store distribution certificate"
echo "- Mac Installer distribution certificate"
echo "- App Store Connect account with proper agreements"
echo ""
echo "Done!"