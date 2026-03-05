@echo off
echo ===================================
echo Build ReactOS 64-bit (amd64) UEFI
echo ===================================
echo.
echo Configurando ambiente Visual Studio para x64...
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" amd64

echo.
echo Adicionando ferramentas ao PATH...
set PATH=%PATH%;C:\Program Files (x86)\GnuWin32\bin

echo.
echo Navegando para o diretório de build amd64...
cd output-VS-amd64

echo.
echo ===================================
echo Compilando ReactOS 64-bit...
echo Este processo pode levar de 30 minutos a várias horas
echo Por favor, aguarde...
echo ===================================
echo.
ninja

echo.
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ===================================
    echo Build 64-bit concluído com sucesso!
    echo ===================================
    echo.
    echo Para criar uma imagem bootável UEFI, execute:
    echo ninja bootcd
    echo.
    echo O arquivo bootcd.iso será criado com suporte a UEFI
) else (
    echo.
    echo ===================================
    echo Build falhou com erros!
    echo Código de erro: %ERRORLEVEL%
    echo ===================================
)
echo.
pause
