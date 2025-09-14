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
            goto qt_found_portable
        )
    )
    :qt_found_portable
    if "!QT_PATH!"=="" (
        echo ERROR: No Qt installation found for x64
        exit /b 1
    )
)

echo Building MouseCross Portable ZIP Package for %ARCH%...
echo ==========================================

:: Set variables based on script location
set SCRIPT_DIR=%~dp0
set ROOT_DIR=%SCRIPT_DIR%..\..

:: Add CMake to PATH if available in Qt Tools
if exist "C:\Qt\Tools\CMake_64\bin\cmake.exe" (
    set "PATH=C:\Qt\Tools\CMake_64\bin;%PATH%"
)
set BUILD_DIR=%ROOT_DIR%\build-%ARCH%
set OUTPUT_DIR=%ROOT_DIR%\dist
set PORTABLE_DIR=%OUTPUT_DIR%\MouseCross-Portable-%ARCH%

:: Clean and create directories
if exist "%PORTABLE_DIR%" rd /s /q "%PORTABLE_DIR%"
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"
mkdir "%PORTABLE_DIR%"

:: Clean and create build directory for this architecture
if exist "%BUILD_DIR%" rd /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"

:: Configure with CMake for the specific architecture
cd "%BUILD_DIR%"
echo Configuring CMake for %ARCH% with Qt at %QT_PATH%...
if "%ARCH%"=="x64" (
    cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=%QT_PATH% "%ROOT_DIR%"
) else if "%ARCH%"=="arm64" (
    cmake -G "Visual Studio 17 2022" -A ARM64 -DCMAKE_PREFIX_PATH=%QT_PATH% "%ROOT_DIR%"
) else (
    cmake -DCMAKE_PREFIX_PATH=%QT_PATH% "%ROOT_DIR%"
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
cd "%ROOT_DIR%"

:: Copy executable and Qt dependencies
echo Copying executable and Qt dependencies for %ARCH%...
echo DEBUG: ARCH variable is "%ARCH%"
if "%ARCH%"=="arm64" (
    :: For ARM64, copy executable first then use custom deployment
    copy "%BUILD_DIR%\Release\MouseCross.exe" "%PORTABLE_DIR%\" >nul
    echo Deploying Qt ARM64 libraries with custom script...
    call "%SCRIPT_DIR%deploy_qt_arm64.bat" "%PORTABLE_DIR%\MouseCross.exe" "%QT_PATH%"
    if errorlevel 1 (
        echo Custom ARM64 Qt deployment failed, copying manually...
        xcopy "%BUILD_DIR%\Release\*" "%PORTABLE_DIR%\" /E /I /Y
    ) else (
        echo ARM64 Qt libraries deployed successfully
    )
) else (
    :: For x64, use standard deployment (already includes Qt libraries from CMake)
    xcopy "%BUILD_DIR%\Release\*" "%PORTABLE_DIR%\" /E /I /Y
)

:: Create README for portable version
echo MouseCross Portable Version (%ARCH%) > "%PORTABLE_DIR%\README.txt"
echo ====================================== >> "%PORTABLE_DIR%\README.txt"
echo. >> "%PORTABLE_DIR%\README.txt"
echo Architecture: %ARCH% >> "%PORTABLE_DIR%\README.txt"
echo. >> "%PORTABLE_DIR%\README.txt"
echo This is the portable version of MouseCross. >> "%PORTABLE_DIR%\README.txt"
echo No installation required - just run MouseCross.exe >> "%PORTABLE_DIR%\README.txt"
echo. >> "%PORTABLE_DIR%\README.txt"
echo To run MouseCross automatically at startup: >> "%PORTABLE_DIR%\README.txt"
echo 1. Press Win+R, type "shell:startup" >> "%PORTABLE_DIR%\README.txt"
echo 2. Copy MouseCross.exe to the opened folder >> "%PORTABLE_DIR%\README.txt"
echo. >> "%PORTABLE_DIR%\README.txt"
echo Version: 0.1.0 >> "%PORTABLE_DIR%\README.txt"
echo Website: https://slohmaier.de/MouseCross >> "%PORTABLE_DIR%\README.txt"

:: Create ZIP file with architecture in name
cd "%OUTPUT_DIR%"
set ZIP_NAME=MouseCross-v0.1.0-Windows-%ARCH%-Portable.zip
powershell Compress-Archive -Path "MouseCross-Portable-%ARCH%\*" -DestinationPath "%ZIP_NAME%" -Force

echo.
echo Portable ZIP created: %OUTPUT_DIR%\%ZIP_NAME%
echo Done!