# Guia de Implementação UEFI/GPT no ReactOS

## Status Atual (2026-03-05)

### ✅ O que já existe:
1. **Bootloader UEFI compilado**: `uefildr.efi` em `output-VS-amd64/boot/freeldr/freeldr/`
2. **Imagem EFI criada**: `efisys.bin` com estrutura EFI/BOOT/bootx64.efi
3. **ISO com suporte UEFI**: bootcd.iso já tem `-eltorito-platform efi`
4. **Código GPT no isohybrid**: Completamente implementado, mas desabilitado

### ❌ O que falta:
1. **Habilitar suporte EFI no isohybrid**: Define `REACTOS_ISOHYBRID_EFI_MAC_SUPPORT`
2. **Adicionar parâmetros UEFI ao build**: `--uefi` no isohybrid
3. **Implementar suporte GPT completo**: Tabela de partições GPT + MBR híbrida

---

## Implementação

### Etapa 1: Habilitar suporte EFI no isohybrid

**Arquivo**: `sdk/tools/isohybrid/CMakeLists.txt`

**Mudança**:
```cmake
add_definitions(
    -DISOHYBRID_C_STANDALONE
    -DREACTOS_ISOHYBRID_EFI_MAC_SUPPORT  # ← ADICIONAR ESTA LINHA
)
```

**Nota**: O código EFI usa `uuid/uuid.h`, mas há implementações alternativas no Windows (rpcrt4.dll).

---

### Etapa 2: Modificar boot_images.cmake

**Arquivo**: `boot/boot_images.cmake`

**Mudança na linha ~120**:
```cmake
# ANTES:
COMMAND native-isohybrid -b ${_isombr_file} -t 0x96 ${REACTOS_BINARY_DIR}/bootcd.iso

# DEPOIS:
COMMAND native-isohybrid -b ${_isombr_file} -t 0x96 --uefi ${REACTOS_BINARY_DIR}/bootcd.iso
```

**O que isso faz**:
- `--uefi`: Cria tabela GPT + partição ESP (EFI System Partition)
- `-b ${_isombr_file}`: Define MBR customizado
- Resultado: **ISO híbrida BIOS (MBR) + UEFI (GPT)**

---

### Etapa 3: Verificar implementação GPT no FreeLoader

**Arquivo**: `boot/freeldr/freeldr/disk/partition.c`

**Status atual**:
```c
case PARTITION_STYLE_GPT:
    BOOLEAN UefiGetGptPartitionEntry(...);  // ✅ Declarado
    return UefiGetGptPartitionEntry(...);   // ✅ Implementado em arch/uefi/
    // Mas tem FIXME em alguns lugares
```

**Ações**:
- ✅ Detecção de GPT: Implementada (linha 321-349)
- ✅ Leitura de partições GPT: Implementada em `arch/uefi/uefidisk.c`
- ⚠️ Alguns FIXMEs podem ser ignorados (só afetam boot BIOS de disco GPT)

---

## Estrutura de Boot Resultante

### MBR (BIOS Legacy)
```
Setor 0: MBR Híbrido
- Partition 1: Tipo 0x00 (Protective GPT)
- Partition 2: Tipo 0xEF (EFI System Partition)
- Código de boot: isombr.bin
```

### GPT (UEFI)
```
Setor 1: GPT Header
Setores 2-33: Partition Table Entries
- Partition 1: ESP (EFI System Partition)
  - Tipo: C12A7328-F81F-11D2-BA4B-00A0C93EC93B
  - Contém: EFI/BOOT/BOOTX64.EFI
  - Tamanho: ~2.88 MB (efisys.bin)
- Partition 2: Dados ISO9660
  - Tipo: EBD0A0A2-B9E5-4433-87C0-68B6B72699C7
```

---

## Passos de Compilação

### 1. Habilitar EFI Support
```bash
# Editar sdk/tools/isohybrid/CMakeLists.txt
# Adicionar: -DREACTOS_ISOHYBRID_EFI_MAC_SUPPORT

# Reconfigurar
cd e:\ReactOS\reactos
configure.cmd VSSolution amd64 output-VS-amd64
```

### 2. Modificar boot_images.cmake
```bash
# Editar boot/boot_images.cmake (linha ~120)
# Adicionar: --uefi ao comando native-isohybrid
```

### 3. Recompilar ISO
```bash
cd e:\ReactOS\reactos\output-VS-amd64
ninja isohybrid  # Recompila ferramenta
ninja bootcd     # Recompila ISO com GPT
```

### 4. Verificar resultado
```bash
# Verificar tamanho ISO (deve ser ~80-100 MB)
Get-Item bootcd.iso | Select-Object Length, LastWriteTime

# Verificar partições GPT (usar gdisk ou similar)
gdisk -l bootcd.iso
```

---

## Verificação de Boot

### BIOS Legacy (MBR):
1. Boot via isombr.bin → isoboot.bin → freeldr.sys
2. Lê partição ISO9660 (El Torito)
3. Carrega kernel ReactOS

### UEFI (GPT):
1. Firmware UEFI lê GPT
2. Monta ESP (EFI/BOOT/)
3. Executa BOOTX64.EFI (uefildr.efi)
4. FreeLoader carrega kernel ReactOS

---

## Problemas Conhecidos e Soluções

### 1. uuid/uuid.h não disponível no MSVC
**Solução**: Dois caminhos:
- **A)** Usar implementação RPC nativa do Windows (CoCreateGuid)
- **B)** Criar UUIDs estáticos (adequado para ISO)

**Implementação B (mais simples)**:
```c
// Substituir uuid_generate() por UUIDs fixos
static const uuid_t disk_uuid = {0x12,0x34,...};  // Gerar uma vez
```

### 2. isohybrid --uefi requer efisys.bin
**Status**: ✅ Já configurado corretamente

### 3. Tamanho da ISO aumenta
**Causa**: GPT Header + Protective MBR
**Impacto**: +34 KB (desprezível)

---

## Alternativa: Script Pós-Processamento

Se compilar isohybrid com EFI for problemático, criar script PowerShell:

```powershell
# gpt-iso-hybrid.ps1
# Adiciona GPT à bootcd.iso manualmente usando gdisk

$isoPath = "e:\ReactOS\reactos\output-VS-amd64\bootcd.iso"

# 1. Criar backup
Copy-Item $isoPath "$isoPath.bak"

# 2. Usar gdisk (necessário instalação)
# ou criar GPT manualmente com Write-FileStream

# 3. Adicionar Protective MBR + GPT Header
# ...implementação...
```

---

## Próximos Passos

1. ✅ **Habilitar EFI support** (modificar CMakeLists.txt)
2. ✅ **Adicionar --uefi** (modificar boot_images.cmake)
3. ⏳ **Recompilar** (ninja isohybrid && ninja bootcd)
4. ⏳ **Testar em VM UEFI** (QEMU com OVMF, VirtualBox UEFI, Hyper-V Gen2)
5. ⏳ **Validar GPT** (gdisk -l bootcd.iso)

---

## Referências

- [UEFI Specification 2.10](https://uefi.org/specifications)
- [GPT Partition Layout](https://en.wikipedia.org/wiki/GUID_Partition_Table)
- [Syslinux isohybrid](https://wiki.syslinux.org/wiki/index.php?title=Isohybrid)
- [ReactOS UEFI Implementation](https://github.com/reactos/reactos/tree/master/boot/freeldr/freeldr/arch/uefi)

---

**Última atualização**: 2026-03-05  
**Status**: Pronto para implementação
