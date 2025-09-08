@echo off
setlocal enabledelayedexpansion

echo MouseCross - Complete Windows Deployment Pipeline (Multi-Architecture)
echo ======================================================================
echo This script will create deployment packages for available architectures:
echo - x64 (msvc2022_64) if available
echo - ARM64 (msvc2022_arm64) if available
echo.
echo Packages created:
echo - Portable ZIP (no installation required)
echo - MSI Installer (for WinGet and enterprise)
echo - MSIX Package (for Microsoft Store)
echo ======================================================================
echo.

:: Set directories based on script location
set SCRIPT_DIR=%~dp0
set ROOT_DIR=%SCRIPT_DIR%..
set DEPLOYMENT_DIR=%SCRIPT_DIR%windows

:: Add CMake to PATH if available in Qt Tools
if exist "C:\Qt\Tools\CMake_64\bin\cmake.exe" (
    set "PATH=C:\Qt\Tools\CMake_64\bin;%PATH%"
    echo [DETECT] Found CMake at C:\Qt\Tools\CMake_64
)

:: Detect available Qt architectures
set QT_VERSION=6.9.2
set QT_BASE=C:\Qt\%QT_VERSION%
set BUILD_X64=0
set BUILD_ARM64=0

if exist "%QT_BASE%\msvc2022_64" (
    echo [DETECT] Found x64 Qt at %QT_BASE%\msvc2022_64
    set BUILD_X64=1
)

if exist "%QT_BASE%\msvc2022_arm64" (
    echo [DETECT] Found ARM64 Qt at %QT_BASE%\msvc2022_arm64
    set BUILD_ARM64=1
)

if %BUILD_X64%==0 if %BUILD_ARM64%==0 (
    echo ERROR: No Qt architectures found in %QT_BASE%
    echo Please ensure Qt %QT_VERSION% is installed with at least one architecture.
    pause
    exit /b 1
)

echo.

:: Kill any running MouseCross processes
echo [PREP] Stopping any running MouseCross processes...
taskkill /f /im MouseCross.exe >nul 2>&1 || echo No MouseCross processes found

:: Clean previous builds
echo [PREP] Cleaning previous deployment files...
if exist "%ROOT_DIR%\dist" rmdir /s /q "%ROOT_DIR%\dist" >nul 2>&1

:: Verify WiX installation
echo [PREP] Checking WiX Toolset installation...
set "WIX_PATH=C:\Program Files\WiX Toolset v6.0"
if exist "%WIX_PATH%\bin\wix.exe" (
    echo Found WiX Toolset v6.0 at %WIX_PATH%
) else (
    where wix >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        echo ERROR: WiX Toolset not found. Please install WiX Toolset v6.0
        echo Download from: https://wixtoolset.org/
        pause
        exit /b 1
    )
    echo Found WiX Toolset in PATH
)

:: Verify Windows SDK (for makeappx)
echo [PREP] Checking Windows SDK installation...
where makeappx >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo WARNING: makeappx not found in PATH. MSIX creation may fail.
    echo Please install Windows 10 SDK if needed.
    echo Continuing with other deployment methods...
    echo.
)

echo.
echo ======================================================================
echo Starting deployment process...
echo ======================================================================
echo.

:: Build for x64 if available
if %BUILD_X64%==1 (
    echo ======================================================================
    echo Building x64 packages...
    echo ======================================================================
    
    set ARCH=x64
    set QT_PATH=%QT_BASE%\msvc2022_64
    
    :: 1. Create Portable ZIP
    echo [x64 1/3] Creating Portable ZIP package...
    echo ----------------------------------------
    cd /d "%DEPLOYMENT_DIR%"
    call build_portable.bat !ARCH! !QT_PATH!
    if errorlevel 1 (
        echo ERROR: x64 Portable ZIP creation failed!
        pause
        exit /b 1
    )
    echo [x64 1/3] ✓ Portable ZIP created successfully
    echo.
    
    :: 2. Create MSI Installer
    echo [x64 2/3] Creating MSI installer package...
    echo ----------------------------------------
    call build_msi.bat !ARCH! !QT_PATH!
    if errorlevel 1 (
        echo ERROR: x64 MSI creation failed!
        pause
        exit /b 1
    )
    echo [x64 2/3] ✓ MSI installer created successfully
    echo.
    
    :: 3. Create MSIX Package
    echo [x64 3/3] Creating MSIX package...
    echo ----------------------------------------
    call build_msix.bat !ARCH! !QT_PATH!
    if errorlevel 1 (
        echo ERROR: x64 MSIX creation failed!
        echo This is non-critical if you don't need Microsoft Store deployment.
        echo Continuing...
    )
    echo [x64 3/3] ✓ MSIX package created successfully
    echo.
)

:: Build for ARM64 if available
if %BUILD_ARM64%==1 (
    echo ======================================================================
    echo Building ARM64 packages...
    echo ======================================================================
    
    set ARCH=arm64
    set QT_PATH=%QT_BASE%\msvc2022_arm64
    
    :: 1. Create Portable ZIP
    echo [ARM64 1/3] Creating Portable ZIP package...
    echo ----------------------------------------
    cd /d "%DEPLOYMENT_DIR%"
    call build_portable.bat !ARCH! !QT_PATH!
    if errorlevel 1 (
        echo ERROR: ARM64 Portable ZIP creation failed!
        pause
        exit /b 1
    )
    echo [ARM64 1/3] ✓ Portable ZIP created successfully
    echo.
    
    :: 2. Create MSI Installer
    echo [ARM64 2/3] Creating MSI installer package...
    echo ----------------------------------------
    call build_msi.bat !ARCH! !QT_PATH!
    if errorlevel 1 (
        echo ERROR: ARM64 MSI creation failed!
        pause
        exit /b 1
    )
    echo [ARM64 2/3] ✓ MSI installer created successfully
    echo.
    
    :: 3. Create MSIX Package
    echo [ARM64 3/3] Creating MSIX package...
    echo ----------------------------------------
    call build_msix.bat !ARCH! !QT_PATH!
    if errorlevel 1 (
        echo ERROR: ARM64 MSIX creation failed!
        echo This is non-critical if you don't need Microsoft Store deployment.
        echo Continuing...
    )
    echo [ARM64 3/3] ✓ MSIX package created successfully
    echo.
)

:: Summary
echo ======================================================================
echo Deployment Summary
echo ======================================================================
echo.
echo All Windows deployment packages have been created in: %ROOT_DIR%\dist\
echo.

:: Check what was actually created for x64
if %BUILD_X64%==1 (
    echo x64 Packages:
    if exist "%ROOT_DIR%\dist\MouseCross-v0.1.0-Windows-x64-Portable.zip" (
        echo ✓ Portable ZIP: MouseCross-v0.1.0-Windows-x64-Portable.zip
    ) else (
        echo ✗ Portable ZIP: FAILED
    )
    
    if exist "%ROOT_DIR%\dist\MSI\MouseCross-0.1.0-x64.msi" (
        echo ✓ MSI Installer: MSI\MouseCross-0.1.0-x64.msi
    ) else (
        echo ✗ MSI Installer: FAILED
    )
    
    if exist "%ROOT_DIR%\dist\MSIX\MouseCross-v0.1.0-x64.msix" (
        echo ✓ MSIX Package: MSIX\MouseCross-v0.1.0-x64.msix
    ) else (
        echo ✗ MSIX Package: FAILED
    )
    echo.
)

:: Check what was actually created for ARM64
if %BUILD_ARM64%==1 (
    echo ARM64 Packages:
    if exist "%ROOT_DIR%\dist\MouseCross-v0.1.0-Windows-arm64-Portable.zip" (
        echo ✓ Portable ZIP: MouseCross-v0.1.0-Windows-arm64-Portable.zip
    ) else (
        echo ✗ Portable ZIP: FAILED
    )
    
    if exist "%ROOT_DIR%\dist\MSI\MouseCross-0.1.0-arm64.msi" (
        echo ✓ MSI Installer: MSI\MouseCross-0.1.0-arm64.msi
    ) else (
        echo ✗ MSI Installer: FAILED
    )
    
    if exist "%ROOT_DIR%\dist\MSIX\MouseCross-v0.1.0-arm64.msix" (
        echo ✓ MSIX Package: MSIX\MouseCross-v0.1.0-arm64.msix
    ) else (
        echo ✗ MSIX Package: FAILED
    )
    echo.
)

echo ======================================================================
echo Next Steps:
echo ======================================================================
echo 1. Test each package on a clean system
echo 2. Sign packages with code signing certificate if available
echo 3. Submit to distribution channels:
echo    - Direct: Share portable ZIP
echo    - WinGet: Submit MSI to winget-pkgs repository
echo    - Microsoft Store: Upload MSIX to Partner Center
echo.
echo Deployment complete!
pause