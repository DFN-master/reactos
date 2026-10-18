# Experimento NT 6.1 — NT61-0001: SystemPowerInformation (NtQuerySystemInformation)

**Data:** 2026-10-18
**Autor:** Agente coordenador (OpenCode)
**Estado:** `INVESTIGATING` (ReactOS side confirmado; Windows 7 side pendente de verificação REA)
**Classificação de risco:** C3 (kernel Executive — `ntoskrnl/ex/sysinfo.c`)

---

## 1. Funcionalidade

- **Nome:** SystemPowerInformation (SystemInformationClass 42 — `SystemProcessorIdleInformation` em ReactOS, mapeado como "Power Information")
- **Componente:** `ntoskrnl` — Executive, `ex/sysinfo.c`
- **API:** `NtQuerySystemInformation(SystemPowerInformation, ...)`
- **Relevância para aplicações reais:** Média. Usado por aplicações de monitoramento de energia e pelo sistema (power manager). Presença em Windows 7 confirmada (estrutura `SYSTEM_POWER_INFORMATION` documentada).

---

## 2. Evidência do lado ReactOS

**Arquivo:** `ntoskrnl/ex/sysinfo.c`, linhas 2076–2088

```c
/* Class 42 - Power Information */
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

**Classificação:** `ABSENT` — a implementação retorna `STATUS_NOT_IMPLEMENTED` incondicionalmente.
**Dependências:** `KeNumberProcessors`, `PROCESSOR_POWER_INFORMATION` (struct), `QSI_DEF` macro.
**Risco:** C3 (kernel Executive). Requer build + VM boot + debug log.

---

## 3. Evidência do lado Windows 7

**Artefato:** `C:\Users\Douglas\AppData\Local\Temp\opencode\win7-ntoskrnl-x86.exe`
- SHA-256: `d74e389d1a3ed4c79ffc7c0af1c092102f13e67168ab66a9296cdde62a2c6cf8`
- Arquitetura: x86 (32-bit)
- Origem: ISO `D:\Win7ProSP1_ptBR_x86.iso`, índice 4 (Professional), extraído via DISM

**Análise REA (Ghidra 12.1.3):**
- `NtQuerySystemInformation` localizado em `0x005f9775` (exportado por nome).
- `NtSetSystemInformation` **não encontrado** como export (pode ser interno ou com nome diferente).
- Strings relevantes encontradas: `ZwPowerInformation`, `NtSetSystemPowerState`, `SystemPowerPolicy`.
- **Pendente:** análise detalhada do case `SystemPowerInformation` dentro de `NtQuerySystemInformation`. A função é grande (146KB de decompilação) e usa jump table; o case específico precisa ser isolado.

**Limitações da análise REA:**
- Sem PDB (símbolos de debug não disponíveis para ntoskrnl.exe).
- Função NtQuerySystemInformation é muito grande para análise completa em uma única passagem.
- O case SystemPowerInformation (class 42) precisa ser localizado por análise de fluxo (switch/jump table).

---

## 4. Estrutura esperada (documentada)

```c
typedef struct _SYSTEM_POWER_INFORMATION {
    ULONG MaxIdlenessAllowed;
    ULONG Idleness;
    ULONG TimeRemaining;
    UCHAR CoolingMode;
} SYSTEM_POWER_INFORMATION, *PSYSTEM_POWER_INFORMATION;
```

**Comportamento esperado no Windows 7:**
- Retorna `STATUS_SUCCESS` com a estrutura preenchida.
- `MaxIdlenessAllowed`: tempo máximo de inatividade permitido antes de entrar em sleep.
- `Idleness`: porcentagem atual de inatividade (0-100).
- `TimeRemaining`: tempo estimado até sleep automático (em segundos).
- `CoolingMode`: 0 = ativo, 1 = passivo.

**Tamanho esperado:** `sizeof(SYSTEM_POWER_INFORMATION)` = 13 bytes (4+4+4+1).

---

## 5. Acceptance criteria (a verificar)

- AC1: `NtQuerySystemInformation(SystemPowerInformation, buf, sizeof(SYSTEM_POWER_INFORMATION), &retLen)` retorna `STATUS_SUCCESS` no ReactOS (atualmente retorna `STATUS_NOT_IMPLEMENTED`).
- AC2: `*retLen` == `sizeof(SYSTEM_POWER_INFORMATION)`.
- AC3: `buf->CoolingMode` é 0 ou 1 (valor válido).
- AC4: `buf->Idleness` está entre 0 e 100.
- AC5: Com `Size < sizeof(SYSTEM_POWER_INFORMATION)`, retorna `STATUS_INFO_LENGTH_MISMATCH`.
- AC6: Com `InfoClass` inválido (0x80000000), retorna `STATUS_INVALID_INFO_CLASS`.
- AC7: Comportamento no Windows 7 de referência documentado com evidência REA.

---

## 6. Teste proposto

**Arquivo:** `modules/rostests/apitests/ntdll/NtQuerySystemInformation.c` (extensão)

```c
// Test SystemPowerInformation (class 42)
{
    SYSTEM_POWER_INFORMATION spi = {0};
    ULONG retLen = 0;
    NTSTATUS Status = NtQuerySystemInformation(SystemPowerInformation, &spi, sizeof(spi), &retLen);
    ok_ntstatus(Status, STATUS_SUCCESS);
    ok_eq_ulong(retLen, sizeof(spi));
    ok(spi.CoolingMode == 0 || spi.CoolingMode == 1, "CoolingMode=%u\n", spi.CoolingMode);
    ok(spi.Idleness <= 100, "Idleness=%lu\n", spi.Idleness);
}
```

**Nota:** O teste existente (`NtQuerySystemInformation.c`) é mínimo (apenas 2 casos). Este experimento exigiria estender o teste.

---

## 7. Implementação proposta (ReactOS)

**Arquivo a modificar:** `ntoskrnl/ex/sysinfo.c`

**Abordagem mínima:**
1. Substituir o `return STATUS_NOT_IMPLEMENTED` por uma implementação que:
   - Preenche `SYSTEM_POWER_INFORMATION` com valores padrão (CoolingMode=0, Idleness=0, TimeRemaining=0, MaxIdlenessAllowed=0).
   - Retorna `STATUS_SUCCESS`.
2. Não tentar implementar a lógica real de power management nesta fase (requer integração com o power manager).
3. Marcar como `IMPLEMENTED` (não `TESTED`) até que a verificação no Windows 7 seja feita.

**Classificação:** C3 (kernel Executive). Requer build + VM boot + debug log.

---

## 8. Riscos

- Implementação mínima pode não refletir o comportamento real do Windows 7.
- `SystemPowerInformation` pode ter comportamento dependente de hardware/energia.
- Sem verificação REA do case específico, a implementação é baseada em documentação, não em evidência.
- Mudança no kernel requer cuidado extra (C3).

---

## 9. Próximos passos

1. **Verificar no Windows 7** — analisar o case SystemPowerInformation em ntoskrnl via REA (localizar por jump table).
2. **Escrever o teste** — estender `NtQuerySystemInformation.c` com o caso SystemPowerInformation.
3. **Implementar** — substituir STATUS_NOT_IMPLEMENTED por implementação mínima.
4. **Build + VM boot** — validar que o sistema boota.
5. **Executar teste** — rodar apitest na VM ReactOS.
6. **Atualizar inventário** — NT61-0001: `SPECIFIED` → `IMPLEMENTED` → `TESTED_X86`.

---

## 10. Status atual

- ReactOS side: **Confirmado** (STATUS_NOT_IMPLEMENTED em sysinfo.c, linhas 2076-2088)
- Windows 7 side: **Pendente** (ntoskrnl.exe extraído e aberto no REA; NtQuerySystemInformation em 0x005f9775; case SystemPowerInformation não isolado)
- Teste: **Não escrito** (proposta acima)
- Implementação: **Não iniciada**
- Estado no inventário: `INVESTIGATING` (será `SPECIFIED` quando o teste for escrito)