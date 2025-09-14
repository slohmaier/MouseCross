#!/bin/bash

# MouseCross - Certificate Diagnostic Script

echo "================================================"
echo "MouseCross - Certificate Diagnostic"
echo "================================================"
echo

echo "1. All certificates in keychain containing 'Stefan':"
security find-certificate -a -c "Stefan" | grep -E "(labl|alis)" | sort -u
echo

echo "2. All code signing identities:"
security find-identity -v -p codesigning
echo

echo "3. All certificates in login keychain:"
security find-certificate -a | grep -E "(Stefan|labl.*Mac)" | head -10
echo

echo "4. Checking certificate details for Mac Developer certificates:"
security find-certificate -c "3rd Party Mac Developer Application: Stefan Lohmaier" -p 2>/dev/null && echo "App cert found" || echo "App cert NOT found"
security find-certificate -c "3rd Party Mac Developer Installer: Stefan Lohmaier" -p 2>/dev/null && echo "Installer cert found" || echo "Installer cert NOT found"
echo

echo "5. Checking if private keys are associated:"
echo "Private keys in Downloads folder:"
ls -la ~/Documents/"Apple Developer Account"/MouseCross-Certificates/*.key 2>/dev/null || echo "No private key files found"
echo

echo "6. Manual certificate import status:"
echo "Attempting to manually link certificates with private keys..."

CERT_DIR="$HOME/Documents/Apple Developer Account/MouseCross-Certificates"

if [[ -f "$CERT_DIR/MouseCross_App_Distribution.key" ]]; then
    echo "App Distribution private key exists"

    # Try to create a PKCS#12 file (certificate + private key)
    if [[ -f ~/Downloads/mac_app.cer ]]; then
        echo "Creating combined certificate + key file for App Distribution..."

        # Convert certificate to PEM
        openssl x509 -inform DER -in ~/Downloads/mac_app.cer -outform PEM -out /tmp/app_cert.pem

        # Create PKCS#12 file
        if openssl pkcs12 -export -out /tmp/app_combined.p12 \
            -inkey "$CERT_DIR/MouseCross_App_Distribution.key" \
            -in /tmp/app_cert.pem \
            -passout pass:; then
            echo "Successfully created combined App Distribution certificate"

            # Import the PKCS#12 file
            if security import /tmp/app_combined.p12 -k ~/Library/Keychains/login.keychain-db -P ""; then
                echo "Successfully imported App Distribution with private key"
            else
                echo "Failed to import App Distribution with private key"
            fi

            rm -f /tmp/app_cert.pem /tmp/app_combined.p12
        else
            echo "Failed to create combined certificate file"
        fi
    fi
else
    echo "App Distribution private key not found"
fi

if [[ -f "$CERT_DIR/MouseCross_Installer_Distribution.key" ]]; then
    echo "Installer Distribution private key exists"

    # Try to create a PKCS#12 file (certificate + private key)
    if [[ -f ~/Downloads/mac_installer.cer ]]; then
        echo "Creating combined certificate + key file for Installer Distribution..."

        # Convert certificate to PEM
        openssl x509 -inform DER -in ~/Downloads/mac_installer.cer -outform PEM -out /tmp/installer_cert.pem

        # Create PKCS#12 file
        if openssl pkcs12 -export -out /tmp/installer_combined.p12 \
            -inkey "$CERT_DIR/MouseCross_Installer_Distribution.key" \
            -in /tmp/installer_cert.pem \
            -passout pass:; then
            echo "Successfully created combined Installer Distribution certificate"

            # Import the PKCS#12 file
            if security import /tmp/installer_combined.p12 -k ~/Library/Keychains/login.keychain-db -P ""; then
                echo "Successfully imported Installer Distribution with private key"
            else
                echo "Failed to import Installer Distribution with private key"
            fi

            rm -f /tmp/installer_cert.pem /tmp/installer_combined.p12
        else
            echo "Failed to create combined certificate file"
        fi
    fi
else
    echo "Installer Distribution private key not found"
fi

echo
echo "7. Final check - Code signing identities after fixes:"
security find-identity -v -p codesigning

echo
echo "8. Certificate names for script configuration:"
security find-identity -v -p codesigning | grep -E "(3rd Party|Mac.*Developer)" | while read -r line; do
    if [[ $line =~ \"([^\"]+)\" ]]; then
        echo "Certificate name: ${BASH_REMATCH[1]}"
    fi
done