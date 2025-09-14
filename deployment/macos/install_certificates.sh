#!/bin/bash

# MouseCross - Manual Certificate Installation Helper

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

print_action() {
    echo -e "${YELLOW}[ACTION REQUIRED]${NC} $1"
}

echo "================================================"
echo "MouseCross - Certificate Installation Helper"
echo "================================================"
echo

print_status "Checking for certificate files..."

# Check for certificate files
if [[ -f ~/Downloads/mac_app.cer ]]; then
    print_success "Found mac_app.cer"
else
    print_error "mac_app.cer not found in Downloads"
    exit 1
fi

if [[ -f ~/Downloads/mac_installer.cer ]]; then
    print_success "Found mac_installer.cer"
else
    print_error "mac_installer.cer not found in Downloads"
    exit 1
fi

echo
print_status "Installing certificates using security command..."

# Import certificates using security command
print_status "Installing Mac App Distribution certificate..."
if security import ~/Downloads/mac_app.cer -k ~/Library/Keychains/login.keychain-db; then
    print_success "Mac App Distribution certificate imported"
else
    print_warning "Certificate import may have failed or certificate already exists"
fi

print_status "Installing Mac Installer Distribution certificate..."
if security import ~/Downloads/mac_installer.cer -k ~/Library/Keychains/login.keychain-db; then
    print_success "Mac Installer Distribution certificate imported"
else
    print_warning "Certificate import may have failed or certificate already exists"
fi

echo
print_status "Checking certificate installation..."

# Check what certificates are now available
echo "All code signing identities:"
security find-identity -v -p codesigning

echo
print_status "Looking for Mac Developer certificates..."
security find-identity -v -p codesigning | grep -i "mac.*developer" || echo "No Mac Developer certificates found"

echo
print_status "Looking for distribution certificates..."
security find-identity -v -p codesigning | grep -i "distribution" || echo "No distribution certificates found"

echo
print_status "Alternative certificate check..."
security find-identity -v -p codesigning | grep -i "stefan" || echo "No certificates with 'stefan' found"

echo
print_action "If certificates still don't appear correctly, try:"
echo "1. Open Keychain Access (Applications → Utilities → Keychain Access)"
echo "2. Select 'My Certificates' in the left sidebar"
echo "3. Look for certificates containing 'Stefan Lohmaier'"
echo "4. Check that they have associated private keys (look for small key icons)"
echo
echo "Certificate files can be manually installed by:"
echo "1. Double-clicking: ~/Downloads/mac_app.cer"
echo "2. Double-clicking: ~/Downloads/mac_installer.cer"
echo "3. Following the Keychain Access installation prompts"
echo

# Open Keychain Access to show certificates
if command -v open &> /dev/null; then
    print_status "Opening Keychain Access..."
    open -a "Keychain Access"
fi

echo
print_status "After installation, run this to verify:"
echo "  security find-identity -v -p codesigning"