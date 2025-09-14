@echo off
setlocal enabledelayedexpansion

:: Parse command line arguments
set ARCH=%1
set QT_PATH=%2

:: If no arguments provided, use defaults (x64) with auto-detected Qt
if "%ARCH%"=="" (
    set ARCH=x64
    :: Auto-detect Qt version
    for /d %%d in ("C:\Qt\6.*") do (
        if exist "%%d\msvc2022_64" (
            set QT_PATH=%%d\msvc2022_64
            goto qt_found_msi
        )
    )
    :qt_found_msi
    if "!QT_PATH!"=="" (
        echo ERROR: No Qt installation found for x64
        exit /b 1
    )
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

:: Check for WiX Toolset (both v6.0 and v3.14)
set "WIX_PATH_V6=C:\Program Files\WiX Toolset v6.0"
set "WIX_PATH_V3=C:\Program Files (x86)\WiX Toolset v3.14"
if exist "%WIX_PATH_V6%\bin\wix.exe" (
    set "PATH=%WIX_PATH_V6%\bin;%PATH%"
    echo Found WiX Toolset v6.0 at "%WIX_PATH_V6%"
) else if exist "%WIX_PATH_V3%\bin\candle.exe" (
    set "PATH=%WIX_PATH_V3%\bin;%PATH%"
    echo Found WiX Toolset v3.14 at "%WIX_PATH_V3%"
) else (
    where wix >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        where candle >nul 2>&1
        if %ERRORLEVEL% NEQ 0 (
            echo Error: WiX Toolset not found. Please install WiX Toolset v3.14 or v6.0
            echo Download v3.14 from: https://github.com/wixtoolset/wix3/releases
            echo Download v6.0 from: https://wixtoolset.org/
            exit /b 1
        )
        echo Found WiX Toolset v3.x in PATH
    ) else (
        echo Found WiX Toolset v6.x in PATH
    )
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