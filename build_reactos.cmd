@echo off
echo Configurando ambiente Visual Studio...
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x86

echo.
echo Adicionando ferramentas ao PATH...
set PATH=%PATH%;C:\Program Files (x86)\GnuWin32\bin

echo.
echo Navegando para o diretório de build...
cd output-VS-i386

echo.
echo Compilando ReactOS... (isso pode levar de 30 minutos a várias horas)
echo Por favor, aguarde...
ninja

echo.
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ===================================
    echo Build concluído com sucesso!
    echo ===================================
    echo.
    echo Para criar uma imagem bootável, execute:
    echo ninja bootcd
) else (
    echo.
    echo ===================================
    echo Build falhou com erros!
    echo ===================================
)
pause
