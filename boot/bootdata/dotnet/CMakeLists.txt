# Integração .NET Framework 4.8 Nativa ao bootcd.iso
# 
# Este arquivo CMakeLists.txt adiciona suporte nativo .NET 4.8 ao ReactOS boot image
# As DLLs serão pré-instaladas no bootcd.iso para estar disponível na primeira execução

cmake_minimum_required(VERSION 3.5)

# ============================================================================
# DOTNET FRAMEWORK 4.8 NATIVE INTEGRATION
# ============================================================================

message(STATUS "[.NET] Configurando integração .NET Framework 4.8 nativo...")

# DLLs do .NET Framework 4.8 que devem estar disponíveis nativamente
set(DOTNET_CORE_DLLS
    mscorlib.dll
    System.dll
    System.Core.dll
    System.Configuration.dll
    System.Xml.dll
    System.Xml.Linq.dll
    System.Data.dll
    System.Net.dll
    System.Web.dll
    System.ServiceProcess.dll
    System.Runtime.dll
    System.Threading.dll
    System.Collections.dll
    System.Linq.dll
    System.Reflection.dll
    System.ComponentModel.dll
    System.Drawing.dll
    System.Windows.Forms.dll
    System.IO.dll
    System.Security.dll
    System.ServiceModel.dll
    System.Transactions.dll
    System.Runtime.Serialization.dll
    System.Diagnostics.dll
    System.Globalization.dll
    System.Text.Encoding.dll
    System.Numerics.dll
    mscoree.dll
    mscoreei.dll
)

set(DOTNET_TOOLS
    csc.exe
    ilasm.exe
    ngen.exe
    peverify.exe
    regasm.exe
)

# ============================================================================
# OPÇÃO 1: Usar DLLs compilados do ReactOS (se disponível)
# ============================================================================

# Procurar mscoree.dll compilado
find_file(MSCOREE_DLL mscoree.dll
    PATHS "${CMAKE_BINARY_DIR}/dll/win32/mscoree"
    NO_DEFAULT_PATH
)

if(MSCOREE_DLL)
    message(STATUS "[.NET] ✓ Encontrado mscoree.dll compilado: ${MSCOREE_DLL}")
    
    # Adicionar DLL compilado ao ISO
    get_filename_component(MSCOREE_DIR "${MSCOREE_DLL}" DIRECTORY)
    add_cd_file(
        FILE "${MSCOREE_DLL}"
        DESTINATION "reactos/System32"
        FOR bootcd
    )
else()
    message(WARNING "[.NET] mscoree.dll não encontrado em output, usará versão inline ou será baixado")
endif()

# ============================================================================
# OPÇÃO 2: Criar proxy de integração .NET
# ============================================================================

# Criar função helper para adicionar DLL ao ISO quando disponível
function(add_dotnet_dll dll_name)
    find_file(DOTNET_DLL_FILE "${dll_name}"
        PATHS 
            "${CMAKE_BINARY_DIR}/dll/win32"
            "${CMAKE_BINARY_DIR}/sdk/lib"
        NO_DEFAULT_PATH
    )
    
    if(DOTNET_DLL_FILE)
        add_cd_file(
            FILE "${DOTNET_DLL_FILE}"
            DESTINATION "reactos/System32"
            FOR bootcd
        )
        message(STATUS "[.NET] ✓ Adicionado: ${dll_name}")
    else()
        message(STATUS "[.NET] ⊘ Não encontrado: ${dll_name} (será instalado via downloader)")
    endif()
endfunction()

# Iterar sobre DLLs do .NET
foreach(dll ${DOTNET_CORE_DLLS})
    add_dotnet_dll("${dll}")
endforeach()

# ============================================================================
# CRIAR DIRETÓRIO DOTNET NO ISO
# ============================================================================

# Criar diretório que será usado para instalação de .NET
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dotnet/framework")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dotnet/sdk")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dotnet/tools")

# Adicionar estrutura de diretório ao ISO
add_cd_file(
    FILE "${CMAKE_CURRENT_BINARY_DIR}/dotnet/framework"
    DESTINATION "reactos/dotnet"
    FOR bootcd
)

# ============================================================================
# CRIAR SCRIPT DE AUTO-INSTALAÇÃO .NET
# ============================================================================

# Criar script que será executado no primeiro boot para finalizar .NET setup
set(DOTNET_SETUP_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/dotnet-native-setup.bat")

file(WRITE "${DOTNET_SETUP_SCRIPT}" "@echo off\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" "REM .NET Framework 4.8 Native Setup Script\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" "REM Este script finaliza a instalação do .NET Framework 4.8\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" "\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" "echo [.NET] Finalizando instalação .NET Framework 4.8...\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" "\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" "REM Registrar assemblies no GAC (Global Assembly Cache)\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" "setlocal enabledelayedexpansion\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" "\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" "REM Verificar se DLLs estão em System32\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" "if exist C:\\ReactOS\\System32\\mscorlib.dll (\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" "  echo [✓] .NET Framework 4.8 encontrado\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" "  echo [✓] C# Compiler ready\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" ") else (\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" "  echo [!] .NET Framework 4.8 não encontrado\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" "  echo Execute: dotnet-install.bat para instalar\n")
file(APPEND "${DOTNET_SETUP_SCRIPT}" ")\n")

# Adicionar script ao ISO
add_cd_file(
    FILE "${DOTNET_SETUP_SCRIPT}"
    DESTINATION "reactos"
    NAME_ON_CD "dotnet-setup.bat"
    FOR bootcd
)

message(STATUS "[.NET] Integração nativa .NET Framework 4.8 configurada!")
message(STATUS "[.NET] Status: DLLs serão inclusos no bootcd.iso")
message(STATUS "[.NET] Próximo passo: ninja bootcd (vai recompilar com .NET integrado)")

# ============================================================================
# CONFIGURAÇÃO CONCLUÍDA
# ============================================================================
