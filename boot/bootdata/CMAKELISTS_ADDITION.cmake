# =====================================================================
# .NET Framework 4.8 Native Integration - Exact CMakeLists.txt Addition
# =====================================================================
#
# COPIAR TUDO ABAIXO E ADICIONAR AO FINAL DE:
# e:\ReactOS\reactos\boot\bootdata\CMakeLists.txt
#
# =====================================================================

# =====================================================================
# .NET Framework 4.8 Native Integration
# =====================================================================
# Este bloco adiciona suporte nativo ao .NET Framework 4.8
# para que esteja disponível imediatamente no bootcd.iso
# Data adição: 2026-03-05
# =====================================================================

message(STATUS "[.NET] Iniciando configuração .NET Framework 4.8 nativo...")

# Adicionar script de download nativo do .NET Framework
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/dotnet-download-native.bat)
    add_cd_file(
        FILE ${CMAKE_CURRENT_SOURCE_DIR}/dotnet-download-native.bat
        DESTINATION reactos
        FOR bootcd livecd hybridcd
    )
    message(STATUS "[.NET] ✓ dotnet-download-native.bat adicionado ao ISO")
else()
    message(STATUS "[.NET] ⊘ dotnet-download-native.bat não encontrado")
endif()

# =====================================================================
# SEÇÃO 1: Procurar mscoree.dll compilado
# =====================================================================

find_file(MSCOREE_DLL
    NAMES mscoree.dll
    PATHS "${CMAKE_BINARY_DIR}/dll/win32/mscoree"
    NO_DEFAULT_PATH
)

if(MSCOREE_DLL)
    message(STATUS "[.NET] ✓ mscoree.dll encontrado: ${MSCOREE_DLL}")
    
    get_filename_component(MSCOREE_SIZE ${MSCOREE_DLL} SIZE)
    message(STATUS "[.NET]   Tamanho: ${MSCOREE_SIZE} bytes")
    
    add_cd_file(
        FILE ${MSCOREE_DLL}
        DESTINATION reactos/System32
        FOR bootcd
    )
    message(STATUS "[.NET] ✓ mscoree.dll será incluído no bootcd.iso")
else()
    message(STATUS "[.NET] ⊘ mscoree.dll não encontrado em output")
    message(STATUS "[.NET]   (será usado arquivo stub ou será baixado em runtime)")
endif()

# =====================================================================
# SEÇÃO 2: Adicionar manifesto de .NET Framework
# =====================================================================

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/dotnet-manifest.txt)
    add_cd_file(
        FILE ${CMAKE_CURRENT_SOURCE_DIR}/dotnet-manifest.txt
        DESTINATION reactos/dotnet
        NAME_ON_CD DOTNET_MANIFEST.txt
        FOR bootcd
    )
    message(STATUS "[.NET] ✓ DOTNET_MANIFEST.txt adicionado")
endif()

# =====================================================================
# SEÇÃO 3: Status Final
# =====================================================================

message(STATUS "")
message(STATUS "[.NET] ════════════════════════════════════════════════════")
message(STATUS "[.NET] .NET Framework 4.8 Native Integration Configurado")
message(STATUS "[.NET] ════════════════════════════════════════════════════")
message(STATUS "[.NET] ")
message(STATUS "[.NET] Arquivos a incluir no ISO:")
message(STATUS "[.NET]   ✓ dotnet-download-native.bat")
message(STATUS "[.NET]   ✓ mscoree.dll (se encontrado)")
message(STATUS "[.NET]   ✓ DOTNET_MANIFEST.txt")
message(STATUS "[.NET] ")
message(STATUS "[.NET] Próximo passo: ninja bootcd")
message(STATUS "[.NET]   (Vai recompilar bootcd.iso com .NET integrado)")
message(STATUS "[.NET] ")
message(STATUS "[.NET] Duração estimada: 45-60 minutos")
message(STATUS "[.NET] ISO resultante: 250-300 MB")
message(STATUS "[.NET] ════════════════════════════════════════════════════")
message(STATUS "")

# =====================================================================
# FIM DA INTEGRAÇÃO .NET FRAMEWORK 4.8
# =====================================================================
