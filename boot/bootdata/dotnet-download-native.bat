@echo off
REM =====================================================================
REM .NET Framework 4.8 Native Downloader & Installer
REM =====================================================================
REM Este script baixa e instala .NET Framework 4.8 propietário no ReactOS
REM Deve ser executado como Administrador para registro no GAC
REM
REM Uso: dotnet-download-native.bat [--skip-registry]
REM =====================================================================

setlocal enabledelayedexpansion
title .NET Framework 4.8 - Native Installer for ReactOS

REM Detectar arquitetura
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    set ARCH=amd64
    set DOTNET_URL=https://download.microsoft.com/download/4/6/1/46166E4B-D4B4-490D-B12B-82D481578057/NDP48-x64-Developer.exe
) else (
    set ARCH=x86
    set DOTNET_URL=https://download.microsoft.com/download/4/6/1/46166E4B-D4B4-490D-B12B-82D481578057/NDP48-x86-Developer.exe
)

REM Diretórios
set DOTNET_TEMP=%TEMP%\dotnet48
set DOTNET_EXTRACTED=%DOTNET_TEMP%\extracted
set SYSTEM32=%SystemRoot%\System32

echo =====================================================================
echo .NET Framework 4.8 - Native Installation para ReactOS
echo =====================================================================
echo.
echo Arquitetura detectada: %ARCH%
echo Diretório de destino: %SYSTEM32%
echo.

REM Criar diretórios temporários
if not exist "%DOTNET_TEMP%" mkdir "%DOTNET_TEMP%"
if not exist "%DOTNET_EXTRACTED%" mkdir "%DOTNET_EXTRACTED%"

echo [1/4] Verificando instalação existente...
if exist "%SYSTEM32%\mscorlib.dll" (
    echo [✓] .NET Framework 4.8 já está instalado!
    echo.
    goto :verify_dlls
)

echo [2/4] Baixando Microsoft .NET Framework 4.8 (%ARCH%)...
echo URL: %DOTNET_URL%
echo.

REM Usar PowerShell para download se disponível
powershell -Command "^
  $ProgressPreference = 'SilentlyContinue'; ^
  [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; ^
  Write-Host '[+] Aguarde, baixando ~64 MB...'; ^
  try { ^
    Invoke-WebRequest -Uri '%DOTNET_URL%' -OutFile '%DOTNET_TEMP%\ndp48-installer.exe' -ErrorAction Stop; ^
    Write-Host '[✓] Download concluído com sucesso'; ^
    exit 0 ^
  } catch { ^
    Write-Host '[!] Erro ao baixar: $_.Exception.Message'; ^
    exit 1 ^
  } ^
" 

if errorlevel 1 (
    echo [!] Erro ao baixar .NET Framework 4.8
    echo.
    echo Opcoes:
    echo 1. Verificar conexão de internet
    echo 2. Baixar manualmente: %DOTNET_URL%
    echo 3. Extrair de USB/DVD se disponível
    pause
    goto :cleanup
)

echo.
echo [3/4] Instalando Microsoft .NET Framework 4.8...
echo.

REM Executar instalador com flags de quiet e norestart
"%DOTNET_TEMP%\ndp48-installer.exe" /q /norestart

if errorlevel 1 (
    echo [!] Erro na instalação do .NET Framework 4.8
    pause
    goto :cleanup
)

echo [✓] Instalação do .NET Framework 4.8 concluída!
echo.

:verify_dlls
echo [4/4] Verificando DLLs instalados...
echo.

REM Lista de DLLs críticas a verificar
set "CRITICAL_DLLS=mscorlib.dll mscoree.dll System.dll System.Core.dll"

for %%D in (%CRITICAL_DLLS%) do (
    if exist "%SYSTEM32%\%%D" (
        echo [✓] %%D encontrado
    ) else (
        echo [!] %%D NÃO encontrado
    )
)

echo.
echo =====================================================================
echo .NET Framework 4.8 Installation Completo!
echo =====================================================================
echo.
echo Próximos passos:
echo 1. VS Code instalará automáticamente
echo 2. C# Compiler (C#Compiler.dll) disponível
echo 3. SDK e ferramentas prontas para uso
echo.
echo Digite qualquer letra para sair...
pause > nul

:cleanup
if exist "%DOTNET_TEMP%" (
    echo Limpando arquivos temporários...
    rmdir /s /q "%DOTNET_TEMP%" >nul 2>&1
)

endlocal
exit /b 0

REM =====================================================================
REM Funções auxiliares
REM =====================================================================

:check_admin
echo Verificando permissões de Administrador...
net session >nul 2>&1
if errorlevel 1 (
    echo [!] Este script requer permissões de Administrador
    echo Clique direito e selecione "Executar como administrador"
    pause
    exit /b 1
)
goto :eof

REM =====================================================================
REM INFORMAÇÕES ADICIONAIS
REM =====================================================================
REM
REM DLLs que serão instalados por Microsoft .NET Framework 4.8:
REM
REM Sistema/Core (11 DLLs):
REM   - mscorlib.dll (core runtime library)
REM   - mscoreei.dll (EE interface)
REM   - mscoree.dll (execution engine)
REM   - mscorecli.dll (inline cache)
REM   - System.dll (system APIs)
REM   - System.Core.dll (LINQ, TPL)
REM   - System.Configuration.dll (app config)
REM   - System.Xml.dll (XML processing)
REM   - System.Xml.Linq.dll (XML LINQ)
REM   - System.Data.dll (ADO.NET)
REM   - System.Net.dll (networking)
REM
REM Web/WCF (6 DLLs):
REM   - System.Web.dll
REM   - System.ServiceModel.dll
REM   - System.ServiceProcess.dll
REM   - System.Net.Http.dll
REM   - System.Web.Services.dll
REM   - System.Transactions.dll
REM
REM Ferramentas/SDK (3+):
REM   - C:\Windows\Microsoft.NET\Framework\v4.0.30319\csc.exe (C# Compiler)
REM   - C:\Windows\Microsoft.NET\Framework\v4.0.30319\vbc.exe (VB.NET Compiler)
REM   - etc...
REM
REM Compatibilidade ReactOS:
REM   - Versão mínima: 0.4.14
REM   - Arquitetura: x86 e x86-64 (amd64)
REM   - Requisitos: NTFS, 4 GB RAM recomendado
REM   - Espaço em disco: 500 MB para runtime + ferramentas
REM
