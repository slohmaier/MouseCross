#!/bin/bash

# MouseCross - Certificate Linking Fix

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

echo "================================================"
echo "MouseCross - Certificate Linking Fix"
echo "================================================"
echo

CERT_DIR="$HOME/Documents/Apple Developer Account/MouseCross-Certificates"

print_status "Fixing certificate + private key linking..."

# Remove existing certificates first
print_status "Removing existing certificates to re-import with keys..."
security delete-certificate -c "3rd Party Mac Developer Application: Stefan Lohmaier (9KDRP4Q2D3)" 2>/dev/null || true
security delete-certificate -c "3rd Party Mac Developer Installer: Stefan Lohmaier (9KDRP4Q2D3)" 2>/dev/null || true

print_status "Re-importing certificates with proper key linking..."

# Method 1: Import private keys first, then certificates
print_status "Importing private keys..."
security import "$CERT_DIR/MouseCross_App_Distribution.key" -k ~/Library/Keychains/login.keychain-db -A
security import "$CERT_DIR/MouseCross_Installer_Distribution.key" -k ~/Library/Keychains/login.keychain-db -A

# Now import certificates
print_status "Importing certificates..."
security import ~/Downloads/mac_app.cer -k ~/Library/Keychains/login.keychain-db
security import ~/Downloads/mac_installer.cer -k ~/Library/Keychains/login.keychain-db

print_status "Setting trust settings for certificates..."
# Make certificates trusted for code signing
security set-key-partition-list -S apple-tool:,apple: -s -k "" ~/Library/Keychains/login.keychain-db 2>/dev/null || true

echo
print_status "Checking code signing identities after fix..."
security find-identity -v -p codesigning

echo
print_status "Specific check for Mac Developer certificates..."
security find-identity -v -p codesigning | grep -E "(3rd Party|Mac.*Developer)" || echo "None found with that pattern"

# Alternative check - look for any certificates with Stefan
security find-identity -v -p codesigning | grep -i stefan || echo "None found with Stefan"

echo
print_status "Final diagnostic - checking certificate details..."

# Check if certificates have associated private keys
for cert_name in "3rd Party Mac Developer Application: Stefan Lohmaier (9KDRP4Q2D3)" "3rd Party Mac Developer Installer: Stefan Lohmaier (9KDRP4Q2D3)"; do
    if security find-identity -v -p codesigning | grep -q "$cert_name"; then
        print_success "Certificate found in code signing identities: $cert_name"
    else
        print_warning "Certificate NOT found in code signing identities: $cert_name"
    fi
done

echo
print_status "If certificates still don't work, try manual Keychain Access method:"
echo "1. Open Keychain Access"
echo "2. File → Import Items..."
echo "3. Import the .key files first:"
echo "   - $CERT_DIR/MouseCross_App_Distribution.key"
echo "   - $CERT_DIR/MouseCross_Installer_Distribution.key"
echo "4. Then import the .cer files:"
echo "   - ~/Downloads/mac_app.cer"
echo "   - ~/Downloads/mac_installer.cer"
echo "5. The certificates should automatically link to their private keys"

# Open Keychain Access for manual inspection
if command -v open &> /dev/null; then
    print_status "Opening Keychain Access for manual verification..."
    open -a "Keychain Access"
fi