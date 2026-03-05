@echo off
REM Script para criar imagem bootável UEFI do ReactOS 64-bit

echo Configurando ambiente Visual Studio...
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" amd64

echo Adicionando ferramentas ao PATH...
set PATH=C:\msys64\usr\bin;%PATH%

echo.
echo ===================================
echo Criando imagem bootável UEFI...
echo Este processo pode levar de 5 a 15 minutos
echo ===================================
echo.

cd output-VS-amd64

echo Gerando bootcd.iso...
ninja bootcd

echo.
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ===================================
    echo IMAGEM UEFI CRIADA COM SUCESSO!
    echo ===================================
    echo.
    echo A imagem foi salva em:
    echo   E:\ReactOS\reactos\output-VS-amd64\bootcd.iso
    echo.
    echo Você pode agora:
    echo   1. Queimar em DVD/USB para testar em hardware real
    echo   2. Usar em uma máquina virtual (VirtualBox, QEMU, Hyper-V)
    echo   3. Instalar em uma partição FAT32
) else (
    echo.
    echo ===================================
    echo ERRO ao criar imagem!
    echo Código de erro: %ERRORLEVEL%
    echo ===================================
)
pause
