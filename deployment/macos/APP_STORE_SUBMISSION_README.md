# MouseCross - Mac App Store Submission Guide

This guide documents all scripts and processes for submitting MouseCross to the Mac App Store. Each script serves a specific purpose in the build, sign, and submission pipeline.

## Overview

The Mac App Store submission process involves several scripts working together to:
1. Build the application with proper App Store configuration
2. Code sign the application and create installer packages
3. Register Bundle IDs and create App Store Connect records
4. Upload the signed package to App Store Connect

## Required Scripts Analysis

### ✅ Core Build Scripts (Required)

#### `build_appstore.sh` - **REQUIRED**
**Purpose**: Builds MouseCross specifically for App Store submission with proper configuration.

**What it does**:
- Cleans previous builds and creates fresh build directory
- Configures CMake with `APPSTORE_BUILD=ON` for universal binary (x86_64 + arm64)
- Uses static Qt path: `/Users/stefan/Qt/6.9.0/macos`
- Runs `macdeployqt` to bundle Qt libraries
- Creates unsigned App Store package: `MouseCross-0.1.0-appstore.pkg`

**When to use**: First step in App Store submission process
```bash
./build_appstore.sh
```

#### `submit_appstore.sh` - **REQUIRED**
**Purpose**: Complete code signing and upload to App Store Connect.

**What it does**:
- Validates environment, certificates, and provisioning profile
- Creates proper entitlements file (sandbox + file access only)
- Signs all Qt frameworks, plugins, and the main executable
- Embeds Mac App Store provisioning profile for TestFlight compatibility
- Signs app bundle with entitlements and provisioning profile
- Creates signed installer package
- Uploads to App Store Connect using `altool`

**Configuration**:
- Team ID: `9KDRP4Q2D3`
- Apple ID: `stefan@slohmaier.de`
- Bundle ID: `de.slohmaier.mousecross`

**Usage**:
```bash
# Full process (sign + upload)
./submit_appstore.sh

# Sign only
./submit_appstore.sh --sign-only

# Upload only (if already signed)
./submit_appstore.sh --upload-only
```

### ✅ Certificate Management Scripts (Required for First-Time Setup)

#### `create_csrs.sh` - **REQUIRED (One-time)**
**Purpose**: Creates Certificate Signing Requests (CSRs) for Mac App Store distribution certificates.

**What it does**:
- Generates private keys and CSRs for both App and Installer certificates
- Provides instructions for uploading CSRs to Apple Developer Portal
- Creates: `MouseCross_App_Certificate.certSigningRequest` and `MouseCross_Installer_Certificate.certSigningRequest`

**When to use**: Only once, when setting up certificates for the first time

#### `install_certificates.sh` - **REQUIRED (One-time)**
**Purpose**: Installs downloaded certificates from Apple into the keychain.

**What it does**:
- Installs Mac App Store distribution certificates
- Verifies certificate installation
- Checks keychain access permissions

**When to use**: After downloading certificates from Apple Developer Portal

#### `create_provisioning_profile.sh` - **REQUIRED (One-time)**
**Purpose**: Guides through creating Mac App Store provisioning profile for TestFlight.

**What it does**:
- Opens Apple Developer Portal in browser
- Provides step-by-step instructions for provisioning profile creation
- Validates downloaded provisioning profile
- Creates: `MouseCross_Mac_App_Store.provisionprofile`

**When to use**: One-time setup for TestFlight compatibility (fixes ITMS-90889 error)

#### `diagnose_certificates.sh` - **UTILITY**
**Purpose**: Troubleshooting tool to check certificate status.

**What it does**:
- Lists all code signing identities
- Checks for specific App Store certificates
- Validates certificate expiration dates
- Shows keychain status

**When to use**: When troubleshooting code signing issues

### ❓ App Store Connect Setup Scripts (Partially Required)

#### `register_app_complete.sh` - **REQUIRED (One-time)**
**Purpose**: Guides through creating the App Store Connect app record.

**What it does**:
- Opens App Store Connect in browser
- Provides step-by-step instructions for app creation
- Tests upload after app creation
- Provides exact Bundle ID and app details

**When to use**: One-time setup when creating the App Store Connect app record

#### `auto_create_app.sh` - **DEPRECATED**
**Status**: ❌ Not working reliably
**Purpose**: Attempted to automate App Store Connect app creation via AppleScript.

**Issues**:
- Browser automation unreliable
- App Store Connect interface changes break automation
- Manual process more reliable

**Recommendation**: Use `register_app_complete.sh` instead

#### `setup_appstore.sh` - **COMPREHENSIVE SETUP**
**Purpose**: Complete interactive setup wizard for entire App Store submission process.

**What it does**:
- Checks all prerequisites and tools
- Verifies Apple Developer certificates
- Guides through App Store Connect setup
- Configures submission scripts
- Tests the submission process

**When to use**: For complete first-time setup or troubleshooting

### 🔧 Utility and Legacy Scripts

#### `build_all.sh` - **LEGACY**
**Purpose**: General macOS deployment (not App Store specific).
**Status**: ❌ Not needed for App Store
**What it does**: Creates DMG and general distribution packages
**Recommendation**: Use `build_appstore.sh` instead for App Store

#### `build_app.sh` - **LEGACY**
**Purpose**: General macOS app bundle creation with DMG.
**Status**: ❌ Not needed for App Store
**What it does**: Creates standard app bundle, DMG, and ZIP for direct distribution
**Recommendation**: Use `build_appstore.sh` instead for App Store

#### `prepare_appstore.sh` - **LEGACY**
**Purpose**: Old App Store package creation.
**Status**: ❌ Superseded by `submit_appstore.sh`
**What it does**: Creates unsigned package using CPack
**Recommendation**: Use `submit_appstore.sh` instead

#### `upload_appstore.sh` - **UTILITY**
**Purpose**: Simple upload-only script.
**Status**: ✅ Useful for upload-only workflows
**What it does**: Interactive password prompt and upload to App Store Connect
**When to use**: When you have a pre-signed package and only need to upload

#### `create_app_manual.sh` - **MINIMAL SETUP**
**Purpose**: Minimal browser automation for app creation.
**Status**: ❌ Unreliable
**Recommendation**: Use `register_app_complete.sh` instead

### 🗂️ Supporting Files

#### `appstore_api_helper.py` - **ADVANCED UTILITY**
**Purpose**: Python-based App Store Connect API integration.
**Status**: ⚠️ Optional (requires Python dependencies)
**What it does**:
- Lists apps via API
- Retrieves app information
- Manages API configuration

**Dependencies**: `pip3 install PyJWT requests`

#### `MouseCross.entitlements` - **REQUIRED**
**Purpose**: Defines app sandbox permissions for App Store.
**Contents**:
- `com.apple.security.app-sandbox`: Enables App Store sandbox
- `com.apple.security.files.user-selected.read-only`: File access permission

#### `CERTIFICATE_SETUP_GUIDE.md` - **DOCUMENTATION**
**Purpose**: Detailed certificate setup instructions.

## Complete App Store Submission Workflow

### First-Time Setup (One-Time Only)

1. **Create Certificates**:
   ```bash
   ./create_csrs.sh
   # Upload CSRs to Apple Developer Portal
   # Download certificates
   ./install_certificates.sh
   ```

2. **Create Provisioning Profile** (Required for TestFlight):
   ```bash
   ./create_provisioning_profile.sh
   # Follow instructions to create and download Mac App Store provisioning profile
   ```

3. **Create App Store Connect Record**:
   ```bash
   ./register_app_complete.sh
   ```

### Regular Submission Process

1. **Build for App Store**:
   ```bash
   ./build_appstore.sh
   ```

2. **Sign and Upload**:
   ```bash
   ./submit_appstore.sh
   ```

### Alternative Workflows

**Sign Only** (for testing):
```bash
./submit_appstore.sh --sign-only
```

**Upload Pre-Signed Package**:
```bash
./submit_appstore.sh --upload-only
# OR
./upload_appstore.sh
```

**Complete Interactive Setup**:
```bash
./setup_appstore.sh
```

## Script Dependencies and Requirements

### Required Tools
- Xcode Command Line Tools (`xcrun`, `codesign`, `productbuild`)
- Qt 6.9.0 installed at `/Users/stefan/Qt/6.9.0/macos`
- Python 3 (for icon generation)
- Active Apple Developer Program membership

### Required Certificates
- **3rd Party Mac Developer Application: Stefan Lohmaier (9KDRP4Q2D3)**
- **3rd Party Mac Developer Installer: Stefan Lohmaier (9KDRP4Q2D3)**

### Environment Variables
- `APP_SPECIFIC_PASSWORD`: Required for upload (can be prompted)

## File Outputs

### Build Outputs
- `../../dist/MouseCross.app` - App bundle (from `build_appstore.sh`)
- `MouseCross-0.1.0-appstore.pkg` - Unsigned package
- `MouseCross-0.1.0-signed.pkg` - Signed package ready for upload

### Certificate Files
- `MouseCross_App_Certificate.certSigningRequest`
- `MouseCross_Installer_Certificate.certSigningRequest`
- Downloaded `.cer` files from Apple

## Troubleshooting Scripts

| Issue | Script to Use | Purpose |
|-------|---------------|---------|
| Certificate problems | `diagnose_certificates.sh` | Check certificate status |
| Missing certificates | `create_csrs.sh` → `install_certificates.sh` | Create and install certs |
| App Store Connect issues | `register_app_complete.sh` | Re-create app record |
| General setup problems | `setup_appstore.sh` | Interactive troubleshooting |
| Upload failures | `upload_appstore.sh` | Simple upload retry |

## Scripts to Remove

The following scripts are **NOT REQUIRED** for App Store submission and can be removed:

### ❌ Remove These Scripts
- `auto_create_app.sh` - Unreliable browser automation
- `create_app_manual.sh` - Minimal functionality, superseded
- `build_all.sh` - Not App Store specific
- `build_app.sh` - Creates DMG/ZIP, not needed for App Store
- `prepare_appstore.sh` - Superseded by `submit_appstore.sh`

### ✅ Keep These Scripts
- `build_appstore.sh` - Core build process
- `submit_appstore.sh` - Core signing and upload
- `create_csrs.sh` - Certificate creation (one-time)
- `install_certificates.sh` - Certificate installation (one-time)
- `register_app_complete.sh` - App Store Connect setup (one-time)
- `setup_appstore.sh` - Complete setup wizard
- `diagnose_certificates.sh` - Troubleshooting
- `upload_appstore.sh` - Upload utility
- `appstore_api_helper.py` - API utility (optional)

## Summary

**Minimal Required Scripts** for App Store submission:
1. `build_appstore.sh` - Build with App Store configuration
2. `submit_appstore.sh` - Sign and upload
3. `register_app_complete.sh` - One-time App Store Connect setup
4. `create_csrs.sh` + `install_certificates.sh` - One-time certificate setup

**Optional but Useful**:
- `setup_appstore.sh` - Interactive setup wizard
- `upload_appstore.sh` - Upload-only utility
- `diagnose_certificates.sh` - Troubleshooting

The current script collection contains some redundancy and legacy scripts that can be cleaned up, but all core functionality for App Store submission is properly covered.