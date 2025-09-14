#!/bin/bash

# MouseCross - Simple App Store Upload Script
# Interactive version that prompts for password

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Configuration
APPLE_ID="stefan@slohmaier.de"
PKG_NAME="MouseCross-0.1.0-signed.pkg"

echo "================================================"
echo "MouseCross - App Store Upload"
echo "================================================"
echo

# Check if signed package exists
if [[ ! -f "$PKG_NAME" ]]; then
    print_error "Signed package not found: $PKG_NAME"
    print_status "Run the signing script first: ./submit_appstore.sh --sign-only"
    exit 1
fi

print_success "Found signed package: $PKG_NAME"

# Get package size
SIZE=$(du -h "$PKG_NAME" | cut -f1)
print_status "Package size: $SIZE"

echo
print_status "To upload to App Store Connect, you need an App-Specific Password:"
echo "1. Go to: https://appleid.apple.com/account/manage"
echo "2. Sign in with: $APPLE_ID"
echo "3. In Security section, generate App-Specific Password"
echo "4. Name it: MouseCross Upload"
echo "5. Copy the generated password"
echo

# Prompt for password
read -p "Enter your app-specific password: " -s APP_SPECIFIC_PASSWORD
echo

if [[ -z "$APP_SPECIFIC_PASSWORD" ]]; then
    print_error "Password is required for upload"
    exit 1
fi

print_status "Starting upload to App Store Connect..."
print_warning "This may take several minutes..."

# Upload using altool
if xcrun altool --upload-app \
    --type osx \
    --file "$PKG_NAME" \
    --username "$APPLE_ID" \
    --password "$APP_SPECIFIC_PASSWORD" \
    --verbose; then

    print_success "Upload completed successfully! 🎉"
    echo
    print_status "Next steps:"
    echo "1. Check App Store Connect for processing status"
    echo "2. Go to: https://appstoreconnect.apple.com"
    echo "3. Navigate to your MouseCross app"
    echo "4. Wait for build processing (usually 10-30 minutes)"
    echo "5. Configure app metadata and submit for review"

else
    print_error "Upload failed"
    echo
    print_status "Common solutions:"
    echo "1. Check your app-specific password"
    echo "2. Verify your Apple ID has access to the team"
    echo "3. Ensure the app record exists in App Store Connect"
    echo "4. Try again - uploads can sometimes be slow"
    exit 1
fi