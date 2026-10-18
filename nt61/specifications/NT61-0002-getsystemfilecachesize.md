# Experimento NT 6.1 — NT61-0002: GetSystemFileCacheSize (kernel32)

**Data:** 2026-10-18
**Autor:** Agente coordenador (OpenCode)
**Estado:** `IMPLEMENTED` (build OK i386 + amd64; teste pendente)
**Classificação de risco:** C2 (user-mode DLL — `dll/win32/kernel32/client/sysinfo.c`)

---

## 1. Funcionalidade

- **Nome:** GetSystemFileCacheSize
- **Componente:** `kernel32` — `client/sysinfo.c`
- **API:** `GetSystemFileCacheSize(PSIZE_T lpMinimumFileCacheSize, PSIZE_T lpMaximumFileCacheSize, PDWORD lpFlags)`
- **Relevância para aplicações reais:** Média. Usado por aplicações que monitoram ou ajustam o cache de arquivos do sistema.
- **Documentação Windows:** Retorna o tamanho mínimo, máximo e flags do cache de arquivos do sistema.

---

## 2. Evidência do lado ReactOS (antes)

**Arquivo:** `dll/win32/kernel32/client/sysinfo.c`, linhas 575-583

```c
BOOL
WINAPI
GetSystemFileCacheSize(OUT PSIZE_T lpMinimumFileCacheSize,
                       OUT PSIZE_T lpMaximumFileCacheSize,
                       OUT PDWORD lpFlags)
{
    STUB;  // SetLastError(ERROR_CALL_NOT_IMPLEMENTED); DPRINT1(...)
    return FALSE;
}
```

**Classificação:** `ABSENT` — stub que retorna FALSE com ERROR_CALL_NOT_IMPLEMENTED.

---

## 3. Implementação realizada

**Arquivo modificado:** `dll/win32/kernel32/client/sysinfo.c`

```c
BOOL
WINAPI
GetSystemFileCacheSize(OUT PSIZE_T lpMinimumFileCacheSize,
                       OUT PSIZE_T lpMaximumFileCacheSize,
                       OUT PDWORD lpFlags)
{
    SYSTEM_FILECACHE_INFORMATION CacheInfo;
    NTSTATUS Status;
    ULONG ReturnLength;

    Status = NtQuerySystemInformation(SystemFileCacheInformation,
                                      &CacheInfo,
                                      sizeof(CacheInfo),
                                      &ReturnLength);

    if (!NT_SUCCESS(Status))
    {
        BaseSetLastNTError(Status);
        return FALSE;
    }

    if (lpMinimumFileCacheSize)
        *lpMinimumFileCacheSize = CacheInfo.MinimumWorkingSet;

    if (lpMaximumFileCacheSize)
        *lpMaximumFileCacheSize = CacheInfo.MaximumWorkingSet;

    if (lpFlags)
        *lpFlags = CacheInfo.Flags;

    return TRUE;
}
```

**Mudanças:**
- Substituído `STUB` (que retornava FALSE) por implementação real.
- Chama `NtQuerySystemInformation(SystemFileCacheInformation, ...)` (já implementado no kernel ReactOS).
- Mapeia `MinimumWorkingSet` → `lpMinimumFileCacheSize`, `MaximumWorkingSet` → `lpMaximumFileCacheSize`, `Flags` → `lpFlags`.
- Retorna TRUE em sucesso, FALSE com last error em falha.
- `@unimplemented` → `@implemented`.

**Classificação de risco:** C2 (user-mode DLL). Sem alteração no kernel.

---

## 4. Verificação

- **Build i386:** ✅ `kernel32.dll` compilado e linkado com sucesso (exit 0).
- **Build amd64:** ✅ `kernel32.dll` compilado e linkado com sucesso (exit 0).
- **Teste:** Não executado (requer VM ReactOS + execução do apitest).
- **Boot VM:** Não testado (requer boot da nova build na VM).

---

## 5. Evidência do lado Windows 7

**Pendente.** Necessário:
- Verificar se `GetSystemFileCacheSize` existe no Windows 7 (kernel32 ou kernelbase).
- Confirmar o mapeamento de campos (MinimumWorkingSet → min, MaximumWorkingSet → max).
- Confirmar que retorna TRUE com valores válidos.

**Nota:** A estrutura `SYSTEM_FILECACHE_INFORMATION` e o `NtQuerySystemInformation(SystemFileCacheInformation, ...)` já são implementados no ReactOS. A implementação segue o padrão documentado do Windows.

---

## 6. Teste proposto

**Arquivo:** `modules/rostests/apitests/kernel32/GetSystemFileCacheSize.c` (novo)

```c
START_TEST(GetSystemFileCacheSize)
{
    SIZE_T MinSize = 0, MaxSize = 0;
    DWORD Flags = 0;
    BOOL Result;

    Result = GetSystemFileCacheSize(&MinSize, &MaxSize, &Flags);
    ok(Result == TRUE, "GetSystemFileCacheSize returned %d\n", Result);
    ok(MinSize > 0, "MinimumFileCacheSize=%lu\n", (ULONG)MinSize);
    ok(MaxSize >= MinSize, "MaxSize=%lu < MinSize=%lu\n", (ULONG)MaxSize, (ULONG)MinSize);
}
```

---

## 7. Status atual

- ReactOS side: **Implementado** (kernel32/client/sysinfo.c)
- Windows 7 side: **Pendente** (verificação REA)
- Teste: **Não escrito** (proposta acima)
- Build: **OK** (i386 + amd64)
- Estado no inventário: `IMPLEMENTED` (será `TESTED_X86` após execução do teste)