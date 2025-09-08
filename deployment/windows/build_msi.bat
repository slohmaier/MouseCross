@echo off
setlocal enabledelayedexpansion

:: Parse command line arguments
set ARCH=%1
set QT_PATH=%2

:: If no arguments provided, use defaults (x64)
if "%ARCH%"=="" (
    set ARCH=x64
    set QT_PATH=C:\Qt\6.9.2\msvc2022_64
)

echo Building MouseCross MSI Installer for WinGet (%ARCH%)...
echo ===============================================

:: Set variables based on script location
set SCRIPT_DIR=%~dp0
set ROOT_DIR=%SCRIPT_DIR%..\..

:: Add CMake to PATH if available in Qt Tools
if exist "C:\Qt\Tools\CMake_64\bin\cmake.exe" (
    set "PATH=C:\Qt\Tools\CMake_64\bin;%PATH%"
)
set BUILD_DIR=%ROOT_DIR%\build-%ARCH%
set OUTPUT_DIR=%ROOT_DIR%\dist
set MSI_DIR=%OUTPUT_DIR%\MSI

:: Check for WiX Toolset
set "WIX_PATH=C:\Program Files\WiX Toolset v6.0"
if exist "%WIX_PATH%\bin\wix.exe" (
    set "PATH=%WIX_PATH%\bin;%PATH%"
    echo Found WiX Toolset v6.0 at "%WIX_PATH%"
) else (
    where wix >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        echo Error: WiX Toolset not found. Please install WiX Toolset v6.0
        echo Download from: https://wixtoolset.org/
        echo Make sure wix.exe is in your PATH
        exit /b 1
    )
    echo Found WiX Toolset in PATH
)

:: Clean and create directories
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"
if not exist "%MSI_DIR%" mkdir "%MSI_DIR%"

:: Clean and create build directory for this architecture
if exist "%BUILD_DIR%" rd /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"

:: Configure with CMake for the specific architecture
cd "%BUILD_DIR%"
echo Configuring CMake for %ARCH% with Qt at %QT_PATH%...
if "%ARCH%"=="x64" (
    cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="%QT_PATH%" "%ROOT_DIR%"
) else if "%ARCH%"=="arm64" (
    cmake -G "Visual Studio 17 2022" -A ARM64 -DCMAKE_PREFIX_PATH="%QT_PATH%" "%ROOT_DIR%"
) else (
    cmake -DCMAKE_PREFIX_PATH="%QT_PATH%" "%ROOT_DIR%"
)
if errorlevel 1 (
    echo CMake configuration failed for %ARCH%!
    exit /b 1
)

:: Build in Release mode (icons generated automatically by CMake)
echo Building Release configuration for %ARCH%...
cmake --build . --config Release
if errorlevel 1 (
    echo Build failed for %ARCH%!
    exit /b 1
)

:: Create MSI using CPack WiX generator (Qt dependencies deployed by CMake)
echo Creating MSI package for %ARCH%...
cpack -G WIX
if errorlevel 1 (
    echo MSI creation failed for %ARCH%!
    exit /b 1
)

:: Move and rename MSI to include architecture
for %%f in (*.msi) do (
    set "MSI_FILE=%%~nf-%ARCH%%%~xf"
    move "%%f" "%MSI_DIR%\!MSI_FILE!"
)

cd "%ROOT_DIR%"

echo.
echo MSI installer created in: %MSI_DIR%\
echo Architecture: %ARCH%
echo.
echo For WinGet submission:
echo 1. Test the MSI installer thoroughly
echo 2. Submit to winget-pkgs repository: https://github.com/microsoft/winget-pkgs
echo 3. Follow WinGet manifest creation guidelines
echo 4. Create manifests in format: manifests/s/slohmaier/MouseCross/0.1.0/
echo.
echo Done!