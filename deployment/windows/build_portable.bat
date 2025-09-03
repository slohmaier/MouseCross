@echo off
setlocal enabledelayedexpansion

echo Building MouseCross Portable ZIP Package...
echo ==========================================

:: Set variables based on script location
set SCRIPT_DIR=%~dp0
set ROOT_DIR=%SCRIPT_DIR%..\..
set BUILD_DIR=%ROOT_DIR%\build
set OUTPUT_DIR=%ROOT_DIR%\dist
set PORTABLE_DIR=%OUTPUT_DIR%\MouseCross-Portable

:: Clean and create directories
if exist "%PORTABLE_DIR%" rd /s /q "%PORTABLE_DIR%"
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"
mkdir "%PORTABLE_DIR%"

:: Build in Release mode (icons generated automatically by CMake)
cd "%BUILD_DIR%"
cmake --build . --config Release
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)
cd ..

:: Copy executable and Qt dependencies (deployed by CMake)
echo Copying executable and Qt dependencies...
xcopy "%BUILD_DIR%\Release\*" "%PORTABLE_DIR%\" /E /I /Y

:: Create README for portable version
echo MouseCross Portable Version > "%PORTABLE_DIR%\README.txt"
echo ========================== >> "%PORTABLE_DIR%\README.txt"
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

:: Create ZIP file
cd "%OUTPUT_DIR%"
powershell Compress-Archive -Path "MouseCross-Portable\*" -DestinationPath "MouseCross-v0.1.0-Windows-Portable.zip" -Force

echo.
echo Portable ZIP created: %OUTPUT_DIR%\MouseCross-v0.1.0-Windows-Portable.zip
echo Done!