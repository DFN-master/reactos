@echo off
setlocal enabledelayedexpansion

echo ===================================
echo Compilando ReactOS 64-bit...
echo Este processo pode levar de 30 minutos a várias horas
echo Por favor, aguarde...
echo ===================================
echo.

echo Configurando ambiente Visual Studio...
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" amd64

echo.
echo Adicionando ferramentas GNU ao PATH...
set PATH=C:\msys64\usr\bin;C:\msys64\ucrt64\bin;%PATH%

echo.
echo Verificando ferramentas...
bison --version
flex --version

echo.
echo Navegando para o diretório de build...
cd output-VS-amd64

echo.
echo Compilando ReactOS 64-bit...
ninja

echo.
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ===================================
    echo Build concluído com sucesso!
    echo ===================================
    echo.
    echo Para criar uma imagem bootável UEFI, execute:
    echo ninja bootcd
) else (
    echo.
    echo ===================================
    echo Build falhou com erros!
    echo Código de erro: %ERRORLEVEL%
    echo ===================================
)
pause
