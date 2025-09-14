#!/bin/bash

# MouseCross - Complete App Store Setup Script
# Guides through the entire App Store Connect setup process

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

# Function to wait for user input
wait_for_user() {
    echo
    read -p "Press Enter to continue..." -r
    echo
}

# Function to ask yes/no question
ask_yes_no() {
    while true; do
        read -p "$1 (y/n): " -r yn
        case $yn in
            [Yy]* ) return 0;;
            [Nn]* ) return 1;;
            * ) echo "Please answer yes or no.";;
        esac
    done
}

# Step 1: Prerequisites check
check_prerequisites() {
    print_step "1" "Checking Prerequisites"

    # Check if we're in the right directory
    if [[ ! -f "../../CMakeLists.txt" ]]; then
        print_error "Must run from deployment/macos directory"
        exit 1
    fi

    # Check for required tools
    local missing_tools=()

    if ! command -v xcrun &> /dev/null; then
        missing_tools+=("Xcode Command Line Tools")
    fi

    if ! command -v codesign &> /dev/null; then
        missing_tools+=("codesign")
    fi

    if ! command -v productbuild &> /dev/null; then
        missing_tools+=("productbuild")
    fi

    if ! command -v python3 &> /dev/null; then
        missing_tools+=("Python 3")
    fi

    if [ ${#missing_tools[@]} -ne 0 ]; then
        print_error "Missing required tools:"
        for tool in "${missing_tools[@]}"; do
            echo "  - $tool"
        done
        print_action "Install missing tools before continuing"
        exit 1
    fi

    print_success "All required tools are installed"

    # Check if app is built
    if [[ ! -d "../../dist/MouseCross.app" ]]; then
        print_warning "App bundle not found"
        print_action "Run ./build_appstore.sh first to build the app"

        if ask_yes_no "Would you like to build it now?"; then
            print_status "Building MouseCross for App Store..."
            ./build_appstore.sh
            print_success "Build completed"
        else
            print_error "App bundle is required. Exiting."
            exit 1
        fi
    else
        print_success "App bundle found"
    fi

    wait_for_user
}

# Step 2: Apple Developer Account setup
setup_developer_account() {
    print_step "2" "Apple Developer Account Setup"

    print_status "You need an active Apple Developer Program membership to submit to the App Store."
    echo
    echo "Requirements:"
    echo "  • Apple Developer Program membership (\$99/year)"
    echo "  • Mac App Store distribution certificates"
    echo "  • App Store Connect access with App Manager role"
    echo

    if ! ask_yes_no "Do you have an active Apple Developer Program membership?"; then
        print_action "Sign up at: https://developer.apple.com/programs/"
        print_status "Come back after enrollment is complete"
        exit 0
    fi

    print_success "Developer membership confirmed"
    wait_for_user
}

# Step 3: Certificate setup
setup_certificates() {
    print_step "3" "Code Signing Certificates"

    print_status "Checking for required certificates..."

    # Check for App Store certificates
    local has_app_cert=false
    local has_installer_cert=false

    if security find-identity -v -p codesigning | grep -q "3rd Party Mac Developer Application"; then
        has_app_cert=true
        print_success "Mac Developer Application certificate found"
    else
        print_warning "Mac Developer Application certificate not found"
    fi

    if security find-identity -v -p codesigning | grep -q "3rd Party Mac Developer Installer"; then
        has_installer_cert=true
        print_success "Mac Developer Installer certificate found"
    else
        print_warning "Mac Developer Installer certificate not found"
    fi

    if [[ "$has_app_cert" == false ]] || [[ "$has_installer_cert" == false ]]; then
        print_action "You need to create and install distribution certificates"
        echo
        echo "To create certificates:"
        echo "1. Go to https://developer.apple.com/account/resources/certificates"
        echo "2. Click '+' to create new certificates"
        echo "3. Select 'Mac App Store' under 'Distribution'"
        echo "4. Create both 'Mac App Store Application' and 'Mac Installer Distribution'"
        echo "5. Download and install both certificates"
        echo

        if ask_yes_no "Have you installed the required certificates?"; then
            # Re-check certificates
            setup_certificates
            return
        else
            print_error "Certificates are required for code signing. Exiting."
            exit 1
        fi
    fi

    print_success "All required certificates are installed"
    wait_for_user
}

# Step 4: App Store Connect setup
setup_appstore_connect() {
    print_step "4" "App Store Connect Setup"

    print_status "Setting up App Store Connect integration..."

    # Check if API helper dependencies are available
    if ! python3 -c "import jwt, requests" 2>/dev/null; then
        print_warning "Required Python packages not installed"
        print_action "Installing required packages..."

        if pip3 install PyJWT requests; then
            print_success "Python packages installed"
        else
            print_warning "Failed to install packages. API features will be limited."
        fi
    fi

    # Create API configuration if it doesn't exist
    if [[ ! -f "appstore_config.json" ]]; then
        print_status "Creating App Store Connect API configuration template..."
        python3 appstore_api_helper.py --setup

        echo
        print_action "Edit appstore_config.json with your API credentials"
        echo "To get API credentials:"
        echo "1. Go to https://appstoreconnect.apple.com/access/api"
        echo "2. Generate API key with 'App Manager' role"
        echo "3. Download the .p8 private key file"
        echo "4. Copy Key ID and Issuer ID to appstore_config.json"
        echo

        if ask_yes_no "Would you like to configure API access now?"; then
            echo "Opening configuration file..."
            if command -v code &> /dev/null; then
                code appstore_config.json
            elif command -v nano &> /dev/null; then
                nano appstore_config.json
            else
                print_status "Edit appstore_config.json manually"
            fi

            wait_for_user
        fi
    fi

    # Manual App Store Connect setup instructions
    print_status "Manual App Store Connect Setup"
    echo
    echo "Go to https://appstoreconnect.apple.com and create your app:"
    echo "1. Click 'My Apps' → '+ App'"
    echo "2. Fill in the information:"
    echo "   - Platform: macOS"
    echo "   - Name: MouseCross"
    echo "   - Primary Language: English (U.S.)"
    echo "   - Bundle ID: de.slohmaier.mousecross"
    echo "   - SKU: mousecross-macos (unique identifier)"
    echo "3. Complete basic app information"
    echo

    if ask_yes_no "Have you created the app record in App Store Connect?"; then
        print_success "App Store Connect setup complete"
    else
        print_action "Please create the app record before continuing"
        exit 0
    fi

    wait_for_user
}

# Step 5: Configuration
configure_submission() {
    print_step "5" "Submission Configuration"

    print_status "Configuring submission script..."

    # Check if submission script is configured
    if grep -q "YOUR_TEAM_ID" submit_appstore.sh; then
        print_warning "Submission script needs configuration"
        print_action "You need to edit submit_appstore.sh with your details:"
        echo "  - TEAM_ID: Your Apple Developer Team ID"
        echo "  - APPLE_ID: Your Apple ID email"
        echo "  - Certificate names: Update with your actual certificate names"
        echo

        if ask_yes_no "Would you like to edit the script now?"; then
            if command -v code &> /dev/null; then
                code submit_appstore.sh
            elif command -v nano &> /dev/null; then
                nano submit_appstore.sh
            else
                print_status "Edit submit_appstore.sh manually"
            fi
            wait_for_user
        fi
    else
        print_success "Submission script already configured"
    fi

    # Create app-specific password reminder
    echo
    print_status "App-Specific Password Setup"
    echo "You'll need an app-specific password for uploading:"
    echo "1. Go to https://appleid.apple.com/account/manage"
    echo "2. Sign in with your Apple ID"
    echo "3. In Security section, generate app-specific password"
    echo "4. Copy the password for use during upload"
    echo

    wait_for_user
}

# Step 6: Test submission
test_submission() {
    print_step "6" "Test Submission Process"

    print_status "Ready to test the submission process"
    echo
    echo "Available options:"
    echo "1. Sign-only test (recommended first)"
    echo "2. Full submission (sign + upload)"
    echo "3. Skip test"
    echo

    read -p "Choose option (1-3): " choice
    case $choice in
        1)
            print_status "Running sign-only test..."
            if ./submit_appstore.sh --sign-only; then
                print_success "Signing test completed successfully!"
                print_status "Signed package created and ready for upload"
            else
                print_error "Signing test failed"
                print_action "Check the error messages and fix any issues"
            fi
            ;;
        2)
            print_warning "This will upload to App Store Connect"
            if ask_yes_no "Are you sure you want to proceed?"; then
                print_status "Running full submission..."
                ./submit_appstore.sh
            fi
            ;;
        3)
            print_status "Skipping test submission"
            ;;
        *)
            print_error "Invalid choice"
            ;;
    esac

    wait_for_user
}

# Step 7: Next steps
show_next_steps() {
    print_step "7" "Next Steps"

    print_success "Setup completed!"
    echo
    echo "What you can do now:"
    echo
    echo "📦 Build and Submit:"
    echo "  ./build_appstore.sh        # Build the app"
    echo "  ./submit_appstore.sh        # Sign and upload to App Store"
    echo
    echo "🔧 Advanced Options:"
    echo "  ./submit_appstore.sh --sign-only      # Just sign the app"
    echo "  ./submit_appstore.sh --upload-only    # Just upload (if already signed)"
    echo "  ./submit_appstore.sh --help           # Show all options"
    echo
    echo "🔗 API Management:"
    echo "  python3 appstore_api_helper.py --list-apps     # List your apps"
    echo "  python3 appstore_api_helper.py --app-info [bundle-id]  # Get app info"
    echo
    echo "📚 Documentation:"
    echo "  - Complete submission guide: ../../APP_STORE_SUBMISSION_GUIDE.md"
    echo "  - LGPL compliance docs: ../../LGPL_REBUILD_GUIDE.md"
    echo
    echo "🌐 Useful Links:"
    echo "  - App Store Connect: https://appstoreconnect.apple.com"
    echo "  - Developer Portal: https://developer.apple.com/account"
    echo "  - API Keys: https://appstoreconnect.apple.com/access/api"
    echo

    print_success "MouseCross is ready for App Store submission!"
}

# Main execution
main() {
    print_header "MouseCross - App Store Setup Wizard"

    echo "This wizard will guide you through setting up MouseCross for App Store submission."
    echo "It will check prerequisites, configure certificates, and prepare for upload."
    echo

    if ask_yes_no "Ready to start the setup process?"; then
        check_prerequisites
        setup_developer_account
        setup_certificates
        setup_appstore_connect
        configure_submission
        test_submission
        show_next_steps
    else
        print_status "Setup cancelled. Run again when ready."
        exit 0
    fi
}

# Show help if requested
if [[ "${1:-}" == "--help" ]]; then
    echo "MouseCross App Store Setup Wizard"
    echo
    echo "This script guides you through the complete App Store submission setup:"
    echo "• Checks prerequisites and tools"
    echo "• Verifies Apple Developer certificates"
    echo "• Sets up App Store Connect integration"
    echo "• Configures submission scripts"
    echo "• Tests the submission process"
    echo
    echo "Usage: $0"
    echo
    echo "The wizard is interactive and will guide you through each step."
    exit 0
fi

main "$@"