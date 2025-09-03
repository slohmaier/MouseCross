@echo off
setlocal enabledelayedexpansion

echo Building MouseCross MSI Installer for WinGet...
echo ===============================================

:: Set variables based on script location
set SCRIPT_DIR=%~dp0
set ROOT_DIR=%SCRIPT_DIR%..\..
set BUILD_DIR=%ROOT_DIR%\build
set OUTPUT_DIR=%ROOT_DIR%\dist
set MSI_DIR=%OUTPUT_DIR%\MSI

:: Check for WiX Toolset
where candle >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: WiX Toolset not found. Please install WiX Toolset v3.x
    echo Download from: https://github.com/wixtoolset/wix3/releases
    echo Make sure candle.exe and light.exe are in your PATH
    exit /b 1
)

:: Clean and create directories
if exist "%MSI_DIR%" rd /s /q "%MSI_DIR%"
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"
mkdir "%MSI_DIR%"

:: Build in Release mode (icons generated automatically by CMake)
cd "%BUILD_DIR%"
cmake --build . --config Release
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)
cd ..

:: Create MSI using CPack WiX generator (Qt dependencies deployed by CMake)
cd "%BUILD_DIR%"
cpack -G WIX
if errorlevel 1 (
    echo MSI creation failed!
    exit /b 1
)

:: Move MSI to dist/MSI directory
move *.msi "..\%MSI_DIR%\"

echo.
echo MSI installer created in: %MSI_DIR%\
echo.
echo For WinGet submission:
echo 1. Test the MSI installer thoroughly
echo 2. Submit to winget-pkgs repository: https://github.com/microsoft/winget-pkgs
echo 3. Follow WinGet manifest creation guidelines
echo 4. Create manifests in format: manifests/s/slohmaier/MouseCross/0.1.0/
echo.
echo Done!