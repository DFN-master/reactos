@echo off
REM Build script for ReactOS with VS environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cd /d e:\ReactOS\reactos\output-VS-amd64
echo ===== Building reactos.exe with admin page integration =====
ninja base/setup/reactos/reactos.exe
if %errorlevel% equ 0 (
    echo ===== Build successful! =====
    dir base\setup\reactos\reactos.exe
) else (
    echo ===== Build failed with error code %errorlevel% =====
)
pause
