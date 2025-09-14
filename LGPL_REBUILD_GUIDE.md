# How to Rebuild MouseCross - LGPL Compliance Guide

MouseCross is free and open source software licensed under the GNU Lesser General Public License (LGPL) version 3.0 or later. This guide explains how users can rebuild MouseCross from source code to comply with LGPL requirements.

## What is LGPL Compliance?

The LGPL (Lesser General Public License) ensures that users have the right to:
- **Access source code** of the software
- **Modify the software** according to their needs
- **Rebuild the software** from the provided source code
- **Use their own version** instead of the distributed binary

This guide fulfills the LGPL requirement to provide clear instructions for rebuilding the software.

## Prerequisites

### For macOS:
- **Xcode Command Line Tools**: Install with `xcode-select --install`
- **Qt 6.9.0 or later**: Download from [Qt.io](https://www.qt.io/download-open-source) or install via Homebrew
- **CMake 3.16 or later**: Install via Homebrew with `brew install cmake`
- **Git**: Usually pre-installed or available via `brew install git`

### For Windows:
- **Visual Studio 2022** with C++ workload
- **Qt 6.5.0 or later**: Download from [Qt.io](https://www.qt.io/download-open-source)
- **CMake 3.16 or later**: Download from [cmake.org](https://cmake.org/download/)
- **Git**: Download from [git-scm.com](https://git-scm.com/)

### For Linux:
- **GCC or Clang compiler**: Install via package manager
- **Qt 6 development packages**: Install via package manager (e.g., `qt6-base-dev`)
- **CMake**: Install via package manager
- **Git**: Install via package manager

## Step-by-Step Rebuild Instructions

### 1. Obtain the Source Code

MouseCross source code is available on GitHub:

```bash
git clone https://github.com/slohmaier/MouseCross.git
cd MouseCross
```

**Alternative**: Download the source code as a ZIP file from the GitHub repository and extract it.

### 2. Platform-Specific Build Instructions

#### macOS Build

##### Regular Build (for direct distribution):
```bash
# Create build directory
mkdir build
cd build

# Configure the build
cmake -DCMAKE_PREFIX_PATH="/Users/$(whoami)/Qt/6.9.0/macos" ..
# Or for Homebrew Qt: cmake -DCMAKE_PREFIX_PATH="/opt/homebrew/opt/qt@6" ..

# Build the application
cmake --build . --config Release

# The app bundle will be created at: build/MouseCross.app
```

##### App Store Compliant Build:
```bash
# Use the dedicated App Store build script
cd deployment/macos
./build_appstore.sh

# The bundled app will be created at: dist/MouseCross.app
```

#### Windows Build

##### x64 Build:
```bash
# Create build directory
mkdir build
cd build

# Configure for x64
cmake -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2022_64" ..

# Build the application
cmake --build . --config Release

# The executable will be at: build\Release\MouseCross.exe
```

##### ARM64 Build:
```bash
# Create build directory
mkdir build
cd build

# Configure for ARM64
cmake -A ARM64 -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2022_arm64" ..

# Build the application
cmake --build . --config Release

# The executable will be at: build\Release\MouseCross.exe
```

##### Windows Deployment Packages:
MouseCross provides several Windows deployment options:

```bash
# Portable ZIP (no installation required)
cd deployment\windows
build_portable.bat

# MSI Installer (for WinGet and enterprise)
build_msi.bat

# MSIX Package (for Microsoft Store)
build_msix.bat

# Simple build (basic executable with dependencies)
build_simple.bat
```

#### Linux Build

```bash
# Install dependencies (Ubuntu/Debian example)
sudo apt update
sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential git

# Create build directory
mkdir build
cd build

# Configure the build
cmake ..

# Build the application
cmake --build . --config Release

# The executable will be at: build/MouseCross
```

## Build Verification

After building, you can verify your build works correctly:

### macOS:
```bash
# Test the app bundle
./build/MouseCross.app/Contents/MacOS/MouseCross

# Or for App Store build:
./dist/MouseCross.app/Contents/MacOS/MouseCross
```

### Windows:
```bash
# Test the executable
.\build\Release\MouseCross.exe
```

### Linux:
```bash
# Test the executable
./build/MouseCross
```

## Customization Options

The LGPL license allows you to modify MouseCross. Here are some common customization points:

### Build Configuration Options:
- **APPSTORE_BUILD=ON**: Enable App Store-specific features (macOS only)
- **CMAKE_BUILD_TYPE=Debug**: Build with debug information
- **CMAKE_BUILD_TYPE=Release**: Build optimized release version

### Source Code Modifications:
All source code is located in the `src/` directory:
- **User interface**: `*Dialog.cpp` files
- **Crosshair rendering**: `CrosshairRenderer.cpp`, `CrosshairOverlay.cpp`
- **Settings management**: `SettingsManager.cpp`
- **System integration**: `AutoStart.cpp`

### Icon and Resource Customization:
- **Application icon**: `resources/icons/app_icon.svg`
- **UI resources**: `resources/resources.qrc`
- **Translations**: `translations/` directory

## License Compliance Notes

### Your Rights Under LGPL:
- **Use freely**: Use MouseCross for any purpose, including commercial use
- **Modify freely**: Make any changes you need to the source code
- **Redistribute**: Share your modified versions under the same LGPL license
- **Link with proprietary code**: LGPL allows linking with proprietary applications

### Your Obligations:
- **Preserve license notices**: Keep all copyright and license headers in source files
- **Provide source access**: If you distribute MouseCross, provide access to the source code
- **License derivatives**: Modifications to MouseCross itself must remain under LGPL
- **Document changes**: Clearly mark any changes you make to the original code

### Third-Party Components:
MouseCross uses the following third-party components:
- **Qt Framework**: Licensed under LGPL v3.0 - source available at [Qt.io](https://www.qt.io/)
- **System APIs**: Uses standard macOS, Windows, and Linux system libraries

## Support and Community

### Getting Help:
- **Documentation**: Complete build and usage documentation is available in `CLAUDE.md`
- **Issues**: Report problems at [GitHub Issues](https://github.com/slohmaier/MouseCross/issues)
- **Website**: Visit [slohmaier.de/mousecross](https://slohmaier.de/mousecross) for more information

### Contributing:
MouseCross welcomes contributions under the LGPL license:
- **Fork the repository** on GitHub
- **Make your improvements** in a feature branch
- **Submit a pull request** with your changes
- All contributions will be licensed under LGPL v3.0 or later

## Troubleshooting Common Build Issues

### Qt Not Found:
- Verify Qt installation path in CMAKE_PREFIX_PATH
- Ensure Qt version 6.0 or later is installed
- Check that Qt development packages are installed (Linux)

### CMake Configuration Errors:
- Update CMake to version 3.16 or later
- Clear build directory and reconfigure: `rm -rf build && mkdir build`
- Verify all dependencies are installed

### Compiler Errors:
- Ensure C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2019+)
- Install platform-specific development tools
- Check that all source files have proper license headers

## License Information

**MouseCross** is licensed under the **GNU Lesser General Public License v3.0 or later**.

**Copyright**: © 2025 Stefan Lohmaier <stefan@slohmaier.de>

**License Text**: The complete LGPL v3.0 license is available in the `LICENSE` file included with the source code, or online at [gnu.org/licenses/lgpl-3.0.html](https://www.gnu.org/licenses/lgpl-3.0.html).

**Website**: [slohmaier.de/mousecross](https://slohmaier.de/mousecross)

---

*This rebuild guide ensures full compliance with LGPL requirements by providing users the means to obtain, build, modify, and redistribute MouseCross according to the terms of the license.*