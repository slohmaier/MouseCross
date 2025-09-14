#!/bin/bash

# MouseCross - Complete App Store Submission Script
# Automates code signing, packaging, and uploading to App Store Connect

set -e  # Exit on any error

# Configuration - UPDATE THESE VALUES
TEAM_ID="9KDRP4Q2D3"                      # Your Apple Developer Team ID
APPLE_ID="stefan@slohmaier.de"             # Your Apple ID email
APP_BUNDLE_ID="de.slohmaier.mousecross"   # App bundle identifier (without team prefix)
FULL_APP_ID="$TEAM_ID.$APP_BUNDLE_ID"     # Full application identifier (with team prefix)
APP_NAME="MouseCross"
VERSION="0.1.12"
BUILD_NUMBER="1"

# Certificate names (actual installed certificate names)
APP_CERT="3rd Party Mac Developer Application: Stefan Lohmaier (9KDRP4Q2D3)"
INSTALLER_CERT="3rd Party Mac Developer Installer: Stefan Lohmaier (9KDRP4Q2D3)"

# Provisioning profile for TestFlight (will be created if needed)
PROVISIONING_PROFILE_NAME="MouseCross_Mac_App_Store.provisionprofile"
PROVISIONING_PROFILE="$HOME/Documents/Apple Developer/$PROVISIONING_PROFILE_NAME"

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

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if running from correct directory
check_environment() {
    print_status "Checking environment..."

    if [[ ! -f "../../CMakeLists.txt" ]]; then
        print_error "Must run from deployment/macos directory"
        exit 1
    fi

    if [[ ! -d "../../dist/MouseCross.app" ]]; then
        print_error "App bundle not found. Run ./build_appstore.sh first"
        exit 1
    fi

    # Check if certificates are installed
    if ! security find-identity -v -p codesigning | grep -q "3rd Party Mac Developer.*Stefan Lohmaier"; then
        print_error "3rd Party Mac Developer certificate not found in keychain"
        print_warning "Please install your distribution certificates first"
        print_status "Run the certificate creation script: ./create_csrs.sh"
        exit 1
    fi

    # Note: Provisioning profile not required for direct Mac App Store submission
    # Only needed for TestFlight distribution
    if [[ -f "$PROVISIONING_PROFILE" ]]; then
        print_status "Provisioning profile found, but not required for Mac App Store"
    else
        print_status "No provisioning profile found (not required for Mac App Store)"
    fi

    print_success "Environment check passed"
}

# Function to validate configuration
validate_config() {
    print_status "Validating configuration..."

    if [[ "$TEAM_ID" == "YOUR_TEAM_ID" ]]; then
        print_error "Please update TEAM_ID in the script"
        exit 1
    fi

    if [[ "$APPLE_ID" == "your-email@example.com" ]]; then
        print_error "Please update APPLE_ID in the script"
        exit 1
    fi

    print_success "Configuration validated"
}

# Function to create entitlements file
create_entitlements() {
    print_status "Creating entitlements file..."

    cat > MouseCross.entitlements << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>com.apple.security.app-sandbox</key>
    <true/>
    <key>com.apple.security.files.user-selected.read-only</key>
    <true/>
</dict>
</plist>
EOF

    print_success "Entitlements file created"
}

# Function to sign all components
sign_app() {
    print_status "Starting code signing process..."

    APP_PATH="../../dist/MouseCross.app"

    # Sign all Qt frameworks
    print_status "Signing Qt frameworks..."
    for framework in "$APP_PATH/Contents/Frameworks"/*.framework; do
        if [[ -d "$framework" ]]; then
            framework_name=$(basename "$framework")
            print_status "  Signing $framework_name..."
            codesign --force --verify --verbose \
                --sign "$APP_CERT" \
                --timestamp \
                "$framework"
        fi
    done

    # Sign all plugins
    print_status "Signing plugins..."
    find "$APP_PATH/Contents/PlugIns" -name "*.dylib" -exec \
        codesign --force --verify --verbose \
        --sign "$APP_CERT" \
        --timestamp \
        {} \;

    # Sign the main executable with Bundle ID (matches App Store Connect)
    print_status "Signing main executable..."
    codesign --force --verify --verbose \
        --sign "$APP_CERT" \
        --identifier "$APP_BUNDLE_ID" \
        --timestamp \
        "$APP_PATH/Contents/MacOS/MouseCross"

    # Remove quarantine attributes from all files (ITMS-91109)
    print_status "Removing quarantine attributes from app bundle..."
    xattr -cr "$APP_PATH"

    # Sign the app bundle with entitlements and Bundle ID (matches App Store Connect)
    print_status "Signing app bundle with entitlements and Bundle ID..."
    codesign --force --verify --verbose \
        --sign "$APP_CERT" \
        --entitlements MouseCross.entitlements \
        --identifier "$APP_BUNDLE_ID" \
        --timestamp \
        "$APP_PATH"

    print_success "App bundle signed successfully"
}

# Function to verify code signature
verify_signature() {
    print_status "Verifying code signatures..."

    APP_PATH="../../dist/MouseCross.app"

    # Verify app bundle signature
    if codesign --verify --deep --verbose=2 "$APP_PATH"; then
        print_success "Code signature verification passed"
    else
        print_error "Code signature verification failed"
        exit 1
    fi

    # Check if it's properly signed for App Store
    if spctl --assess --verbose "$APP_PATH"; then
        print_success "App Store signature check passed"
    else
        print_warning "App Store signature check failed (this may be normal for unsigned developer builds)"
    fi
}

# Function to create installer package
create_installer() {
    print_status "Creating installer package..."

    APP_PATH="../../dist/MouseCross.app"
    PKG_NAME="MouseCross-${VERSION}-signed.pkg"

    productbuild --component "$APP_PATH" /Applications \
        --sign "$INSTALLER_CERT" \
        --timestamp \
        "$PKG_NAME"

    if [[ -f "$PKG_NAME" ]]; then
        print_success "Installer package created: $PKG_NAME"

        # Get file size
        SIZE=$(du -h "$PKG_NAME" | cut -f1)
        print_status "Package size: $SIZE"

        return 0
    else
        print_error "Failed to create installer package"
        exit 1
    fi
}

# Function to upload to App Store Connect
upload_to_appstore() {
    print_status "Uploading to App Store Connect..."

    PKG_NAME="MouseCross-${VERSION}-signed.pkg"

    # Check if app-specific password is set
    if [[ -z "$APP_SPECIFIC_PASSWORD" ]]; then
        print_warning "APP_SPECIFIC_PASSWORD environment variable not set"
        print_status "Please set it with: export APP_SPECIFIC_PASSWORD='your-app-specific-password'"
        print_status "Generate one at: https://appleid.apple.com/account/manage"

        read -p "Enter your app-specific password: " -s APP_SPECIFIC_PASSWORD
        echo
    fi

    print_status "Starting upload (this may take several minutes)..."

    if xcrun altool --upload-app \
        --type osx \
        --file "$PKG_NAME" \
        --username "$APPLE_ID" \
        --password "$APP_SPECIFIC_PASSWORD" \
        --verbose; then
        print_success "Upload completed successfully!"
        print_status "Check App Store Connect for processing status"
    else
        print_error "Upload failed"
        print_status "Common solutions:"
        print_status "1. Check your app-specific password"
        print_status "2. Verify your Apple ID has access to the team"
        print_status "3. Ensure the app record exists in App Store Connect"
        exit 1
    fi
}

# Function to create App Store Connect metadata JSON
create_metadata_json() {
    print_status "Creating App Store Connect metadata template..."

    cat > appstore_metadata.json << EOF
{
  "app_info": {
    "bundle_id": "$APP_BUNDLE_ID",
    "name": "$APP_NAME",
    "version": "$VERSION",
    "build_number": "$BUILD_NUMBER",
    "category": "UTILITIES",
    "subcategory": "ACCESSIBILITY"
  },
  "description": {
    "en": "MouseCross helps visually impaired users locate their mouse cursor with a customizable crosshair overlay.\n\nFeatures:\n• Customizable crosshair with multiple shapes (Circle, Arrow, Cross, Diamond)\n• Adjustable size, color, and opacity settings\n• Inverted mode for visibility on any background\n• System tray integration with easy toggle controls\n• Accessible design with keyboard shortcuts\n• Runs efficiently in the background\n\nPerfect for users with visual impairments, large displays, or anyone who frequently loses track of their cursor. MouseCross is free, open-source software licensed under LGPL v3.0.\n\nSource code available at: slohmaier.de/mousecross"
  },
  "keywords": "accessibility,mouse,cursor,crosshair,visual,impaired,overlay,utilities",
  "support_url": "https://slohmaier.de/mousecross",
  "marketing_url": "https://slohmaier.de/mousecross",
  "privacy_url": "https://slohmaier.de/mousecross/privacy",
  "review_notes": "MouseCross is an accessibility application that helps visually impaired users locate their mouse cursor.\n\nTESTING INSTRUCTIONS:\n1. Launch MouseCross from Applications folder\n2. The app runs in the system tray (menu bar)\n3. Click the MouseCross icon in menu bar to access settings\n4. Enable crosshair overlay - you'll see crosshair following mouse cursor\n5. Test different crosshair shapes and settings in preferences\n6. Verify system tray menu functions (toggle, settings, quit)\n\nThe app requires no special permissions and runs entirely in user space.",
  "pricing": {
    "free": true,
    "price_tier": null
  },
  "availability": {
    "countries": ["US", "GB", "DE", "FR", "ES", "IT", "NL", "BE", "CH", "AT", "SE", "NO", "DK", "FI"],
    "release_date": "automatic"
  }
}
EOF

    print_success "Metadata template created: appstore_metadata.json"
    print_status "Edit this file to customize your app information"
}

# Function to show manual setup instructions
show_manual_setup() {
    print_status "App Store Connect Manual Setup Required:"
    echo
    echo "1. Go to https://appstoreconnect.apple.com"
    echo "2. Click 'My Apps' → '+ App'"
    echo "3. Fill in the following information:"
    echo "   - Platform: macOS"
    echo "   - Name: $APP_NAME"
    echo "   - Primary Language: English (U.S.)"
    echo "   - Bundle ID: $APP_BUNDLE_ID"
    echo "   - SKU: mousecross-macos (or similar unique identifier)"
    echo
    echo "4. After creating the app record, you can use this script to upload builds"
    echo
}

# Function to display help
show_help() {
    echo "MouseCross App Store Submission Script"
    echo
    echo "Usage: $0 [OPTIONS]"
    echo
    echo "Options:"
    echo "  --sign-only       Only sign the app bundle (skip upload)"
    echo "  --upload-only     Only upload (assumes already signed)"
    echo "  --setup           Show manual App Store Connect setup instructions"
    echo "  --metadata        Create metadata template file"
    echo "  --help            Show this help message"
    echo
    echo "Environment Variables:"
    echo "  APP_SPECIFIC_PASSWORD    Your Apple ID app-specific password"
    echo
    echo "Before running:"
    echo "1. Update TEAM_ID, APPLE_ID, and certificate names in this script"
    echo "2. Install your Mac App Store distribution certificates"
    echo "3. Create the app record in App Store Connect (use --setup for instructions)"
    echo "4. Run ./build_appstore.sh to create the app bundle"
    echo
    echo "Full workflow:"
    echo "  ./build_appstore.sh      # Build the app"
    echo "  ./submit_appstore.sh     # Sign and upload"
}

# Main execution
main() {
    echo "================================================"
    echo "MouseCross - App Store Submission Script"
    echo "================================================"
    echo

    case "${1:-}" in
        --help)
            show_help
            exit 0
            ;;
        --setup)
            show_manual_setup
            exit 0
            ;;
        --metadata)
            create_metadata_json
            exit 0
            ;;
        --sign-only)
            check_environment
            validate_config
            create_entitlements
            sign_app
            verify_signature
            create_installer
            print_success "Signing complete! Package ready for upload."
            exit 0
            ;;
        --upload-only)
            if [[ ! -f "MouseCross-${VERSION}-signed.pkg" ]]; then
                print_error "Signed package not found. Run with --sign-only first."
                exit 1
            fi
            upload_to_appstore
            exit 0
            ;;
        "")
            # Full workflow
            check_environment
            validate_config
            create_entitlements
            sign_app
            verify_signature
            create_installer
            upload_to_appstore

            print_success "App Store submission completed!"
            print_status "Next steps:"
            print_status "1. Check App Store Connect for processing status"
            print_status "2. Configure app metadata and screenshots"
            print_status "3. Submit for review when ready"
            ;;
        *)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
}

main "$@"