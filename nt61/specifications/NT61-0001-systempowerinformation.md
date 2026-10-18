# Experimento NT 6.1 — NT61-0001: SystemProcessorIdleInformation (NtQuerySystemInformation, class 42)

**Data:** 2026-10-18
**Autor:** Agente coordenador (OpenCode)
**Estado:** `INVESTIGATING`
**Classificação de risco:** C3 (kernel Executive — `ntoskrnl/ex/sysinfo.c`)

---

## 1. Funcionalidade

- **Nome:** SystemProcessorIdleInformation (SystemInformationClass 42 / 0x2A)
- **Componente:** `ntoskrnl` — Executive, `ex/sysinfo.c`
- **API:** `NtQuerySystemInformation(SystemProcessorIdleInformation, ...)`
- **Relevância para aplicações reais:** Média. Usado por ferramentas de monitoramento de CPU e pelo sistema (power manager, task scheduler).
- **Nota:** O código ReactOS original rotula incorretamente este caso como "SystemPowerInformation" (DPRINT1). Na verdade, a classe 42 é `SystemProcessorIdleInformation` em ambas as plataformas. `SystemPowerInformation` não é uma classe válida de `NtQuerySystemInformation`.

---

## 2. Evidência do lado ReactOS

**Arquivo:** `ntoskrnl/ex/sysinfo.c`, linhas 2076–2088

```c
/* Class 42 - Power Information (mislabel) */
QSI_DEF(SystemProcessorIdleInformation)
{
    *ReqSize = sizeof(PROCESSOR_POWER_INFORMATION) * KeNumberProcessors;

    if (sizeof(PROCESSOR_POWER_INFORMATION) * KeNumberProcessors > Size)
    {
        return STATUS_INFO_LENGTH_MISMATCH;
    }

    /* FIXME */
    DPRINT1("NtQuerySystemInformation - SystemPowerInformation not implemented\n");
    return STATUS_NOT_IMPLEMENTED;
}
```

**Classificação:** `ABSENT` — retorna `STATUS_NOT_IMPLEMENTED` incondicionalmente.
**Dependências:** `KeNumberProcessors`, `PROCESSOR_POWER_INFORMATION` (struct), `QSI_DEF` macro.
**Risco:** C3 (kernel Executive). Requer build + VM boot + debug log.

---

## 3. Evidência do lado Windows 7

**Artefato:** `C:\Users\Douglas\AppData\Local\Temp\opencode\win7-ntoskrnl-x86.exe`
- SHA-256: `d74e389d1a3ed4c79ffc7c0af1c092102f13e67168ab66a9296cdde62a2c6cf8`
- Arquitetura: x86 (32-bit)
- Origem: ISO `D:\Win7ProSP1_ptBR_x86.iso`, índice 4 (Professional)

**Análise REA (Ghidra 12.1.3):**
- `NtQuerySystemInformation` localizado em `0x005f9775` (exportado por nome).
- Decompilação mostra que classe 0x2A (42) é tratada: `if (param_1 == 0x2a) goto LAB_005f97c4`
- LAB_005f97c4: `param_1 = (uint)*(ushort *)((int)ClientId.UniqueProcess + 0x3c6)` — lê valor do processo
- Em seguida chama `FUN_005f97f4` (implementação real, função grande com jump table)
- **Pendente:** isolar o case 0x2A dentro de `FUN_005f97f4` para verificar o comportamento exato.

**Limitações:**
- Sem PDB (símbolos não disponíveis).
- `FUN_005f97f4` é muito grande para análise completa em uma passagem.
- O case específico precisa ser localizado por análise de fluxo (switch/jump table).

---

## 4. Estrutura esperada (documentada)

```c
typedef struct _PROCESSOR_POWER_INFORMATION {
    ULONG Number;
    ULONG MaxMhz;
    ULONG CurrentMhz;
    ULONG MhzLimit;
    ULONG MaxIdleState;
    ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;
```

**Comportamento esperado no Windows 7:**
- Retorna `STATUS_SUCCESS` com array de `PROCESSOR_POWER_INFORMATION` (um por processador).
- `Number`: índice do processador (0-based).
- `MaxMhz`: frequência máxima do processador.
- `CurrentMhz`: frequência atual.
- `MhzLimit`: limite de frequência (throttling).
- `MaxIdleState`: número máximo de estados de idle (C-states).
- `CurrentIdleState`: estado de idle atual.

**Tamanho esperado:** `sizeof(PROCESSOR_POWER_INFORMATION) * KeNumberProcessors`.

---

## 5. Acceptance criteria

- AC1: `NtQuerySystemInformation(SystemProcessorIdleInformation, buf, sizeof(PROCESSOR_POWER_INFORMATION)*N, &retLen)` retorna `STATUS_SUCCESS` no ReactOS (atualmente retorna `STATUS_NOT_IMPLEMENTED`).
- AC2: `*retLen` == `sizeof(PROCESSOR_POWER_INFORMATION) * KeNumberProcessors`.
- AC3: `buf[0].Number` == 0.
- AC4: `buf[0].MaxMhz` > 0.
- AC5: Com `Size < sizeof(PROCESSOR_POWER_INFORMATION) * KeNumberProcessors`, retorna `STATUS_INFO_LENGTH_MISMATCH`.
- AC6: Com `InfoClass` inválido (0x80000000), retorna `STATUS_INVALID_INFO_CLASS`.
- AC7: Comportamento no Windows 7 de referência documentado com evidência REA.

---

## 6. Teste proposto

**Arquivo:** `modules/rostests/apitests/ntdll/NtQuerySystemInformation.c` (extensão)

```c
// Test SystemProcessorIdleInformation (class 42)
{
    ULONG Size = sizeof(PROCESSOR_POWER_INFORMATION) * 1; // at least 1 processor
    PROCESSOR_POWER_INFORMATION *ppi = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Size);
    ULONG retLen = 0;
    NTSTATUS Status = NtQuerySystemInformation(SystemProcessorIdleInformation, ppi, Size, &retLen);
    ok_ntstatus(Status, STATUS_SUCCESS);
    ok_eq_ulong(retLen, sizeof(PROCESSOR_POWER_INFORMATION));
    ok(ppi->MaxMhz > 0, "MaxMhz=%lu\n", ppi->MaxMhz);
    HeapFree(GetProcessHeap(), 0, ppi);
}
```

---

## 7. Implementação proposta (ReactOS)

**Arquivo a modificar:** `ntoskrnl/ex/sysinfo.c`, linhas 2076–2088

**Abordagem mínima (honest):**
1. Implementar usando `KeQueryProcessorPowerInformation` ou similar (se disponível no ReactOS).
2. Se não disponível, retornar `STATUS_NOT_IMPLEMENTED` com DPRINT (estado atual) — **não** retornar STATUS_SUCCESS com dados falsos.
3. Se implementável, preencher `PROCESSOR_POWER_INFORMATION` com dados reais do HAL/power manager.

**Classificação:** C3 (kernel Executive). Requer build + VM boot + debug log.

---

## 8. Riscos

- Implementação requer integração com power management (HAL, ACPI).
- ReactOS pode não ter as APIs internas necessárias (`KeQueryProcessorPowerInformation`).
- Sem verificação REA do case específico em `FUN_005f97f4`, a implementação é baseada em documentação.
- Se as APIs internas não existirem, o experimento pode precisar ser reescopado.

---

## 9. Próximos passos

1. **Verificar APIs internas** — buscar `KeQueryProcessorPowerInformation` ou equivalente no ReactOS.
2. **Verificar no Windows 7** — isolar o case 0x2A em `FUN_005f97f4` via REA (jump table).
3. **Escrever o teste** — estender `NtQuerySystemInformation.c`.
4. **Implementar** — substituir STATUS_NOT_IMPLEMENTED (se viável).
5. **Build + VM boot + execução do teste** — validar.
6. **Atualizar inventário** — NT61-0001: `INVESTIGATING` → `SPECIFIED` → `IMPLEMENTED` → `TESTED_X86`.

---

## 10. Status atual

- ReactOS side: **Confirmado** (STATUS_NOT_IMPLEMENTED em sysinfo.c, class 42 = SystemProcessorIdleInformation)
- Windows 7 side: **Parcial** (NtQuerySystemInformation em 0x005f9775; case 0x2A identificado no wrapper; implementação em FUN_005f97f4 pendente de análise)
- Teste: **Não escrito** (proposta acima)
- Implementação: **Não iniciada**
- Estado no inventário: `INVESTIGATING`