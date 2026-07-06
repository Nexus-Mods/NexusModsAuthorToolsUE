@echo off
setlocal

pushd "%~dp0"

powershell.exe ^
    -NoProfile ^
    -ExecutionPolicy Bypass ^
    -File "%~dp0Scripts\BuildToolsMenu.ps1"

set EXIT_CODE=%ERRORLEVEL%

popd

if %EXIT_CODE% NEQ 0 (
    echo.
    echo ========================================
    echo Build tools exited with code %EXIT_CODE%.
    echo ========================================
    pause
    exit /b %EXIT_CODE%
)

exit /b 0
