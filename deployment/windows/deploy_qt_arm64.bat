@echo off
setlocal enabledelayedexpansion

:: Custom Qt deployment script for ARM64
:: This script manually deploys Qt libraries and plugins for ARM64 builds
:: since windeployqt doesn't support ARM64 cross-platform deployment

set "EXECUTABLE_PATH=%~1"
set "QT_ARM64_PATH=%~2"

if "%EXECUTABLE_PATH%"=="" (
    echo Usage: deploy_qt_arm64.bat ^<executable_path^> ^<qt_arm64_path^>
    exit /b 1
)

if "%QT_ARM64_PATH%"=="" (
    echo Usage: deploy_qt_arm64.bat ^<executable_path^> ^<qt_arm64_path^>
    exit /b 1
)

echo Deploying Qt ARM64 libraries to %EXECUTABLE_PATH%...
echo Qt ARM64 path: %QT_ARM64_PATH%

:: Get the directory containing the executable
for %%F in ("%EXECUTABLE_PATH%") do set "TARGET_DIR=%%~dpF"

:: Core Qt libraries needed for MouseCross
echo Copying core Qt libraries...
copy "%QT_ARM64_PATH%\bin\Qt6Core.dll" "%TARGET_DIR%"
copy "%QT_ARM64_PATH%\bin\Qt6Gui.dll" "%TARGET_DIR%"
copy "%QT_ARM64_PATH%\bin\Qt6Widgets.dll" "%TARGET_DIR%"
copy "%QT_ARM64_PATH%\bin\Qt6Network.dll" "%TARGET_DIR%"
copy "%QT_ARM64_PATH%\bin\Qt6Svg.dll" "%TARGET_DIR%"

:: Create plugin directories and copy essential plugins
echo Copying Qt plugins...

:: Platforms (essential for GUI)
mkdir "%TARGET_DIR%platforms" >nul 2>&1
copy "%QT_ARM64_PATH%\plugins\platforms\qwindows.dll" "%TARGET_DIR%platforms\"

:: Styles (for native Windows appearance)
mkdir "%TARGET_DIR%styles" >nul 2>&1
if exist "%QT_ARM64_PATH%\plugins\styles\qmodernwindowsstyle.dll" (
    copy "%QT_ARM64_PATH%\plugins\styles\qmodernwindowsstyle.dll" "%TARGET_DIR%styles\" >nul
)
if exist "%QT_ARM64_PATH%\plugins\styles\qwindowsvistastyle.dll" (
    copy "%QT_ARM64_PATH%\plugins\styles\qwindowsvistastyle.dll" "%TARGET_DIR%styles\" >nul
)

:: Image formats (for PNG/ICO icons)
mkdir "%TARGET_DIR%imageformats" >nul 2>&1
copy "%QT_ARM64_PATH%\plugins\imageformats\qico.dll" "%TARGET_DIR%imageformats\" >nul
copy "%QT_ARM64_PATH%\plugins\imageformats\qjpeg.dll" "%TARGET_DIR%imageformats\" >nul
copy "%QT_ARM64_PATH%\plugins\imageformats\qpng.dll" "%TARGET_DIR%imageformats\" >nul 2>nul || (
    copy "%QT_ARM64_PATH%\plugins\imageformats\qgif.dll" "%TARGET_DIR%imageformats\" >nul 2>nul
)
copy "%QT_ARM64_PATH%\plugins\imageformats\qsvg.dll" "%TARGET_DIR%imageformats\" >nul

:: Icon engines (for SVG icons)
mkdir "%TARGET_DIR%iconengines" >nul 2>&1
copy "%QT_ARM64_PATH%\plugins\iconengines\qsvgicon.dll" "%TARGET_DIR%iconengines\" >nul

:: TLS plugins (for network functionality)
mkdir "%TARGET_DIR%tls" >nul 2>&1
copy "%QT_ARM64_PATH%\plugins\tls\qcertonlybackend.dll" "%TARGET_DIR%tls\" >nul 2>nul
copy "%QT_ARM64_PATH%\plugins\tls\qschannelbackend.dll" "%TARGET_DIR%tls\" >nul 2>nul

:: Network information
mkdir "%TARGET_DIR%networkinformation" >nul 2>&1
copy "%QT_ARM64_PATH%\plugins\networkinformation\qnetworklistmanager.dll" "%TARGET_DIR%networkinformation\" >nul 2>nul

:: Generic plugins
mkdir "%TARGET_DIR%generic" >nul 2>&1
copy "%QT_ARM64_PATH%\plugins\generic\qtuiotouchplugin.dll" "%TARGET_DIR%generic\" >nul 2>nul

:: Count deployed files
set /a DLL_COUNT=0
for %%F in ("%TARGET_DIR%*.dll") do set /a DLL_COUNT+=1

set /a PLUGIN_COUNT=0
for /r "%TARGET_DIR%" %%F in (*.dll) do (
    if not "%%~nxF"=="%%~nF" set /a PLUGIN_COUNT+=1
)
set /a PLUGIN_COUNT-=DLL_COUNT

echo.
echo ARM64 Qt deployment completed:
echo - %DLL_COUNT% core libraries deployed
echo - %PLUGIN_COUNT% plugin libraries deployed
echo - Target directory: %TARGET_DIR%
echo.

:: Verify critical files exist
if not exist "%TARGET_DIR%Qt6Core.dll" (
    echo ERROR: Qt6Core.dll not found! Deployment may have failed.
    exit /b 1
)
if not exist "%TARGET_DIR%platforms\qwindows.dll" (
    echo ERROR: qwindows.dll not found! Application may not start.
    exit /b 1
)

echo ARM64 Qt libraries successfully deployed.