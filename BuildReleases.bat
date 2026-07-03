@echo off
setlocal

pushd "%~dp0"

powershell.exe ^
    -NoProfile ^
    -ExecutionPolicy Bypass ^
    -File "%~dp0BuildReleases.ps1"

set EXIT_CODE=%ERRORLEVEL%

popd

if %EXIT_CODE% NEQ 0 (
    echo.
    echo ========================================
    echo Build failed with exit code %EXIT_CODE%.
    echo ========================================
    pause
    exit /b %EXIT_CODE%
)

echo.
echo ========================================
echo Build completed successfully.
echo ========================================
pause
exit /b 0