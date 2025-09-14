#!/bin/bash

# MouseCross - Provisioning Profile Creation Guide
# Helps create the Mac App Store provisioning profile required for TestFlight

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

print_header() {
    echo -e "${CYAN}================================================${NC}"
    echo -e "${CYAN}$1${NC}"
    echo -e "${CYAN}================================================${NC}"
    echo
}

print_step() {
    echo -e "${BLUE}[STEP $1]${NC} $2"
    echo
}

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

print_action() {
    echo -e "${YELLOW}[ACTION REQUIRED]${NC} $1"
}

wait_for_user() {
    echo
    read -p "Press Enter when you have completed this step..." -r
    echo
}

# Configuration
TEAM_ID="9KDRP4Q2D3"
APP_BUNDLE_ID="de.slohmaier.mousecross"
PROVISIONING_PROFILE="MouseCross_Mac_App_Store.provisionprofile"

print_header "MouseCross - Create Mac App Store Provisioning Profile"

echo "This script will guide you through creating a Mac App Store provisioning profile"
echo "required for TestFlight distribution."
echo
echo "App Details:"
echo "  Bundle ID: $APP_BUNDLE_ID"
echo "  Team ID: $TEAM_ID"
echo "  Profile Name: $PROVISIONING_PROFILE"
echo

print_step "1" "Opening Apple Developer Portal"

if command -v open &> /dev/null; then
    open "https://developer.apple.com/account/resources/profiles/"
    print_success "Apple Developer Portal opened in your browser"
else
    print_action "Please manually open: https://developer.apple.com/account/resources/profiles/"
fi

echo
print_action "Complete these steps in Apple Developer Portal:"
echo
echo "┌─────────────────────────────────────────────────────────────┐"
echo "│                  PROVISIONING PROFILE CREATION             │"
echo "├─────────────────────────────────────────────────────────────┤"
echo "│ 1. Sign in to Apple Developer Portal                       │"
echo "│ 2. Go to: Certificates, Identifiers & Profiles             │"
echo "│ 3. Click: Profiles                                         │"
echo "│ 4. Click: + (plus button)                                  │"
echo "│                                                             │"
echo "│ 5. Select Profile Type:                                    │"
echo "│    • Choose: Mac App Store (under Distribution)           │"
echo "│    • Click: Continue                                       │"
echo "│                                                             │"
echo "│ 6. Select App ID:                                          │"
echo "│    • Choose: de.slohmaier.mousecross                      │"
echo "│    • Click: Continue                                       │"
echo "│                                                             │"
echo "│ 7. Select Certificates:                                    │"
echo "│    • Select your 'Mac App Store' distribution certificate │"
echo "│    • Click: Continue                                       │"
echo "│                                                             │"
echo "│ 8. Name the Profile:                                       │"
echo "│    • Profile Name: MouseCross Mac App Store               │"
echo "│    • Click: Generate                                       │"
echo "│                                                             │"
echo "│ 9. Download:                                               │"
echo "│    • Click: Download                                       │"
echo "│    • Save as: MouseCross_Mac_App_Store.provisionprofile   │"
echo "│    • Move to this directory: deployment/macos/            │"
echo "└─────────────────────────────────────────────────────────────┘"

wait_for_user

print_step "2" "Verifying Provisioning Profile"

if [[ -f "$PROVISIONING_PROFILE" ]]; then
    print_success "Provisioning profile found: $PROVISIONING_PROFILE"

    # Show profile details
    print_status "Profile details:"
    security cms -D -i "$PROVISIONING_PROFILE" | grep -A 5 -E "(Name|TeamIdentifier|ApplicationIdentifierPrefix)" || true

    print_step "3" "Testing App Store Submission"
    print_status "Now you can run the App Store submission with TestFlight support:"
    echo "  ./submit_appstore.sh"
    echo
    print_success "Provisioning profile setup complete! 🎉"

else
    print_error "Provisioning profile not found: $PROVISIONING_PROFILE"
    print_action "Please download the provisioning profile and place it in this directory"
    print_status "Expected filename: $PROVISIONING_PROFILE"
    exit 1
fi

echo
print_header "Next Steps"
echo "1. Run: ./submit_appstore.sh to build and upload with provisioning profile"
echo "2. The app bundle will now include embedded.provisionprofile for TestFlight"
echo "3. After upload, check App Store Connect for TestFlight availability"
echo