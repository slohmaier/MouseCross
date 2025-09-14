@echo off
setlocal enabledelayedexpansion

echo Simple MouseCross Build Test
echo ============================

:: Set basic variables
set ROOT_DIR=%~dp0..\..
set BUILD_DIR=%ROOT_DIR%\build-test
set ARCH=x64

:: Try to find Qt
if exist "C:\Qt\6.9.1\msvc2022_64" (
    set QT_PATH=C:\Qt\6.9.1\msvc2022_64
    echo Found Qt 6.9.1 x64
) else if exist "C:\Qt\6.9.0\msvc2022_64" (
    set QT_PATH=C:\Qt\6.9.0\msvc2022_64
    echo Found Qt 6.9.0 x64
) else if exist "C:\Qt\6.8.1\msvc2022_64" (
    set QT_PATH=C:\Qt\6.8.1\msvc2022_64
    echo Found Qt 6.8.1 x64
) else if exist "C:\Qt\6.8.0\msvc2022_64" (
    set QT_PATH=C:\Qt\6.8.0\msvc2022_64
    echo Found Qt 6.8.0 x64
) else if exist "C:\Qt\6.7.0\msvc2022_64" (
    set QT_PATH=C:\Qt\6.7.0\msvc2022_64
    echo Found Qt 6.7.0 x64
) else if exist "C:\Qt\6.5.0\msvc2022_64" (
    set QT_PATH=C:\Qt\6.5.0\msvc2022_64
    echo Found Qt 6.5.0 x64
) else (
    echo ERROR: No Qt installation found
    echo Checked:
    echo   - C:\Qt\6.9.1\msvc2022_64
    echo   - C:\Qt\6.9.0\msvc2022_64
    echo   - C:\Qt\6.8.1\msvc2022_64
    echo   - C:\Qt\6.8.0\msvc2022_64
    echo   - C:\Qt\6.7.0\msvc2022_64
    echo   - C:\Qt\6.5.0\msvc2022_64
    echo.
    echo Please install Qt with MSVC 2022 x64 compiler
    pause
    exit /b 1
)

:: Add CMake to PATH if available
if exist "C:\Qt\Tools\CMake_64\bin\cmake.exe" (
    set "PATH=C:\Qt\Tools\CMake_64\bin;%PATH%"
    echo Found CMake in Qt Tools
)

:: Clean and create build directory
if exist "%BUILD_DIR%" rd /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"

:: Configure and build
echo.
echo Configuring with Qt at %QT_PATH%...
cd "%BUILD_DIR%"
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="%QT_PATH%" "%ROOT_DIR%"
if errorlevel 1 (
    echo CMAKE CONFIGURATION FAILED!
    pause
    exit /b 1
)

echo.
echo Building Release configuration...
cmake --build . --config Release
if errorlevel 1 (
    echo BUILD FAILED!
    pause
    exit /b 1
)

echo.
echo SUCCESS! Built MouseCross for %ARCH%
echo Binary location: %BUILD_DIR%\Release\MouseCross.exe
echo.

:: Test if executable exists
if exist "%BUILD_DIR%\Release\MouseCross.exe" (
    echo ✓ Executable created successfully
) else (
    echo ✗ Executable not found!
    exit /b 1
)

pause