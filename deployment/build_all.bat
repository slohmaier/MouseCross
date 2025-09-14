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

:: Auto-detect Qt version and architectures (prioritize newer versions)
set QT_BASE=C:\Qt
set BUILD_X64=0
set BUILD_ARM64=0
set QT_VERSION=

if exist "%QT_BASE%\6.9.1\msvc2022_64" (
    set QT_VERSION=6.9.1
    goto found_qt
) else if exist "%QT_BASE%\6.9.1\msvc2022_arm64" (
    set QT_VERSION=6.9.1
    goto found_qt
) else if exist "%QT_BASE%\6.9.0\msvc2022_64" (
    set QT_VERSION=6.9.0
    goto found_qt
) else if exist "%QT_BASE%\6.9.0\msvc2022_arm64" (
    set QT_VERSION=6.9.0
    goto found_qt
) else (
    :: Fallback to directory scanning
    for /d %%d in ("%QT_BASE%\6.*") do (
        if exist "%%d\msvc2022_64" (
            for %%f in ("%%d") do set QT_VERSION=%%~nxf
            goto found_qt
        )
        if exist "%%d\msvc2022_arm64" (
            for %%f in ("%%d") do set QT_VERSION=%%~nxf
            goto found_qt
        )
    )
)

:found_qt
if "%QT_VERSION%"=="" (
    echo ERROR: No Qt installation found in %QT_BASE%
    echo Please ensure Qt is installed with at least one MSVC 2022 architecture.
    pause
    exit /b 1
)

echo [DETECT] Found Qt %QT_VERSION% at %QT_BASE%\%QT_VERSION%

if exist "%QT_BASE%\%QT_VERSION%\msvc2022_64" (
    echo [DETECT] Found x64 Qt at %QT_BASE%\%QT_VERSION%\msvc2022_64
    set BUILD_X64=1
)

if exist "%QT_BASE%\%QT_VERSION%\msvc2022_arm64" (
    echo [DETECT] Found ARM64 Qt at %QT_BASE%\%QT_VERSION%\msvc2022_arm64
    set BUILD_ARM64=1
)

if %BUILD_X64%==0 if %BUILD_ARM64%==0 (
    echo ERROR: No Qt architectures found in %QT_BASE%\%QT_VERSION%
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
    set QT_PATH=%QT_BASE%\%QT_VERSION%\msvc2022_64

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
        echo WARNING: x64 MSI creation failed!
        echo This is non-critical for portable deployment.
        echo.
    ) else (
        echo [x64 2/3] ✓ MSI installer created successfully
        echo.
    )

    :: 3. Create MSIX Package
    echo [x64 3/3] Creating MSIX package...
    echo ----------------------------------------
    call build_msix.bat !ARCH! !QT_PATH!
    if errorlevel 1 (
        echo WARNING: x64 MSIX creation failed!
        echo This is non-critical if you don't need Microsoft Store deployment.
    ) else (
        echo [x64 3/3] ✓ MSIX package created successfully
    )
    echo.
    echo.
)

:: Build for ARM64 if available
if %BUILD_ARM64%==1 (
    echo ======================================================================
    echo Building ARM64 packages...
    echo ======================================================================

    set ARCH=arm64
    set QT_PATH=%QT_BASE%\%QT_VERSION%\msvc2022_arm64

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
        echo WARNING: ARM64 MSI creation failed!
        echo This is non-critical for portable deployment.
        echo.
    ) else (
        echo [ARM64 2/3] ✓ MSI installer created successfully
        echo.
    )

    :: 3. Create MSIX Package
    echo [ARM64 3/3] Creating MSIX package...
    echo ----------------------------------------
    call build_msix.bat !ARCH! !QT_PATH!
    if errorlevel 1 (
        echo WARNING: ARM64 MSIX creation failed!
        echo This is non-critical if you don't need Microsoft Store deployment.
    ) else (
        echo [ARM64 3/3] ✓ MSIX package created successfully
    )
    echo.
    echo.
)

::=======================================
:: Copy artifacts to dist directory
::=======================================
echo.
echo Copying deployment artifacts to dist directory...
echo =====================================================

:: Create dist subdirectories
if not exist "%ROOT_DIR%\dist" mkdir "%ROOT_DIR%\dist"
if not exist "%ROOT_DIR%\dist\MSI" mkdir "%ROOT_DIR%\dist\MSI"
if not exist "%ROOT_DIR%\dist\MSIX" mkdir "%ROOT_DIR%\dist\MSIX"

:: Copy x64 artifacts if built
if %BUILD_X64%==1 (
    echo [COPY] Copying x64 artifacts...

    :: Use PowerShell for more reliable file operations
    powershell -Command "try { Copy-Item -Path '.\build-manual\_CPack_Packages\win64\ZIP\*.zip' -Destination '.\dist\MouseCross-v0.1.0-Windows-x64-Portable.zip' -Force -ErrorAction SilentlyContinue; if (Test-Path '.\dist\MouseCross-v0.1.0-Windows-x64-Portable.zip') { Write-Host '   ✓ Copied Portable ZIP from build-manual' } } catch {}"
    powershell -Command "try { Copy-Item -Path '.\build\_CPack_Packages\win64\ZIP\*.zip' -Destination '.\dist\MouseCross-v0.1.0-Windows-x64-Portable.zip' -Force -ErrorAction SilentlyContinue; if (Test-Path '.\dist\MouseCross-v0.1.0-Windows-x64-Portable.zip') { Write-Host '   ✓ Copied Portable ZIP from build' } } catch {}"

    powershell -Command "try { Copy-Item -Path '.\build-manual\_CPack_Packages\win64\WIX\*.msi' -Destination '.\dist\MSI\MouseCross-0.1.0-x64.msi' -Force -ErrorAction SilentlyContinue; if (Test-Path '.\dist\MSI\MouseCross-0.1.0-x64.msi') { Write-Host '   ✓ Copied MSI installer from build-manual' } } catch {}"
    powershell -Command "try { Copy-Item -Path '.\build\_CPack_Packages\win64\WIX\*.msi' -Destination '.\dist\MSI\MouseCross-0.1.0-x64.msi' -Force -ErrorAction SilentlyContinue; if (Test-Path '.\dist\MSI\MouseCross-0.1.0-x64.msi') { Write-Host '   ✓ Copied MSI installer from build' } } catch {}"

    powershell -Command "try { Copy-Item -Path '.\build-manual\_CPack_Packages\win64\**\*.msix' -Destination '.\dist\MSIX\MouseCross-v0.1.0-x64.msix' -Force -ErrorAction SilentlyContinue; if (Test-Path '.\dist\MSIX\MouseCross-v0.1.0-x64.msix') { Write-Host '   ✓ Copied MSIX package from build-manual' } } catch {}"
    powershell -Command "try { Copy-Item -Path '.\build\_CPack_Packages\win64\**\*.msix' -Destination '.\dist\MSIX\MouseCross-v0.1.0-x64.msix' -Force -ErrorAction SilentlyContinue; if (Test-Path '.\dist\MSIX\MouseCross-v0.1.0-x64.msix') { Write-Host '   ✓ Copied MSIX package from build' } } catch {}"
)

:: Copy ARM64 artifacts if built
if %BUILD_ARM64%==1 (
    echo [COPY] Copying ARM64 artifacts...

    :: Copy Portable ZIP from build directories
    for %%f in ("%ROOT_DIR%\build-arm64\_CPack_Packages\win64\ZIP\*.zip") do (
        if exist "%%f" (
            copy "%%f" "%ROOT_DIR%\dist\MouseCross-v0.1.0-Windows-arm64-Portable.zip" >nul
            echo   ✓ Copied ARM64 Portable ZIP
        )
    )

    :: Copy MSI from build directories
    for %%f in ("%ROOT_DIR%\build-arm64\_CPack_Packages\win64\WIX\*.msi") do (
        if exist "%%f" (
            copy "%%f" "%ROOT_DIR%\dist\MSI\MouseCross-0.1.0-arm64.msi" >nul
            echo   ✓ Copied ARM64 MSI installer
        )
    )

    :: Copy MSIX from build directories
    for %%f in ("%ROOT_DIR%\build-arm64\_CPack_Packages\win64\*\*.msix") do (
        if exist "%%f" (
            copy "%%f" "%ROOT_DIR%\dist\MSIX\MouseCross-v0.1.0-arm64.msix" >nul
            echo   ✓ Copied ARM64 MSIX package
        )
    )
)

echo.

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