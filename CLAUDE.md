# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

MouseCross is a crossplatform Qt C++ application designed to help visually impaired users locate their mouse cursor using a customizable crosshair overlay. The application runs from the system tray and provides comprehensive settings for appearance and behavior customization, including selectable direction shapes (Circle, Arrow, Cross, Raute/Diamond) that scale progressively from center to screen edges.

## Build Commands

### Development Build
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

### Release Build
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Platform-Specific Qt Paths
- Windows: `cmake -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2022_64" ..`
- macOS (Homebrew): `cmake -DCMAKE_PREFIX_PATH="/opt/homebrew/opt/qt@6" ..`

### Icon Generation
```bash
python create_icon.py
```
Generates app_icon.png and app_icon.ico from the programmatic icon design. Icons are automatically generated during CMake build if Python is available.

### Running
- Windows: `.\build\Debug\MouseCross.exe` or `.\build\Release\MouseCross.exe`
- macOS: `./build/MouseCross.app/Contents/MacOS/MouseCross`
- Linux: `./build/MouseCross`

## Deployment

### Windows Deployment

#### Prerequisites
- Qt 6.5.0 or later installed at `C:\Qt\6.5.0\msvc2022_64`
- Visual Studio 2022 with MSVC compiler
- WiX Toolset v3.14 (for MSI creation): Download from https://github.com/wixtoolset/wix3/releases
- Windows 10 SDK (for MSIX creation, optional)
- Python (for icon generation, optional)

#### Complete Deployment Pipeline
Run the all-in-one deployment script that creates all Windows packages:
```bash
cd deployment
deploy_all_windows.bat
```

This creates three deployment packages in `dist/`:
1. **Portable ZIP** (`MouseCross-v0.1.0-Windows-Portable.zip`): No installation required
2. **MSI Installer** (`MSI/MouseCross-0.1.0-win64.msi`): For WinGet and enterprise deployment
3. **MSIX Package** (`MSIX/MouseCross-v0.1.0.msix`): For Microsoft Store submission

#### Individual Package Creation

##### Portable ZIP Package
```bash
cd deployment\windows
build_portable.bat
```
Creates a self-contained ZIP with all Qt dependencies included.

##### MSI Installer (WinGet)
```bash
cd deployment\windows
build_msi.bat
```
Creates an MSI installer suitable for WinGet submission. Requires WiX Toolset.

##### MSIX Package (Microsoft Store)
```bash
cd deployment\windows
build_msix.bat
```
Creates an MSIX package for Microsoft Store. Requires Windows 10 SDK.

#### Deployment Notes
- The build scripts automatically handle Qt dependency deployment using CMake's built-in deployment tools
- Icons are generated automatically during the build process if Python is available
- All packages are created in the `dist/` directory at the project root
- The deployment scripts will stop any running MouseCross processes before building
- For code signing, sign the packages after creation with your certificate

#### Distribution Channels
- **Direct Distribution**: Share the portable ZIP file
- **WinGet**: Submit MSI to [winget-pkgs repository](https://github.com/microsoft/winget-pkgs)
- **Microsoft Store**: Upload MSIX to Partner Center

### macOS Deployment

#### Prerequisites
- Qt 6.9.0 or later installed (Homebrew: `/opt/homebrew/opt/qt@6`)
- Xcode Command Line Tools
- Python (for icon generation, optional)
- macOS Developer Account (for App Store submission)
- Code signing certificates (for distribution)

#### Complete macOS Deployment Pipeline
```bash
cd deployment/macos
./build_all.sh
```
Creates DMG installer, app bundle, and App Store package.

#### Individual Package Creation

##### Standard App Bundle + DMG
```bash
cd deployment/macos
./build_app.sh
```
Creates `MouseCross.app` bundle and DMG for direct distribution.

##### App Store Package (Static Qt - Required for App Store)
```bash
# Use dedicated App Store build script with static Qt linking
cd deployment/macos
./build_appstore.sh
```

##### Manual App Store Static Build
```bash
# Clean and configure for static linking
rm -rf build dist
mkdir build && cd build

# Configure with App Store static linking
cmake -DAPPSTORE_BUILD=ON \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="/Users/stefan/Qt/6.9.0/macos" \
      ..

# Build with static Qt
cmake --build . --config Release --parallel 8

# Create package
mkdir -p ../dist
cp -R MouseCross.app ../dist/
productbuild --component MouseCross.app /Applications MouseCross-0.1.0-static.pkg
mv MouseCross-0.1.0-static.pkg ../dist/
```

##### Development Package (Dynamic Qt - Not for App Store)
```bash
mkdir -p dist
cp -R build/MouseCross.app dist/
cd build
productbuild --component MouseCross.app /Applications MouseCross-0.1.0.pkg
mv MouseCross-0.1.0.pkg ../dist/
```

#### macOS App Store Metadata Requirements
The `Info.plist.in` contains all required App Store metadata:
- **Bundle ID**: `de.slohmaier.mousecross`
- **App Category**: `public.app-category.utilities`
- **Copyright**: `© 2025 Stefan Lohmaier. Licensed under LGPL-3.0-or-later.`
- **Privacy Usage**: Accessibility permissions for mouse tracking
- **Encryption**: `ITSAppUsesNonExemptEncryption = false`
- **Background App**: `LSUIElement = true` (no dock icon)
- **Minimum macOS**: 10.14 (Mojave)

#### Qt Bundled Builds for App Store
**App Store builds use fully bundled Qt frameworks for LGPL compliance:**

##### Why Bundled Qt is Required
- **LGPL Compliance**: App Store bundling satisfies LGPL requirements for closed distribution
- **Self-Contained**: All Qt dependencies embedded within app bundle
- **App Store Policy**: Simplified packaging without external dependencies

##### App Store Build Configuration
The `APPSTORE_BUILD` CMake option enables:
- Enhanced macOS framework linking
- Proper bundle configuration for App Store
- Self-contained app package with embedded Qt frameworks
- macdeployqt automatic Qt bundling

##### Verification Commands
```bash
# Verify Qt bundling (shows @rpath for bundled Qt frameworks)
otool -L dist/MouseCross.app/Contents/MacOS/MouseCross

# Check bundled Qt frameworks
ls -la dist/MouseCross.app/Contents/Frameworks/

# Test the bundled app
./dist/MouseCross.app/Contents/MacOS/MouseCross
```

##### Build Types
- **App Store Build** (`APPSTORE_BUILD=ON`): For App Store submission with bundled Qt
- **Development Build** (default): For development and direct distribution

#### macOS Deployment Notes
- **App Store builds**: Use static Qt linking (required for LGPL compliance)
- **Development builds**: Use dynamic Qt with `macdeployqt` bundling
- Icons are automatically generated during build if Python is available
- The app runs as a background utility (no dock icon)
- Requires accessibility permissions for mouse tracking
- All packages are created in the `dist/` directory

#### macOS Distribution Channels
- **Direct Distribution**: Share DMG file
- **Mac App Store**: Upload PKG to App Store Connect
- **Developer ID**: Sign with Developer ID for notarization

#### Publishing to Mac App Store

##### Prerequisites
1. **Apple Developer Account**: Active paid membership ($99/year)
2. **Certificates**: Mac App Store distribution certificate
3. **App Store Connect**: App record created
4. **Bundle ID**: `de.slohmaier.mousecross` registered in developer portal

##### Step-by-Step Publishing Process

###### 1. Create App Store Connect Record
1. Log into [App Store Connect](https://appstoreconnect.apple.com)
2. Go to "My Apps" → "+" → "New App"
3. Fill in app information:
   - **Name**: MouseCross
   - **Primary Language**: English
   - **Bundle ID**: `de.slohmaier.mousecross`
   - **SKU**: `mousecross-macos-v1`
   - **User Access**: Full Access

###### 2. Configure App Information
**App Information Tab:**
- **Name**: MouseCross
- **Subtitle**: Visual Mouse Locator for Accessibility
- **Category**: Primary: Utilities
- **Content Rights**: Does not contain, show, or access third-party content

**Pricing and Availability:**
- **Price**: Free
- **Availability**: All territories
- **App Store Distribution**: Available

**App Privacy:**
- **Privacy Policy URL**: https://slohmaier.de/mousecross/privacy
- **Data Collection**: None (offline utility app)

###### 3. Version Information
**Version 0.1.0 Information:**
- **What's New**: First release of MouseCross - a crosshair overlay application designed to help visually impaired users locate their mouse cursor
- **Description**:
  ```
  MouseCross provides a customizable crosshair overlay to help visually impaired users with tunnel vision locate their mouse cursor on the screen.

  Features:
  • Customizable crosshair appearance (color, size, opacity)
  • Multiple direction shapes (Circle, Arrow, Cross, Diamond)
  • Inverted mode for visibility on any background
  • System tray integration for non-intrusive operation
  • Keyboard shortcuts for easy toggling
  • Auto-start capability
  • Accessibility-focused design

  MouseCross runs quietly in the background and only appears when needed, ensuring it doesn't interfere with your normal computer usage.
  ```
- **Keywords**: accessibility, mouse, cursor, crosshair, vision, overlay, utility
- **Support URL**: https://slohmaier.de/mousecross
- **Marketing URL**: https://slohmaier.de/mousecross

###### 4. Build Upload and Submission

**Create Signed Package:**
```bash
# 1. Build the app bundle
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH="/opt/homebrew/opt/qt@6" ..
cmake --build . --config Release
cd ..

# 2. Copy to dist directory
mkdir -p dist
cp -R build/MouseCross.app dist/

# 3. Create App Store package (unsigned for testing)
cd build
productbuild --component MouseCross.app /Applications MouseCross-0.1.0-unsigned.pkg

# 4. Sign for App Store submission (requires certificates)
productsign --sign "3rd Party Mac Developer Installer: Stefan Lohmaier" \
  MouseCross-0.1.0-unsigned.pkg \
  MouseCross-0.1.0.pkg

# 5. Upload to App Store Connect
xcrun altool --upload-app --type osx --file MouseCross-0.1.0.pkg \
  --username "your-apple-id@email.com" \
  --password "your-app-specific-password"
```

**Alternative Upload Methods:**
- **Transporter App**: Download from Mac App Store, drag and drop PKG file
- **Xcode**: Use Organizer window to upload builds

###### 5. App Review Information
**App Review Information:**
- **Contact Information**:
  - First Name: Stefan
  - Last Name: Lohmaier
  - Phone: [Your phone number]
  - Email: stefan@slohmaier.de
- **Demo Account**: Not required (utility app)
- **Notes**:
  ```
  MouseCross is an accessibility utility that displays a crosshair overlay to help visually impaired users locate their mouse cursor.

  The app requires accessibility permissions to track mouse position, which is essential for its core functionality.

  The app runs as a background utility (LSUIElement = true) and appears in the system tray for easy access to settings.
  ```

**Age Rating:**
- 4+ (No objectionable content)

###### 6. Screenshots and Metadata
**Required Screenshots (macOS):**
- 1280x800 pixels minimum
- Show app in action with crosshair overlay
- Include settings dialog screenshot
- Show system tray integration

**App Preview Video** (Optional but recommended):
- 30 seconds maximum
- Demonstrate crosshair overlay functionality
- Show settings customization

###### 7. Submit for Review
1. **Review Build**: Select uploaded build in App Store Connect
2. **Complete All Sections**: Ensure all required information is filled
3. **Submit for Review**: Click "Submit for Review" button
4. **Review Process**: Typically 1-7 days
5. **Release**: Choose automatic or manual release after approval

##### Common App Store Rejection Issues

**Accessibility Permissions:**
- Clearly explain why accessibility permissions are needed
- Include privacy policy explaining data usage (none in this case)

**Background App Behavior:**
- Document LSUIElement = true in app description
- Explain system tray integration purpose

**Metadata Consistency:**
- Ensure bundle ID matches exactly: `de.slohmaier.mousecross`
- Version numbers must match between Xcode and App Store Connect
- App name must be consistent across all platforms

##### Post-Approval Checklist
- [ ] Test downloaded app from App Store
- [ ] Verify all functionality works after App Store installation
- [ ] Monitor app reviews and ratings
- [ ] Prepare release notes for future updates
- [ ] Update website with App Store availability

##### App Store Connect URLs
- **App Store Connect**: https://appstoreconnect.apple.com
- **Developer Portal**: https://developer.apple.com/account
- **App Store Guidelines**: https://developer.apple.com/app-store/review/guidelines/

## License and Compliance

### LGPL-3.0-or-later License
MouseCross is licensed under the GNU Lesser General Public License v3.0 or later. This ensures the software remains free and open source while allowing linking with proprietary applications.

#### License Requirements
- All source files include LGPL header with copyright notice
- LICENSE file contains complete LGPL-3.0 text
- About dialog displays license information and website
- CMakeLists.txt configured with proper license metadata

#### Adding License Headers
Use the following script to add LGPL headers to new source files:
```bash
./add_license_headers.sh
```

#### Copyright Information
- **Copyright**: © 2025 Stefan Lohmaier <stefan@slohmaier.de>
- **License**: LGPL-3.0-or-later
- **Website**: https://slohmaier.de/mousecross
- **Contact**: stefan@slohmaier.de

#### Third-Party Dependencies
- **Qt6**: Licensed under LGPL-3.0 (compatible)
- **macOS Frameworks**: System libraries (permitted)
- **Windows APIs**: System libraries (permitted)

### Compliance Commands
```bash
# Commit license compliance changes
git add .
git commit -m "Implement LGPL-3.0 compliance with headers and metadata"

# Verify all source files have license headers
grep -r "Copyright.*Stefan Lohmaier" src/
```

## Architecture Overview

### Core Components
- **MouseCrossApp**: Main application class managing system tray, dialogs, and crosshair lifecycle
- **CrosshairOverlay**: Transparent overlay widget that follows mouse cursor and renders crosshair
- **SettingsManager**: Persistent settings management using QSettings
- **AutoStart**: Cross-platform auto-start functionality for Windows/macOS/Linux

### Dialog System
- **WelcomeDialog**: First-run welcome screen with feature overview
- **SettingsDialog**: Comprehensive settings interface for all customization options
- **AboutDialog**: Application information and version display

### Key Design Patterns
- **System Tray Integration**: Application runs headlessly from system tray with context menu
- **Transparent Overlay**: CrosshairOverlay uses transparent window flags and click-through functionality
- **Platform Abstraction**: AutoStart class handles OS-specific startup integration
- **Signal-Slot Architecture**: Settings changes trigger crosshair updates via Qt signals

### Crosshair Rendering
The CrosshairOverlay widget creates a fullscreen transparent window that:
- Follows mouse cursor position with high-frequency updates (60fps)
- Renders crosshair using QPainter with customizable appearance
- Supports inverted mode using CompositionMode_Difference for visibility on any background
- Implements click-through functionality to avoid interfering with normal mouse operations

### Settings Architecture
Settings are organized hierarchically:
- `crosshair/`: Appearance settings (color, size, opacity, etc.)
- `behavior/`: Application behavior (auto-start, activate on start)
- `hotkey/`: Keyboard shortcuts for toggle functionality

### Cross-Platform Considerations
- **Windows**: Uses Windows registry for auto-start, WS_EX_LAYERED for click-through
- **macOS**: Uses LaunchAgents plist files for auto-start, LSUIElement for background app
- **Linux**: Uses autostart desktop entries, X11 window properties for click-through

## File Structure

- `src/main.cpp`: Application entry point
- `src/MouseCrossApp.*`: Main application controller
- `src/CrosshairOverlay.*`: Crosshair rendering and mouse tracking
- `src/SettingsManager.*`: Persistent configuration management
- `src/SettingsDialog.*`: Settings UI with all customization options
- `src/WelcomeDialog.*`: First-run user introduction
- `src/AboutDialog.*`: Application information display
- `src/AutoStart.*`: Cross-platform system startup integration
- `resources/`: Application icons and Qt resource files
- `CMakeLists.txt`: Build system configuration with Qt6 integration
- `Info.plist.in`: macOS bundle configuration template

## Development Notes

### Qt Framework Usage
- **Qt6**: Minimum required version for modern cross-platform support
- **Core Modules**: QtCore, QtWidgets for GUI components
- **System Integration**: QSystemTrayIcon, QSettings for platform integration
- **Resource System**: Qt resource files for embedded icons

### Platform-Specific Features
- Windows: Registry-based auto-start, native window layering
- macOS: LaunchAgent integration, bundle configuration
- Linux: XDG autostart, desktop entry standards

### UI/UX Design
- System tray-first approach for non-intrusive background operation
- Welcome dialog guides first-time users through feature overview
- Comprehensive settings dialog with live preview capabilities
- Inverted crosshair mode ensures visibility on any background color

## Build Troubleshooting

### Common Build Issues

#### Qt Path Issues
- **macOS Homebrew**: Set `CMAKE_PREFIX_PATH="/opt/homebrew/opt/qt@6"`
- **macOS Manual**: Set `CMAKE_PREFIX_PATH="/Users/$USER/Qt/6.9.0/macos"`
- **Windows**: Set `CMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2022_64"`

#### macOS Deployment Issues
- **macdeployqt not found**: Ensure Qt bin directory is in PATH
- **Code signing errors**: Use unsigned builds for testing
- **Permission denied**: Run `chmod +x deployment/macos/*.sh`

#### Windows Deployment Issues
- **MSVC not found**: Install Visual Studio 2022 with C++ workload
- **WiX Toolset missing**: Download from GitHub releases
- **Qt DLL issues**: Use `windeployqt` for dependency resolution

### Testing Commands
```bash
# Test app bundle directly
./build/MouseCross.app/Contents/MacOS/MouseCross

# Test Windows executable
.\build\Release\MouseCross.exe

# Verify license headers in all source files
find src -name "*.cpp" -o -name "*.h" | xargs grep -L "Copyright.*Stefan Lohmaier"

# Check app bundle structure
ls -la build/MouseCross.app/Contents/
cat build/MouseCross.app/Contents/Info.plist
```

### Repository Cleanup
**Keep repository clean by removing build artifacts:**

```bash
# Clean all build artifacts
rm -rf build dist deployment/MouseCross.app

# Clean only build directory (keeps dist for distribution)
rm -rf build

# Git clean (removes untracked files)
git clean -fd
```

### Build Type Selection
```bash
# App Store build (static Qt) - Required for App Store submission
cmake -DAPPSTORE_BUILD=ON ..

# Development build (dynamic Qt) - Default for testing
cmake ..
```

### Deployment Verification
```bash
# Check macOS app bundle completeness
otool -L build/MouseCross.app/Contents/MacOS/MouseCross
codesign -dv --verbose=4 build/MouseCross.app

# Verify App Store package
pkgutil --payload-files MouseCross-0.1.0.pkg
```

## Performance Optimization

### Build Performance
- Use `ninja` generator: `cmake -G Ninja ..`
- Parallel builds: `cmake --build . --parallel 8`
- ccache for incremental builds

### Runtime Performance
- Crosshair rendering optimized to 60fps
- Mouse tracking uses native OS APIs for minimal latency
- System tray integration avoids main thread blocking