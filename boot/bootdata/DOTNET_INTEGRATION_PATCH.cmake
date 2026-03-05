# =====================================================================
# INTEGRAÇÃO .NET FRAMEWORK 4.8 NATIVO - Adições ao bootdata/CMakeLists.txt
# =====================================================================
# 
# INSTRUÇÕES: Adicionar estas linhas ao final de boot/bootdata/CMakeLists.txt
#
# Este arquivo contém as configurações para incluir .NET Framework 4.8
# nativamente no bootcd.iso durante a compilação
#
# =====================================================================

# Include .NET Framework 4.8 Native Integration Support
# (Descomente a linha abaixo após criar o diretório dotnet/)
# add_subdirectory(dotnet)

# =====================================================================
# .NET FRAMEWORK 4.8 NATIVE INTEGRATION
# =====================================================================

# Adicionar script de download nativo do .NET
add_cd_file(
    FILE ${CMAKE_CURRENT_SOURCE_DIR}/dotnet-download-native.bat
    DESTINATION reactos
    FOR bootcd livecd hybridcd
)

message(STATUS "[.NET] Adicionando suporte .NET Framework 4.8 nativo ao bootcd.iso...")

# ===================================================================
# SEÇÃO 1: Procurar DLLs compilados no ReactOS output
# ===================================================================

# Procurar mscoree.dll (já compilado)
find_file(MSCOREE_DLL
    NAMES mscoree.dll
    PATHS "${CMAKE_BINARY_DIR}/dll/win32/mscoree"
    NO_DEFAULT_PATH
)

if(MSCOREE_DLL)
    message(STATUS "[.NET] ✓ Encontrado mscoree.dll em: ${MSCOREE_DLL}")
    add_cd_file(
        FILE ${MSCOREE_DLL}
        DESTINATION reactos/System32
        FOR bootcd
    )
else()
    message(STATUS "[.NET] ⊘ mscoree.dll não encontrado (será baixado em runtime)")
endif()

# ===================================================================
# SEÇÃO 2: Procurar outros DLLs do .NET já compilados no ReactOS
# ===================================================================

# Lista de DLLs do .NET que podem estar compilados no ReactOS
set(DOTNET_DLLS_TO_SEARCH
    "System.dll"
    "System.Core.dll"
    "System.Data.dll"
    "System.Xml.dll"
    "System.Net.dll"
)

foreach(dll_name ${DOTNET_DLLS_TO_SEARCH})
    find_file(DOTNET_DLL_FOUND
        NAMES ${dll_name}
        PATHS "${CMAKE_BINARY_DIR}/dll/win32"
        NO_DEFAULT_PATH
    )
    
    if(DOTNET_DLL_FOUND)
        message(STATUS "[.NET] ✓ Encontrado ${dll_name}")
        add_cd_file(
            FILE ${DOTNET_DLL_FOUND}
            DESTINATION reactos/System32
            FOR bootcd
        )
        unset(DOTNET_DLL_FOUND CACHE)
    else()
        message(STATUS "[.NET] ⊘ ${dll_name} não encontrado (será baixado)")
    endif()
endforeach()

# ===================================================================
# SEÇÃO 3: Criar diretório de bootstrap do .NET
# ===================================================================

# Criar diretório que será usado para armazenar DLLs do .NET no ISO
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dotnet-bootstrap")

# Este diretório será incluído no ISO para facilitar instalação manual
# se necessário

# ===================================================================
# SEÇÃO 4: Adicionar script de setup inicial
# ===================================================================

# Criar script batch que será executado no primeiro boot
set(DOTNET_FIRST_BOOT_SETUP "${CMAKE_CURRENT_BINARY_DIR}/dotnet-first-boot-setup.bat")

file(WRITE "${DOTNET_FIRST_BOOT_SETUP}" 
"@echo off
REM .NET Framework 4.8 First Boot Setup
REM Execute automaticamente na primeira inicialização do ReactOS

setlocal enabledelayedexpansion

echo Verificando .NET Framework 4.8...
if exist C:\\ReactOS\\System32\\mscorlib.dll (
    echo [OK] .NET Framework 4.8 já instalado
    exit /b 0
)

echo [INFO] Para instalar .NET Framework 4.8:
echo Execute: C:\\ReactOS\\dotnet-download-native.bat
echo.

endlocal
exit /b 0
")

add_cd_file(
    FILE "${DOTNET_FIRST_BOOT_SETUP}"
    DESTINATION reactos/startup
    FOR bootcd
)

# ===================================================================
# SEÇÃO 5: Crear manifesto de .NET Framework
# ===================================================================

# Criar arquivo de manifesto que lista quais DLLs serão necessários
set(DOTNET_MANIFEST "${CMAKE_CURRENT_BINARY_DIR}/dotnet-manifest.txt")

file(WRITE "${DOTNET_MANIFEST}"
".NET Framework 4.8 Nativo para ReactOS
==========================================

Versão: 4.8
Arquitetura: amd64 / x86
Destino: System32 (C:\\Windows\\System32)

DLLs Essenciais:
================
1. mscorlib.dll         - Core Runtime Library
2. mscoree.dll          - Execution Engine
3. mscoreei.dll         - EE Interface
4. System.dll           - System APIs
5. System.Core.dll      - LINQ, TPL, async/await
6. System.Configuration.dll - App configuration
7. System.Xml.dll       - XML processing
8. System.Data.dll      - ADO.NET database
9. System.Net.dll       - Networking APIs
10. System.IO.dll       - File I/O operations
11. System.Text.Encoding.dll - Character encoding
12. System.Reflection.dll    - Reflection APIs
13. System.Threading.dll     - Thread management
14. System.Collections.dll   - Collection classes
15. System.Linq.dll          - LINQ to Objects
16. System.Diagnostics.dll   - Performance counters
17. System.Globalization.dll - Culture info
18. System.Collections.Concurrent.dll - Thread-safe collections
19. System.Threading.Tasks.dll - Task Parallel Library
20. System.Transactions.dll    - Transaction support
21. System.Runtime.dll         - Runtime APIs
22. System.Runtime.InteropServices.dll - P/Invoke support
23. System.Security.dll        - Security APIs
24. System.Security.Cryptography.dll - Crypto
25. System.ComponentModel.dll   - Component model
26. System.ComponentModel.DataAnnotations.dll - Data validation

Ferramentas C#:
===============
- csc.exe                - C# Compiler (v4.8)
- csc.rsp                - Compiler config
- System.CodeDom.dll      - Code generation
- Microsoft.CSharp.dll    - C# runtime binder

Requisitos:
===========
- Mínimo: 4 GB RAM
- Espaço em disco: 500 MB
- ReactOS 0.4.14 ou superior

Status de Instalação:
====================
[ ] Verificado
[ ] Baixado
[ ] Instalado
[ ] Testado

Data de instalação: __________
")

add_cd_file(
    FILE "${DOTNET_MANIFEST}"
    DESTINATION reactos/dotnet
    NAME_ON_CD "DOTNET_MANIFEST.txt"
    FOR bootcd
)

# ===================================================================
# RESUMO DA INTEGRAÇÃO
# ===================================================================

message(STATUS "")
message(STATUS "=== .NET Framework 4.8 Native Integration ===")
message(STATUS "[✓] Arquivos adicionados ao bootcd.iso:")
message(STATUS "    - dotnet-download-native.bat (downloader automático)")
message(STATUS "    - dotnet-first-boot-setup.bat (setup na boot)")
message(STATUS "    - DOTNET_MANIFEST.txt (documentação)")
if(MSCOREE_DLL)
    message(STATUS "    - mscoree.dll (compilado no ReactOS)")
endif()
message(STATUS "")
message(STATUS "[!] Próximas ações:")
message(STATUS "    1. Executar: ninja bootcd")
message(STATUS "    2. Esperar: 45-60 minutos (recompilação do ISO)")
message(STATUS "    3. Testar: bootcd.iso (~250-300 MB)")
message(STATUS "    4. No Windows ReactOS: dotnet-download-native.bat")
message(STATUS "")
message(STATUS "[i] Mais informações: dotnet-download-native.bat --help")
message(STATUS "")

# ===================================================================
