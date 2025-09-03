# MouseCross Deployment Guide

This directory contains all the necessary scripts and configurations for deploying MouseCross across different platforms and distribution channels.

## Overview

MouseCross supports the following deployment methods:

### macOS
- **Standalone .app Bundle** - For direct distribution
- **App Store Package** - For Mac App Store submission

### Windows
- **Portable ZIP** - No installation required
- **MSI Installer** - For WinGet and enterprise distribution
- **MSIX Package** - For Microsoft Store submission

## Prerequisites

### General
- CMake 3.16 or later
- Qt6 with Core, Widgets, and Network components
- Python 3.x (for icon generation)

### macOS Specific
- Xcode Command Line Tools
- macOS Developer ID certificates (for App Store)

### Windows Specific
- Visual Studio Build Tools or Visual Studio
- Qt6 with windeployqt tool
- WiX Toolset v3.x (for MSI)
- Windows 10 SDK (for MSIX)

## Deployment Instructions

### Build the Project First

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build . --config Release
```

### macOS Deployment

#### 1. Create .app Bundle
```bash
cd deployment/macos
chmod +x build_app.sh
./build_app.sh
```

#### 2. Prepare for App Store
```bash
cd deployment/macos
chmod +x prepare_appstore.sh
./prepare_appstore.sh
```

### Windows Deployment

#### 1. Create Portable ZIP
```batch
cd deployment\windows
build_portable.bat
```

#### 2. Create MSI Installer
```batch
cd deployment\windows
build_msi.bat
```

#### 3. Create MSIX Package
```batch
cd deployment\windows
build_msix.bat
```

## Distribution Channels

### Direct Distribution
- **macOS**: Distribute the .app bundle or DMG
- **Windows**: Distribute the portable ZIP or MSI installer

### App Stores
- **Mac App Store**: Use the generated .pkg file
- **Microsoft Store**: Use the generated MSIX file

### Package Managers
- **WinGet**: Submit MSI to [winget-pkgs repository](https://github.com/microsoft/winget-pkgs)
- **Homebrew** (macOS): Create formula for the .app bundle

## File Structure

```
deployment/
├── README.md                     # This file
├── macos/
│   ├── build_app.sh             # Creates .app bundle
│   └── prepare_appstore.sh      # Prepares App Store package
└── windows/
    ├── build_portable.bat       # Creates portable ZIP
    ├── build_msi.bat            # Creates MSI installer
    ├── build_msix.bat           # Creates MSIX package
    ├── license.rtf              # License for MSI installer
    └── Package.appxmanifest     # MSIX manifest
```

## Output Directory Structure

After running the deployment scripts, you'll find the packages in:

```
dist/
├── MouseCross.app                           # macOS app bundle
├── MouseCross.pkg                           # macOS App Store package
├── MouseCross-v0.1.0-Windows-Portable.zip  # Windows portable
├── MSI/
│   └── MouseCross-0.1.0-win64.msi         # Windows MSI installer
└── MSIX/
    └── MouseCross-v0.1.0.msix             # Windows Store package
```

## Code Signing

### macOS
```bash
# Sign the app bundle
codesign --deep --force --verify --verbose --sign "Developer ID Application: Your Name" MouseCross.app

# Sign the installer package
productsign --sign "Developer ID Installer: Your Name" MouseCross.pkg MouseCross-signed.pkg
```

### Windows
```batch
# Sign the MSI
signtool sign /fd SHA256 /a MouseCross-0.1.0-win64.msi

# Sign the MSIX
signtool sign /fd SHA256 /a MouseCross-v0.1.0.msix
```

## Troubleshooting

### Common Issues

1. **Qt dependencies not found**
   - Ensure Qt6 is properly installed
   - Make sure windeployqt (Windows) is in PATH

2. **Build fails**
   - Verify CMake version is 3.16+
   - Check that all Qt6 components are installed

3. **Code signing fails**
   - Verify certificates are installed correctly
   - Check certificate validity dates

4. **WiX tools not found**
   - Install WiX Toolset v3.x from GitHub releases
   - Add WiX tools to system PATH

### Platform-Specific Notes

#### macOS
- Requires macOS 10.14+ (as specified in Info.plist.in)
- App Store submission requires proper provisioning profiles
- Notarization required for distribution outside App Store

#### Windows
- MSIX packages require Windows 10 version 1809+
- Microsoft Store requires Partner Center account
- WinGet submissions go through community review process

## Support

For issues related to deployment, please check:
1. Build logs for specific error messages
2. Platform-specific requirements above
3. Official documentation for each distribution channel