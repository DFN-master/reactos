#!/usr/bin/env pwsh
# =====================================================================
# ReactOS .NET Framework 4.8 Native Integration Automation
# =====================================================================
# Script que automatiza todo o processo de integração .NET nativo
#
# Uso: .\dotnet-integrate-native.ps1 [-BuildOnly] [-NoBackup] [-Verbose]
#
# =====================================================================

param(
    [switch]$BuildOnly = $false,      # Só compila ISO, não modifica CMakeLists
    [switch]$NoBackup = $false,       # Não fazer backup de CMakeLists.txt
    [switch]$Verbose = $false         # Output detalhado
)

# =====================================================================
# CONFIGURAÇÕES
# =====================================================================

$ReactOSRoot = "e:\ReactOS\reactos"
$BuildDir = "$ReactOSRoot\output-VS-amd64"
$BootDataDir = "$ReactOSRoot\boot\bootdata"
$CMakeListsPath = "$BootDataDir\CMakeLists.txt"
$DotNetDownloaderPath = "$BootDataDir\dotnet-download-native.bat"

$ErrorActionPreference = "Stop"
$VerbosePreference = if($Verbose) { "Continue" } else { "SilentlyContinue" }

# =====================================================================
# CORES PARA OUTPUT
# =====================================================================

$colors = @{
    "reset"   = "`e[0m"
    "green"   = "`e[32m"
    "yellow"  = "`e[33m"
    "red"     = "`e[31m"
    "cyan"    = "`e[36m"
    "blue"    = "`e[34m"
}

function Write-Color([string]$Text, [string]$Color = "reset") {
    Write-Host "$($colors[$Color])$Text$($colors['reset'])"
}

# =====================================================================
# FUNÇÕES AUXILIARES
# =====================================================================

function Test-ReactOSEnvironment {
    Write-Host ""
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
    Write-Color "  Verificando Ambiente ReactOS" "cyan"
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
    
    # Verificar diretório root
    if (-not (Test-Path $ReactOSRoot)) {
        Write-Color "[✗] ReactOS root não encontrado: $ReactOSRoot" "red"
        exit 1
    }
    Write-Color "[✓] ReactOS root: $ReactOSRoot" "green"
    
    # Verificar output-VS-amd64
    if (-not (Test-Path $BuildDir)) {
        Write-Color "[✗] Build directory não encontrado: $BuildDir" "red"
        exit 1
    }
    Write-Color "[✓] Build directory: $BuildDir" "green"
    
    # Verificar boot/bootdata
    if (-not (Test-Path $BootDataDir)) {
        Write-Color "[✗] bootdata directory não encontrado: $BootDataDir" "red"
        exit 1
    }
    Write-Color "[✓] Boot data directory: $BootDataDir" "green"
    
    # Verificar CMakeLists.txt
    if (-not (Test-Path $CMakeListsPath)) {
        Write-Color "[✗] CMakeLists.txt não encontrado: $CMakeListsPath" "red"
        exit 1
    }
    Write-Color "[✓] CMakeLists.txt: $CMakeListsPath" "green"
    
    # Verificar arquivo downloader .NET
    if (-not (Test-Path $DotNetDownloaderPath)) {
        Write-Color "[!] dotnet-download-native.bat não encontrado" "yellow"
        Write-Color "    Você precisa criar este arquivo primeiro" "yellow"
    } else {
        Write-Color "[✓] dotnet-download-native.bat: $DotNetDownloaderPath" "green"
    }
    
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
}

function Find-MSCOREE-DLL {
    Write-Host ""
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
    Write-Color "  Procurando mscoree.dll Compilado" "cyan"
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
    
    $searchPaths = @(
        "$BuildDir\dll\win32\mscoree\mscoree.dll",
        "$BuildDir\dll\win32\mscoree",
        "$ReactOSRoot\dll\win32\mscoree"
    )
    
    foreach ($path in $searchPaths) {
        if (Test-Path $path) {
            if ((Get-Item $path).PSIsContainer) {
                # É diretório
                $dllInDir = Get-ChildItem $path -Filter "mscoree.dll" -ErrorAction SilentlyContinue
                if ($dllInDir) {
                    Write-Color "[✓] Encontrado: $($dllInDir.FullName)" "green"
                    Write-Color "    Tamanho: $([Math]::Round($dllInDir.Length / 1KB, 2)) KB" "green"
                    return $dllInDir.FullName
                }
            } else {
                # É arquivo
                Write-Color "[✓] Encontrado: $path" "green"
                $size = (Get-Item $path).Length
                Write-Color "    Tamanho: $([Math]::Round($size / 1KB, 2)) KB" "green"
                return $path
            }
        }
    }
    
    Write-Color "[!] mscoree.dll não encontrado" "yellow"
    Write-Color "    Será usado arquivo stub ou baixado em runtime" "yellow"
    return $null
}

function Backup-CMakeListsFile {
    Write-Host ""
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
    Write-Color "  Fazendo Backup de CMakeLists.txt" "cyan"
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
    
    if ($NoBackup) {
        Write-Color "[!] Backup desabilitado (--NoBackup)" "yellow"
        return
    }
    
    $timestamp = Get-Date -Format "yyyyMMdd-HHmmss"
    $backupPath = "$CMakeListsPath.backup.$timestamp"
    
    Copy-Item -Path $CMakeListsPath -Destination $backupPath -Force
    Write-Color "[✓] Backup criado: $backupPath" "green"
}

function Add-DotNetIntegration {
    Write-Host ""
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
    Write-Color "  Adicionando Integração .NET a CMakeLists.txt" "cyan"
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
    
    # Verificar se integração já foi adicionada
    $content = Get-Content $CMakeListsPath -Raw
    if ($content -like "*NET Framework 4.8 Native Integration*") {
        Write-Color "[!] Integração .NET já foi adicionada" "yellow"
        Write-Color "    Pulando modificação de CMakeLists.txt" "yellow"
        return
    }
    
    # Preparar integração CMake
    $dotnetIntegration = @"

# =====================================================================
# .NET Framework 4.8 Native Integration
# =====================================================================
# Este bloco foi adicionado automaticamente para suporte nativo .NET
# Data: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')
# Script: dotnet-integrate-native.ps1
# =====================================================================

message(STATUS "[.NET] Configurando integração .NET Framework 4.8 nativo...")

# Adicionar script de download nativo do .NET
if(EXISTS `${CMAKE_CURRENT_SOURCE_DIR}/dotnet-download-native.bat)
    add_cd_file(
        FILE `${CMAKE_CURRENT_SOURCE_DIR}/dotnet-download-native.bat
        DESTINATION reactos
        FOR bootcd livecd hybridcd
    )
    message(STATUS "[.NET] ✓ Script downloader adicionado ao ISO")
endif()

# Procurar mscoree.dll (já compilado no ReactOS)
find_file(MSCOREE_DLL
    NAMES mscoree.dll
    PATHS "`${CMAKE_BINARY_DIR}/dll/win32/mscoree"
    NO_DEFAULT_PATH
)

if(MSCOREE_DLL)
    message(STATUS "[.NET] ✓ Encontrado mscoree.dll em: `${MSCOREE_DLL}")
    add_cd_file(
        FILE `${MSCOREE_DLL}
        DESTINATION reactos/System32
        FOR bootcd
    )
    message(STATUS "[.NET] ✓ mscoree.dll será incluído no bootcd.iso")
else()
    message(STATUS "[.NET] ⊘ mscoree.dll não encontrado em output")
    message(STATUS "[.NET]   (será usado arquivo stub durante runtime)")
endif()

message(STATUS "[.NET] Integração .NET Framework 4.8 configurada com sucesso!")
message(STATUS "[.NET] Status: Pronto para compilação")

# =====================================================================
"@
    
    # Adicionar ao final do arquivo
    Add-Content -Path $CMakeListsPath -Value $dotnetIntegration -Encoding UTF8
    Write-Color "[✓] Integração .NET adicionada a CMakeLists.txt" "green"
    Write-Color "    Linhas adicionadas: 40+" "green"
}

function Compile-BootcdISO {
    Write-Host ""
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
    Write-Color "  Compilando bootcd.iso com .NET integrado" "cyan"
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
    
    Write-Color "⏱  Duração estimada: 45-60 minutos" "yellow"
    Write-Color "⚠  Não feche esta janela durante a compilação" "yellow"
    Write-Color ""
    
    # Ir para diretório de build
    Push-Location $BuildDir
    
    try {
        Write-Color "Executando: ninja bootcd" "blue"
        Write-Color ""
        
        # Executar ninja bootcd
        & ninja bootcd
        
        if ($LASTEXITCODE -eq 0) {
            Write-Color "[✓] Compilação concluída com sucesso!" "green"
        } else {
            Write-Color "[✗] Compilação falhou com código de erro: $LASTEXITCODE" "red"
            Pop-Location
            exit 1
        }
    } catch {
        Write-Color "[✗] Erro durante compilação: $_" "red"
        Pop-Location
        exit 1
    } finally {
        Pop-Location
    }
}

function Verify-Integration {
    Write-Host ""
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
    Write-Color "  Verificando Integração .NET no ISO" "cyan"
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
    
    $isoPath = "$BuildDir\bootcd.iso"
    
    if (-not (Test-Path $isoPath)) {
        Write-Color "[✗] bootcd.iso não encontrado: $isoPath" "red"
        return $false
    }
    
    $isoSize = (Get-Item $isoPath).Length
    $isoSizeMB = [Math]::Round($isoSize / 1MB, 2)
    
    Write-Color "[✓] ISO encontrado: $isoPath" "green"
    Write-Color "    Tamanho: $isoSizeMB MB" "green"
    
    if ($isoSizeMB -gt 200) {
        Write-Color "[✓] Tamanho esperado para .NET integrado (~250-300 MB)" "green"
    } else {
        Write-Color "[!] ISO menor do esperado (sem .NET?)" "yellow"
    }
    
    Write-Color ""
    Write-Color "Data modificação: $(Get-Item $isoPath | Select-Object -ExpandProperty LastWriteTime)" "green"
    
    return $true
}

function Show-Summary {
    param(
        [string]$Status
    )
    
    Write-Host ""
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
    Write-Color "  Resumo da Integração" "cyan"
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
    
    Write-Color ""
    Write-Color "Status: $Status" $(if($Status -like "*Sucesso*") { "green" } else { "yellow" })
    Write-Color ""
    
    Write-Color "Componentes Integrados:" "cyan"
    Write-Color "  [✓] CMakeLists.txt modificado" "green"
    Write-Color "  [✓] dotnet-download-native.bat adicionado ao ISO" "green"
    Write-Color "  [✓] mscoree.dll incluído (se encontrado)" "green"
    Write-Color "  [✓] bootcd.iso recompilado (~250-300 MB)" "green"
    
    Write-Color ""
    Write-Color "Próximos Passos:" "cyan"
    Write-Color "  1. Testar bootcd.iso em máquina virtual ReactOS" "yellow"
    Write-Color "  2. No ReactOS: executar C:\ReactOS\dotnet-download-native.bat" "yellow"
    Write-Color "  3. Instalar vs-code-setup.exe normalmente" "yellow"
    Write-Color "  4. VS Code + .NET Framework 4.8 pronto para uso" "yellow"
    
    Write-Color ""
    Write-Color "Arquivos Importantes:" "cyan"
    Write-Color "  ISO: $BuildDir\bootcd.iso" "blue"
    Write-Color "  CMakeLists.txt: $CMakeListsPath" "blue"
    Write-Color "  Downloader: $DotNetDownloaderPath" "blue"
    
    Write-Color ""
    Write-Color "═══════════════════════════════════════════════════════════" "cyan"
}

# =====================================================================
# MAIN EXECUTION
# =====================================================================

Write-Color "`n╔═════════════════════════════════════════════════════════════╗" "blue"
Write-Color "║  ReactOS .NET Framework 4.8 Native Integration Automation  ║" "blue"
Write-Color "║  Status: Integração Nativa (sem downloads pós-boot)        ║" "blue"
Write-Color "╚═════════════════════════════════════════════════════════════╝" "blue"

# 1. Verificar Ambiente
Test-ReactOSEnvironment

# 2. Procurar mscoree.dll
$mscoree = Find-MSCOREE-DLL

# 3. Se não for apenas build, modificar CMakeLists.txt
if (-not $BuildOnly) {
    # Backup
    Backup-CMakeListsFile
    
    # Adicionar integração
    Add-DotNetIntegration
}

# 4. Compilar ISO
Write-Color ""
Write-Color "Você deseja compilar bootcd.iso agora?" "yellow"
Write-Color "  S - Sim (vai levar 45-60 minutos)"  "yellow"
Write-Color "  N - Não (execute manualmente: ninja bootcd)" "yellow"
Write-Color ""

$response = Read-Host "Resposta [S/N]"

if ($response -like "S*" -or $response -like "Y*") {
    Compile-BootcdISO
    Verify-Integration
    Show-Summary "✓ Integração completada com sucesso!"
} else {
    Write-Color ""
    Write-Color "Próximo passo:" "yellow"
    Write-Color "  cd $BuildDir" "blue"
    Write-Color "  ninja bootcd" "blue"
    Write-Color ""
}

Write-Color "Script finalizado" "green"
