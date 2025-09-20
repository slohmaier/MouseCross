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
            goto qt_found_msix
        )
    )
    :qt_found_msix
    if "!QT_PATH!"=="" (
        echo ERROR: No Qt installation found for x64
        exit /b 1
    )
)

:: Map architecture names for MSIX
set MSIX_ARCH=%ARCH%
if "%ARCH%"=="x64" set MSIX_ARCH=x64
if "%ARCH%"=="arm64" set MSIX_ARCH=arm64

echo Building MouseCross MSIX Package for Microsoft Store (%ARCH%)...
echo ======================================================

:: Set variables based on script location
set SCRIPT_DIR=%~dp0
set ROOT_DIR=%SCRIPT_DIR%..\..

:: Add CMake to PATH if available in Qt Tools
if exist "C:\Qt\Tools\CMake_64\bin\cmake.exe" (
    set "PATH=C:\Qt\Tools\CMake_64\bin;%PATH%"
)
set BUILD_DIR=%ROOT_DIR%\build-%ARCH%
set OUTPUT_DIR=%ROOT_DIR%\dist
set MSIX_DIR=%OUTPUT_DIR%\MSIX
set MSIX_STAGING=%MSIX_DIR%\staging-%ARCH%

:: Check for makeappx tool
where makeappx >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: makeappx tool not found. Please install Windows 10 SDK
    echo Download from: https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/
    exit /b 1
)

:: Clean and create directories
if exist "%MSIX_STAGING%" rd /s /q "%MSIX_STAGING%"
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"
if not exist "%MSIX_DIR%" mkdir "%MSIX_DIR%"
mkdir "%MSIX_STAGING%"

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

cd "%ROOT_DIR%"

:: Copy executable and Qt dependencies (deployed by CMake) to staging
echo Copying executable and Qt dependencies to MSIX staging for %ARCH%...
xcopy "%BUILD_DIR%\Release\*" "%MSIX_STAGING%\" /E /I /Y

:: Check if Package.appxmanifest exists, if not create a basic one
if not exist "%SCRIPT_DIR%\Package.appxmanifest" (
    echo Creating Package.appxmanifest for %ARCH%...
    call :CreateManifest "%MSIX_STAGING%\AppxManifest.xml" %MSIX_ARCH%
) else (
    :: Copy and update manifest with correct architecture
    powershell -Command "(Get-Content '%SCRIPT_DIR%\Package.appxmanifest') -replace 'ProcessorArchitecture=\"x64\"', 'ProcessorArchitecture=\"%MSIX_ARCH%\"' | Set-Content '%MSIX_STAGING%\AppxManifest.xml'"
)

:: Create Images directory and placeholder icons
mkdir "%MSIX_STAGING%\Images"

:: Copy app icon to different required sizes (using same icon for all)
:: Check if icons exist in resources folder or use generated ones from build
if exist "%ROOT_DIR%\resources\icons\app_icon.png" (
    copy "%ROOT_DIR%\resources\icons\app_icon.png" "%MSIX_STAGING%\Images\Square44x44Logo.png"
    copy "%ROOT_DIR%\resources\icons\app_icon.png" "%MSIX_STAGING%\Images\Square150x150Logo.png"
    copy "%ROOT_DIR%\resources\icons\app_icon.png" "%MSIX_STAGING%\Images\Wide310x150Logo.png"
    copy "%ROOT_DIR%\resources\icons\app_icon.png" "%MSIX_STAGING%\Images\StoreLogo.png"
    copy "%ROOT_DIR%\resources\icons\app_icon.png" "%MSIX_STAGING%\Images\SplashScreen.png"
) else if exist "%BUILD_DIR%\app_icon.png" (
    copy "%BUILD_DIR%\app_icon.png" "%MSIX_STAGING%\Images\Square44x44Logo.png"
    copy "%BUILD_DIR%\app_icon.png" "%MSIX_STAGING%\Images\Square150x150Logo.png"
    copy "%BUILD_DIR%\app_icon.png" "%MSIX_STAGING%\Images\Wide310x150Logo.png"
    copy "%BUILD_DIR%\app_icon.png" "%MSIX_STAGING%\Images\StoreLogo.png"
    copy "%BUILD_DIR%\app_icon.png" "%MSIX_STAGING%\Images\SplashScreen.png"
)

:: Create MSIX package with architecture in name
cd "%MSIX_STAGING%"
set MSIX_NAME=MouseCross-v0.1.12-%ARCH%.msix
makeappx pack /d . /p "..\%MSIX_NAME%"
if errorlevel 1 (
    echo MSIX package creation failed for %ARCH%!
    exit /b 1
)

cd "%ROOT_DIR%"

echo.
echo MSIX package created: %MSIX_DIR%\%MSIX_NAME%
echo Architecture: %ARCH%
echo.
echo For Microsoft Store submission:
echo 1. Sign the MSIX package with your code signing certificate
echo 2. Test the package with: Add-AppxPackage %MSIX_NAME%
echo 3. Submit to Partner Center: https://partner.microsoft.com/
echo 4. Complete store listing with descriptions, screenshots, etc.
echo.
echo To sign the package (if you have a certificate):
echo signtool sign /fd SHA256 /a %MSIX_NAME%
echo.
echo Done!

goto :eof

:CreateManifest
set MANIFEST_FILE=%~1
set MANIFEST_ARCH=%~2
(
echo ^<?xml version="1.0" encoding="utf-8"?^>
echo ^<Package xmlns="http://schemas.microsoft.com/appx/manifest/foundation/windows10"
echo          xmlns:mp="http://schemas.microsoft.com/appx/2014/phone/manifest"
echo          xmlns:uap="http://schemas.microsoft.com/appx/manifest/uap/windows10"
echo          IgnorableNamespaces="uap mp"^>
echo   ^<Identity Name="slohmaier.MouseCross" Publisher="CN=Stefan Lohmaier" Version="0.1.12.0" ProcessorArchitecture="%MANIFEST_ARCH%" /^>
echo   ^<mp:PhoneIdentity PhoneProductId="12345678-1234-1234-1234-123456789012" PhonePublisherId="00000000-0000-0000-0000-000000000000" /^>
echo   ^<Properties^>
echo     ^<DisplayName^>MouseCross^</DisplayName^>
echo     ^<PublisherDisplayName^>Stefan Lohmaier^</PublisherDisplayName^>
echo     ^<Logo^>Images\StoreLogo.png^</Logo^>
echo   ^</Properties^>
echo   ^<Dependencies^>
echo     ^<TargetDeviceFamily Name="Windows.Desktop" MinVersion="10.0.17763.0" MaxVersionTested="10.0.22621.0" /^>
echo   ^</Dependencies^>
echo   ^<Resources^>
echo     ^<Resource Language="en-US" /^>
echo   ^</Resources^>
echo   ^<Applications^>
echo     ^<Application Id="MouseCross" Executable="MouseCross.exe" EntryPoint="Windows.FullTrustApplication"^>
echo       ^<uap:VisualElements DisplayName="MouseCross"
echo                          Description="Visual crosshair overlay for mouse cursor"
echo                          BackgroundColor="transparent"
echo                          Square150x150Logo="Images\Square150x150Logo.png"
echo                          Square44x44Logo="Images\Square44x44Logo.png"^>
echo         ^<uap:DefaultTile Wide310x150Logo="Images\Wide310x150Logo.png" /^>
echo         ^<uap:SplashScreen Image="Images\SplashScreen.png" /^>
echo       ^</uap:VisualElements^>
echo     ^</Application^>
echo   ^</Applications^>
echo   ^<Capabilities^>
echo     ^<rescap:Capability Name="runFullTrust" xmlns:rescap="http://schemas.microsoft.com/appx/manifest/foundation/windows10/restrictedcapabilities" /^>
echo   ^</Capabilities^>
echo ^</Package^>
) > "%MANIFEST_FILE%"
goto :eof