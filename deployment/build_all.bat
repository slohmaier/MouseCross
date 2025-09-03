@echo off
setlocal enabledelayedexpansion

echo MouseCross - Build All Deployment Packages
echo ==========================================
echo.

:: Set directories based on script location
set SCRIPT_DIR=%~dp0
set ROOT_DIR=%SCRIPT_DIR%..
cd /d "%ROOT_DIR%"

:: Check if build directory exists
if not exist "build" (
    echo Error: Build directory not found. Please run cmake configuration first:
    echo   mkdir build
    echo   cd build
    echo   cmake ..
    echo   cd ..
    exit /b 1
)

:: Build the project first
echo [1/4] Building MouseCross...
cd build
cmake --build . --config Release
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)
cd ..

echo [2/4] Creating Portable ZIP...
cd deployment\windows
call build_portable.bat
if errorlevel 1 (
    echo Portable ZIP creation failed!
    exit /b 1
)
cd ..\..

echo [3/4] Creating MSI Installer...
cd deployment\windows
call build_msi.bat
if errorlevel 1 (
    echo MSI creation failed!
    exit /b 1
)
cd ..\..

echo [4/4] Creating MSIX Package...
cd deployment\windows
call build_msix.bat
if errorlevel 1 (
    echo MSIX creation failed!
    exit /b 1
)
cd ..\..

echo.
echo ==========================================
echo All Windows deployment packages created!
echo ==========================================
echo.
echo Output directory: %ROOT_DIR%\dist\
echo   - MouseCross-v0.1.0-Windows-Portable.zip
echo   - MSI\MouseCross-0.1.0-win64.msi
echo   - MSIX\MouseCross-v0.1.0.msix
echo.
echo Ready for distribution!
echo.
echo Next steps:
echo 1. Test each package thoroughly
echo 2. Sign packages with code signing certificate
echo 3. Submit to respective distribution channels
echo.
pause