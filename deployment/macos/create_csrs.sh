#!/bin/bash

# MouseCross - Create Certificate Signing Requests (CSRs)
# Creates CSR files needed for Mac App Store certificates

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
CERT_DIR="$HOME/Documents/Apple Developer Account/MouseCross-Certificates"
COMMON_NAME="Stefan Lohmaier"
EMAIL="stefan@slohmaier.de"
ORGANIZATION="Stefan Lohmaier"
COUNTRY="DE"

echo "================================================"
echo "MouseCross - Certificate Signing Request Creator"
echo "================================================"
echo

print_status "Creating certificate directory: $CERT_DIR"
mkdir -p "$CERT_DIR"

# Function to create a CSR
create_csr() {
    local cert_type="$1"
    local key_file="$2"
    local csr_file="$3"
    local cn="$4"

    print_status "Creating $cert_type CSR..."

    # Generate private key
    openssl genrsa -out "$key_file" 2048

    # Create CSR configuration
    cat > temp_config.conf << EOF
[req]
distinguished_name = req_distinguished_name
req_extensions = v3_req
prompt = no

[req_distinguished_name]
C = $COUNTRY
O = $ORGANIZATION
CN = $cn
emailAddress = $EMAIL

[v3_req]
keyUsage = keyEncipherment, dataEncipherment
extendedKeyUsage = codeSigning
EOF

    # Generate CSR
    openssl req -new -key "$key_file" -out "$csr_file" -config temp_config.conf

    # Clean up temp config
    rm temp_config.conf

    if [[ -f "$csr_file" ]]; then
        print_success "$cert_type CSR created: $csr_file"
    else
        print_error "Failed to create $cert_type CSR"
        return 1
    fi
}

# Create Mac App Distribution CSR
print_status "Creating Mac App Distribution certificate request..."
create_csr "Mac App Distribution" \
    "$CERT_DIR/MouseCross_App_Distribution.key" \
    "$CERT_DIR/MouseCross_App_Distribution.csr" \
    "Mac Developer: $COMMON_NAME"

# Create Mac Installer Distribution CSR
print_status "Creating Mac Installer Distribution certificate request..."
create_csr "Mac Installer Distribution" \
    "$CERT_DIR/MouseCross_Installer_Distribution.key" \
    "$CERT_DIR/MouseCross_Installer_Distribution.csr" \
    "Mac Developer: $COMMON_NAME"

# Create README with instructions
print_status "Creating certificate instructions..."
cat > "$CERT_DIR/README.txt" << EOF
MouseCross - Mac App Store Certificate Setup
===========================================

Created: $(date)

Files in this directory:
1. MouseCross_App_Distribution.csr - Certificate Signing Request for Mac App Distribution
2. MouseCross_App_Distribution.key - Private key for Mac App Distribution (KEEP SECURE!)
3. MouseCross_Installer_Distribution.csr - Certificate Signing Request for Mac Installer Distribution
4. MouseCross_Installer_Distribution.key - Private key for Mac Installer Distribution (KEEP SECURE!)

NEXT STEPS:
==========

1. Go to https://developer.apple.com/account/resources/certificates
2. Click "+" to create new certificate

For App Distribution Certificate:
- Select "Mac App Distribution"
- Upload: MouseCross_App_Distribution.csr
- Download the resulting certificate (.cer file)
- Double-click to install in Keychain Access

For Installer Distribution Certificate:
- Select "Mac Installer Distribution"
- Upload: MouseCross_Installer_Distribution.csr
- Download the resulting certificate (.cer file)
- Double-click to install in Keychain Access

IMPORTANT:
- Keep the .key files secure and backed up
- The .csr files are only needed for certificate creation
- After installing certificates, you can run the MouseCross submission script

Certificate Names (for script configuration):
- Mac App Distribution: "Mac Developer: Stefan Lohmaier"
- Mac Installer Distribution: "Mac Developer: Stefan Lohmaier"
- Or check exact names in Keychain Access after installation

SECURITY NOTE:
The private keys (.key files) are crucial for code signing. Back them up securely
and never share them. If lost, you'll need to create new certificates.
EOF

# Set appropriate permissions on private keys
chmod 600 "$CERT_DIR"/*.key
print_warning "Private keys secured with restricted permissions"

# Create certificate verification script
cat > "$CERT_DIR/verify_certificates.sh" << 'EOF'
#!/bin/bash
echo "Checking installed certificates..."
echo
echo "Mac App Distribution certificates:"
security find-identity -v -p codesigning | grep "Mac Developer.*Stefan Lohmaier" | grep -v Installer || echo "None found"
echo
echo "Mac Installer Distribution certificates:"
security find-identity -v -p codesigning | grep "Mac.*Installer.*Stefan Lohmaier" || echo "None found"
echo
echo "All code signing identities:"
security find-identity -v -p codesigning
EOF

chmod +x "$CERT_DIR/verify_certificates.sh"

echo
print_success "Certificate setup complete!"
echo
print_status "Files created in: $CERT_DIR"
echo "  - MouseCross_App_Distribution.csr"
echo "  - MouseCross_App_Distribution.key"
echo "  - MouseCross_Installer_Distribution.csr"
echo "  - MouseCross_Installer_Distribution.key"
echo "  - README.txt (detailed instructions)"
echo "  - verify_certificates.sh (check installed certificates)"
echo
print_warning "IMPORTANT: Keep the .key files secure and backed up!"
echo
echo "Next steps:"
echo "1. Go to https://developer.apple.com/account/resources/certificates"
echo "2. Create 'Mac App Distribution' certificate using MouseCross_App_Distribution.csr"
echo "3. Create 'Mac Installer Distribution' certificate using MouseCross_Installer_Distribution.csr"
echo "4. Download and install both certificates"
echo "5. Run: $CERT_DIR/verify_certificates.sh to verify installation"
echo
print_success "CSR files are ready for certificate creation!"

# Open the directory in Finder
if command -v open &> /dev/null; then
    print_status "Opening certificate directory..."
    open "$CERT_DIR"
fi