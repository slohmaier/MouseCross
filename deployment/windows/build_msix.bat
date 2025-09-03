@echo off
setlocal enabledelayedexpansion

echo Building MouseCross MSIX Package for Microsoft Store...
echo ======================================================

:: Set variables
set BUILD_DIR=..\..\build
set OUTPUT_DIR=..\..\dist
set MSIX_DIR=%OUTPUT_DIR%\MSIX
set MSIX_STAGING=%MSIX_DIR%\staging

:: Check for makeappx tool
where makeappx >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: makeappx tool not found. Please install Windows 10 SDK
    echo Download from: https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/
    exit /b 1
)

:: Clean and create directories
if exist "%MSIX_DIR%" rd /s /q "%MSIX_DIR%"
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"
mkdir "%MSIX_DIR%"
mkdir "%MSIX_STAGING%"

:: Build in Release mode
cd "%BUILD_DIR%"
cmake --build . --config Release
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)
cd ..

:: Copy executable to staging
copy "%BUILD_DIR%\Release\MouseCross.exe" "%MSIX_STAGING%\"

:: Find and copy Qt DLLs using windeployqt
where windeployqt >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Deploying Qt dependencies...
    windeployqt --release --no-opengl-sw --no-system-d3d-compiler --no-translations "%MSIX_STAGING%\MouseCross.exe"
) else (
    echo Warning: windeployqt not found. Please ensure Qt dependencies are included manually.
)

:: Copy manifest
copy "deployment\windows\Package.appxmanifest" "%MSIX_STAGING%\"

:: Create Images directory and placeholder icons
mkdir "%MSIX_STAGING%\Images"

:: Copy app icon to different required sizes (using same icon for all)
copy "resources\icons\app_icon.png" "%MSIX_STAGING%\Images\Square44x44Logo.png"
copy "resources\icons\app_icon.png" "%MSIX_STAGING%\Images\Square150x150Logo.png"
copy "resources\icons\app_icon.png" "%MSIX_STAGING%\Images\Wide310x150Logo.png"
copy "resources\icons\app_icon.png" "%MSIX_STAGING%\Images\StoreLogo.png"
copy "resources\icons\app_icon.png" "%MSIX_STAGING%\Images\SplashScreen.png"

:: Create MSIX package
cd "%MSIX_STAGING%"
makeappx pack /d . /p "..\MouseCross-v0.1.0.msix"
if errorlevel 1 (
    echo MSIX package creation failed!
    exit /b 1
)

echo.
echo MSIX package created: %MSIX_DIR%\MouseCross-v0.1.0.msix
echo.
echo For Microsoft Store submission:
echo 1. Sign the MSIX package with your code signing certificate
echo 2. Test the package with: Add-AppxPackage MouseCross-v0.1.0.msix
echo 3. Submit to Partner Center: https://partner.microsoft.com/
echo 4. Complete store listing with descriptions, screenshots, etc.
echo.
echo To sign the package (if you have a certificate):
echo signtool sign /fd SHA256 /a MouseCross-v0.1.0.msix
echo.
echo Done!