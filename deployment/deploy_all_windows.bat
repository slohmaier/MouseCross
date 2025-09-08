@echo off
setlocal enabledelayedexpansion

echo MouseCross - Complete Windows Deployment Pipeline
echo =================================================
echo This script will create all Windows deployment packages:
echo - Portable ZIP (no installation required)
echo - MSI Installer (for WinGet and enterprise)
echo - MSIX Package (for Microsoft Store)
echo =================================================
echo.

:: Set directories based on script location
set SCRIPT_DIR=%~dp0
set ROOT_DIR=%SCRIPT_DIR%..
set DEPLOYMENT_DIR=%SCRIPT_DIR%windows

:: Kill any running MouseCross processes
echo [PREP] Stopping any running MouseCross processes...
taskkill /f /im MouseCross.exe >nul 2>&1 || echo No MouseCross processes found

:: Clean previous builds
echo [PREP] Cleaning previous deployment files...
if exist "%ROOT_DIR%\dist" rmdir /s /q "%ROOT_DIR%\dist" >nul 2>&1

:: Verify WiX installation
echo [PREP] Checking WiX Toolset installation...
set "WIX_PATH=C:\Program Files\WiX Toolset v6.0"
if not exist "%WIX_PATH%\wix.exe" (
    echo ERROR: WiX Toolset not found at expected location: %WIX_PATH%
    echo Please ensure WiX Toolset v6.0 is installed at the correct location.
    pause
    exit /b 1
)
echo Found WiX Toolset at %WIX_PATH%

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
echo =================================================
echo Starting deployment process...
echo =================================================
echo.

:: 1. Create Portable ZIP
echo [1/3] Creating Portable ZIP package...
echo ----------------------------------------
cd /d "%DEPLOYMENT_DIR%"
call build_portable.bat
if errorlevel 1 (
    echo ERROR: Portable ZIP creation failed!
    pause
    exit /b 1
)
echo [1/3] ✓ Portable ZIP created successfully
echo.

:: 2. Create MSI Installer
echo [2/3] Creating MSI installer package...
echo ----------------------------------------
call build_msi.bat
if errorlevel 1 (
    echo ERROR: MSI creation failed!
    pause
    exit /b 1
)
echo [2/3] ✓ MSI installer created successfully
echo.

:: 3. Create MSIX Package
echo [3/3] Creating MSIX package...
echo ----------------------------------------
call build_msix.bat
if errorlevel 1 (
    echo ERROR: MSIX creation failed!
    echo This is non-critical if you don't need Microsoft Store deployment.
    echo Continuing...
)
echo [3/3] ✓ MSIX package created successfully
echo.

:: Summary
echo =================================================
echo Deployment Summary
echo =================================================
echo.
echo All Windows deployment packages have been created in: %ROOT_DIR%\dist\
echo.

:: Check what was actually created
if exist "%ROOT_DIR%\dist\MouseCross-v0.1.0-Windows-Portable.zip" (
    echo ✓ Portable ZIP: MouseCross-v0.1.0-Windows-Portable.zip
) else (
    echo ✗ Portable ZIP: FAILED
)

if exist "%ROOT_DIR%\dist\MSI\MouseCross-0.1.0-win64.msi" (
    echo ✓ MSI Installer: MSI\MouseCross-0.1.0-win64.msi
) else (
    echo ✗ MSI Installer: FAILED
)

if exist "%ROOT_DIR%\dist\MSIX\MouseCross-v0.1.0.msix" (
    echo ✓ MSIX Package: MSIX\MouseCross-v0.1.0.msix
) else (
    echo ✗ MSIX Package: FAILED
)

echo.
echo =================================================
echo Next Steps:
echo =================================================
echo 1. Test each package on a clean system
echo 2. Sign packages with code signing certificate if available
echo 3. Submit to distribution channels:
echo    - Direct: Share portable ZIP
echo    - WinGet: Submit MSI to winget-pkgs repository
echo    - Microsoft Store: Upload MSIX to Partner Center
echo.
echo Deployment complete!
pause