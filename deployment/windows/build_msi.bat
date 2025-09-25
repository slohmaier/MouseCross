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

:: Deploy Qt libraries for ARM64 builds (windeployqt doesn't support ARM64 cross-compilation)
if "%ARCH%"=="arm64" (
    echo Deploying Qt ARM64 libraries to Release directory...
    call "%SCRIPT_DIR%deploy_qt_arm64.bat" "%BUILD_DIR%\Release\MouseCross.exe" "%QT_PATH%"
    if errorlevel 1 (
        echo WARNING: ARM64 Qt deployment to Release directory failed!
    )
)

:: Create MSI using WiX directly (skip CPack for professional installer)
echo Creating MSI package for %ARCH%...

:: Skip CPack and go directly to WiX v4 for professional installer
goto use_wix_directly

:: Old CPack method (disabled for professional installer)
:: cpack -G WIX >nul 2>&1
:: if not errorlevel 1 (
::     echo MSI created successfully with CPack
::     goto msi_success
:: )

:use_wix_directly

:: If CPack failed, check if it's due to WiX v4 (missing candle.exe)
echo CPack WiX failed, trying direct WiX v4 build...

:: Check for wix.exe (WiX v4+)
where wix >nul 2>&1
if errorlevel 1 (
    echo ERROR: Neither WiX v3 for CPack nor WiX v4 CLI found
    echo Please install WiX Toolset v3.14 or v4+
    exit /b 1
)

:: Create simple WiX v4 project file
set WIX_BUILD_DIR=%BUILD_DIR%\wix_build
if not exist "%WIX_BUILD_DIR%" mkdir "%WIX_BUILD_DIR%"

:: Copy all files from Release to wix build dir and deploy Qt for ARM64 if needed
if "%ARCH%"=="arm64" (
    :: First copy the executable
    copy "%BUILD_DIR%\Release\MouseCross.exe" "%WIX_BUILD_DIR%\" >nul
    :: Then deploy Qt libraries using custom deployment script
    call "%SCRIPT_DIR%deploy_qt_arm64.bat" "%WIX_BUILD_DIR%\MouseCross.exe" "%QT_PATH%"
    if errorlevel 1 (
        echo Custom ARM64 Qt deployment failed, copying manually...
        xcopy "%BUILD_DIR%\Release\*" "%WIX_BUILD_DIR%\" /E /I /Y >nul
    )
) else (
    :: For x64, copy all files (already includes Qt libraries from CMake)
    xcopy "%BUILD_DIR%\Release\*" "%WIX_BUILD_DIR%\" /E /I /Y >nul
)

:: Use comprehensive WiX source file with installer workflow
echo Creating WiX source file with full installer workflow...
cd "%WIX_BUILD_DIR%"

:: Copy the appropriate WiX source file based on architecture
if "%ARCH%"=="arm64" (
    copy "%SCRIPT_DIR%MouseCross_ARM64.wxs" "MouseCross.wxs" >nul
    if not exist MouseCross.wxs (
        echo ERROR: MouseCross_ARM64.wxs template not found at %SCRIPT_DIR%MouseCross_ARM64.wxs
        cd "%BUILD_DIR%"
        exit /b 1
    )
    echo Using ARM64-specific WiX configuration (no DirectX/VC components)
) else (
    :: Use the new professional installer v4 with full UI
    copy "%SCRIPT_DIR%MouseCross_Professional_v4.wxs" "MouseCross.wxs" >nul
    if not exist MouseCross.wxs (
        :: Fallback to Professional version if v4 doesn't exist
        copy "%SCRIPT_DIR%MouseCross_Professional.wxs" "MouseCross.wxs" >nul
        if not exist MouseCross.wxs (
            :: Fallback to Complete version if Professional doesn't exist
            copy "%SCRIPT_DIR%MouseCross_Complete.wxs" "MouseCross.wxs" >nul
            if not exist MouseCross.wxs (
                echo ERROR: No suitable WiX template found
                cd "%BUILD_DIR%"
                exit /b 1
            )
            echo Using complete WiX configuration with all components
        ) else (
            echo Using professional installer (v3 syntax)
        )
    ) else (
        echo Using professional installer v4 with enhanced UI workflow
    )
)

:: Copy license file for the installer
copy "%SCRIPT_DIR%license.rtf" "license.rtf" >nul

:: Build with WiX v4 using our comprehensive source with UI and Util extensions
echo Building MSI with professional installer workflow...
wix build MouseCross.wxs -ext WixToolset.UI.wixext -ext WixToolset.Util.wixext -d SourceDir=. -d ProjectDir=. -arch %ARCH% -out MouseCross.msi

if errorlevel 1 (
    echo MSI creation failed for %ARCH%!
    cd "%BUILD_DIR%"
    exit /b 1
)

:: Move the MSI back to build dir
move MouseCross.msi "%BUILD_DIR%\" >nul
cd "%BUILD_DIR%"

:msi_success

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
echo 4. Create manifests in format: manifests/s/slohmaier/MouseCross/0.1.12/
echo.
echo Done!