@echo off
setlocal
echo [1/4] Configurando MSVC...
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" amd64
if errorlevel 1 goto :err

echo [2/4] Ajustando PATH (msys tools)...
set PATH=C:\msys64\usr\bin;C:\msys64\ucrt64\bin;%PATH%

echo [3/4] Entrando no build dir...
cd /d e:\ReactOS\reactos\output-VS-amd64
if errorlevel 1 goto :err

echo [4/4] Gerando bootcd.iso...
ninja bootcd
if errorlevel 1 goto :err

echo BUILD_OK
exit /b 0

:err
echo BUILD_FAIL_%errorlevel%
exit /b %errorlevel%
