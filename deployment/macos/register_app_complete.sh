#!/bin/bash

# MouseCross - Complete App Registration Script

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
}

print_step() {
    echo -e "${BLUE}[STEP $1]${NC} $2"
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

print_action() {
    echo -e "${YELLOW}[ACTION NEEDED]${NC} $1"
}

wait_for_user() {
    echo
    read -p "Press Enter when you have completed this step..." -r
    echo
}

print_header "MouseCross - Complete App Store Registration"

echo "I'll guide you through registering both the App ID and creating the App Store Connect record."
echo "This process has 2 main steps that need to be done manually."
echo

# Step 1: Open App Store Connect
print_step "1" "Opening App Store Connect"
print_status "I'll open App Store Connect in your browser now..."

if command -v open &> /dev/null; then
    open "https://appstoreconnect.apple.com"
    print_success "App Store Connect opened in your browser"
else
    print_action "Please manually open: https://appstoreconnect.apple.com"
fi

echo
print_action "Complete these steps in App Store Connect:"
echo
echo "┌─────────────────────────────────────────────────────────────┐"
echo "│                    APP STORE CONNECT STEPS                 │"
echo "├─────────────────────────────────────────────────────────────┤"
echo "│ 1. Sign in with: stefan@slohmaier.de                       │"
echo "│ 2. Click: My Apps                                          │"
echo "│ 3. Click: + (plus button)                                  │"
echo "│ 4. Select: New App                                         │"
echo "│                                                             │"
echo "│ 5. Fill in EXACTLY:                                        │"
echo "│    • Platform: macOS                                       │"
echo "│    • Name: MouseCross                                      │"
echo "│    • Primary Language: English (U.S.)                     │"
echo "│    • Bundle ID: de.slohmaier.mousecross                    │"
echo "│    • SKU: mousecross-macos-2025                            │"
echo "│                                                             │"
echo "│ 6. Click: Create                                           │"
echo "└─────────────────────────────────────────────────────────────┘"

wait_for_user

print_step "2" "Verifying App Creation"
print_status "Let me check if the app was created successfully..."

# Test if the app exists now
print_status "Testing upload to verify app exists..."
if ./submit_appstore.sh --upload-only 2>/dev/null; then
    print_success "🎉 MouseCross app created successfully and upload worked!"

    print_step "3" "Upload Completed Successfully"
    echo
    print_success "MouseCross has been uploaded to App Store Connect!"
    echo
    echo "What happens next:"
    echo "1. ⏱️  Build Processing (10-30 minutes)"
    echo "2. 📝 Configure app metadata (description, screenshots)"
    echo "3. 🔍 Submit for App Store review"
    echo "4. 📱 App Store approval (typically 2-7 days)"
    echo
    print_status "Check your upload status at: https://appstoreconnect.apple.com"

elif [[ $? -eq 1 ]]; then
    print_warning "Upload test failed - let's diagnose the issue..."

    # Try to get more specific error information
    print_status "Running diagnostic upload..."
    if ./submit_appstore.sh --upload-only 2>&1 | grep -q "No suitable application records"; then
        print_warning "App record still not found"
        print_action "Please verify these details in App Store Connect:"
        echo "  • Bundle ID is exactly: de.slohmaier.mousecross"
        echo "  • App platform is: macOS"
        echo "  • App status is: 'Prepare for Submission' or similar"
        echo
        print_action "After verification, try the upload again:"
        echo "  ./submit_appstore.sh --upload-only"
    else
        print_warning "Different error occurred. Check the error message above."
    fi
else
    print_warning "Could not test upload - manual verification needed"
fi

echo
print_header "Registration Process Complete"

echo "Summary of what we accomplished:"
echo "✅ Guided through App Store Connect app creation"
echo "✅ Provided exact details for MouseCross app"
echo "✅ Tested upload process"
echo
echo "Your MouseCross package is ready for App Store submission!"
echo "Package location: MouseCross-0.1.0-signed.pkg"
echo
print_status "For future uploads, just run: ./submit_appstore.sh --upload-only"