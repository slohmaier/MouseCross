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
```bash
cd deployment/macos
./build_all.sh
```
Creates a DMG installer and app bundle with proper code signing if certificates are available.

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