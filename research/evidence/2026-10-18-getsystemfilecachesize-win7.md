# Evidence: GetSystemFileCacheSize — Windows 7 SP1 (x86)

## Artifact

- File: `C:\Users\Douglas\AppData\Local\Temp\opencode\win7-kernel32-x86.dll`
- Architecture: x86 (32-bit)
- SHA-256: `ff4c354d08c8a6a6554de195c72e5c940be0def803ce0be8312161943a0f5821`
- Source: ISO `D:\Win7ProSP1_ptBR_x86.iso`, índice 4 (Professional), extraído via DISM (somente leitura)
- Claimed product/version: Windows 7 Professional SP1 (x86)

## Tooling

- REA: rea-agents 6.5.0
- Backend: Ghidra 12.1.3
- Commands: `search_strings("GetSystemFileCacheSize")`, `xrefs(0x77e9b776)`, `analyze_function(0x77e77c34)`

## Observations

### [FATO] Função encontrada

- Endereço: `0x77e77c34` (exportada por nome: `GetSystemFileCacheSize`)
- Tamanho: ~80 bytes de código

### [FATO] Decompilação (pseudocódigo Ghidra)

```c
BOOL GetSystemFileCacheSize(PSIZE_T lpMinimumFileCacheSize,
                            PSIZE_T lpMaximumFileCacheSize,
                            PDWORD lpFlags)
{
    int iVar1;
    BOOL BVar2;
    uint uVar3;
    undefined1 local_28[12];  // buffer de 36 bytes (0x24)
    int local_1c;
    int local_18;
    uint local_8;

    iVar1 = NtQuerySystemInformation(0x51, local_28, 0x24, 0);
    if (iVar1 < 0) {
        BaseSetLastNTError(iVar1);
        BVar2 = 0;
    }
    else {
        *lpMinimumFileCacheSize = *(int *)(DAT_77ea6094 + 300) * local_1c;
        *lpMaximumFileCacheSize = *(int *)(DAT_77ea6094 + 300) * local_18;
        uVar3 = 0;
        *lpFlags = local_8;
        if ((local_8 & 4) != 0) {
            uVar3 = 4;
        }
        if ((local_8 & 1) != 0) {
            uVar3 = uVar3 | 1;
        }
        *lpFlags = uVar3;
        BVar2 = 1;
    }
    return BVar2;
}
```

### [FATO] Parâmetros de NtQuerySystemInformation

- InfoClass: `0x51` = 81 = `SystemFileCacheInformationEx` (no enum do ReactOS)
- Buffer: 36 bytes (`0x24` = `sizeof(SYSTEM_FILECACHE_INFORMATION)`)
- ReturnLength: 0 (ignorado)

### [FATO] Mapeamento de campos

| Windows 7 | ReactOS struct field | Offset |
|---|---|---|
| `local_1c` (MinimumWorkingSet) | `MinimumWorkingSet` | 0x0C |
| `local_18` (MaximumWorkingSet) | `MaximumWorkingSet` | 0x10 |
| `local_8` (Flags) | `Flags` | 0x20 |

### [FATO] Multiplicação por page size

- `DAT_77ea6094 + 0x12c` (300): valor global, provavelmente o tamanho de página do sistema (4096 em x86).
- `*lpMinimumFileCacheSize = page_size * MinimumWorkingSet`
- `*lpMaximumFileCacheSize = page_size * MaximumWorkingSet`

### [FATO] Máscara de flags

- Apenas bits 0 e 2 são propagados: `flags = (flags & 4 ? 4 : 0) | (flags & 1 ? 1 : 0)`

### [INFERIDO] Estrutura SYSTEM_FILECACHE_INFORMATION

A estrutura tem 36 bytes (9 campos × 4 bytes em 32-bit):
```
CurrentSize (4) + PeakSize (4) + PageFaultCount (4) + MinimumWorkingSet (4) +
MaximumWorkingSet (4) + CurrentSizeIncludingTransitionInPages (4) +
PeakSizeIncludingTransitionInPages (4) + TransitionRePurposeCount (4) + Flags (4) = 36
```

### [HIPOTESE] DAT_77ea6094 + 0x12c

Provavelmente o tamanho de página do sistema (`SharedUserData->PageSize` ou equivalente). Em x86 Windows 7, o page size é 4096 (0x1000).

## ReactOS comparison

- **Counterpart:** `dll/win32/kernel32/client/sysinfo.c` — `GetSystemFileCacheSize`
- **Antes:** Stub (STUB macro → SetLastError(ERROR_CALL_NOT_IMPLEMENTED), return FALSE)
- **Depois (implementado):** Chama `NtQuerySystemInformation(SystemFileCacheInformationEx, ...)`, mapeia campos, multiplica por PAGE_SIZE, mascara flags
- **Correspondência:** Implementação alinhada com Windows 7 (class 81, page size, flag mask)
- **Pendente:** Teste de execução na VM (não executado ainda)

## Limits of this analysis

- Sem PDB (símbolos de debug não disponíveis).
- `DAT_77ea6094 + 0x12c` não foi verificado como page size — apenas inferido.
- Apenas a versão x86 foi analisada.
- O comportamento em casos de erro (buffer NULL, etc.) não foi verificado.

## Conclusion

- Confidence: **high** (decompilação clara, estrutura bem definida)
- Implementação ReactOS alinhada com Windows 7.
- Próximo passo: executar teste na VM ReactOS para validar.