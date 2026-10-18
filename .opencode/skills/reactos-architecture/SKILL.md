---
name: reactos-architecture
description: Map ReactOS kernel and Executive components, locate the files implementing a given Windows NT behaviour, and document execution paths before proposing any change. Use when you need to know WHERE a feature lives in ReactOS, what it depends on, or how NT 5.2-era ReactOS diverges from NT 6.1 (Windows 7).
license: GPL-2.0
compatibility: opencode
metadata:
  audience: reactos-nt61-project
  workflow: architecture-research
  opencode/autoinvoke: "true"
---

# ReactOS Architecture

## Purpose

Locate and document how ReactOS implements a given NT behaviour, so that a change can be scoped safely.

**This skill is read-and-investigate only. Do not edit code while using it.**

## Critical correction to a common wrong assumption

ReactOS is **not** a copy of Windows XP, and **not** a copy of Windows Server 2003.
Its own `README.md` states: *"The ReactOS project, although currently focused on Windows Server 2003 compatibility, is always keeping an eye toward compatibility with Windows Vista and future Windows NT releases."*

Therefore:
- Do **not** describe ReactOS as "NT 5.2 code" as a blanket statement.
- Do **not** assume a Win7 behaviour is absent just because it is absent from XP.
- Do **not** assume a function exists because its name matches the Windows SDK.

The correct posture: read the actual ReactOS source, at the actual revision, and report what it does.

## Repository map

| Path | Contains |
|---|---|
| `ntoskrnl/` | Executive + kernel core (`ex/`, `ke/`, `ob/`, `mm/`, `io/`, `se/`, `fs/`, `cc/`) |
| `hal/` | Hardware Abstraction Layer (`hal/x86`, `hal/x64`, `hal/arm64`) |
| `win32ss/` | User-mode subsystems: `user/`, `gdi/`, `ntuser/`, `explorer/` |
| `dll/` | Win32 DLLs: `ntdll`, `kernel32`, `advapi32`, `user32`, `gdi32`, `msvcrt`, `shlwapi`, ... |
| `base/` | Base libraries (`base/ntdll`, `base/appmodel`, `base/services`) |
| `sdk/` | Headers, `sdk/include/reactos/` (incl. `version.cmake`), `sdk/libs/` |
| `drivers/` | System drivers (storage, network, bus, video) |
| `subsystems/` | System subsystems (mconfig, test) |
| `boot/` | Bootloaders and the build's livecd tree |
| `modules/` | Extra drivers |

## Method

Follow this order. Do not skip step 1.

### 1. Establish the revision first

```
git rev-parse HEAD
git describe --tags --always
```

Never analyse a file without knowing which revision you are reading.

### 2. Locate the implementation

Start from the public API name, then walk inward.

- SDK stubs and exports: `sdk/include/`, `sdk/lib/`, `dll/ntdll/`, `dll/win32ss/`
- Native syscall implementations: `ntoskrnl/ex/` (`Nt*`, `Zw*`, `Rtl*`)
- Kernel internals: `ntoskrnl/ke/`, `ntoskrnl/mm/`, `ntoskrnl/ob/`, `ntoskrnl/io/`, `ntoskrnl/se/`

Use `glob` for `**/<subsystem>/*` and `grep` for the symbol. Check both the declaration (header) and the definition.

### 3. Determine the interface class

For any given API, state explicitly which class it belongs to:

- **Documented** — appears in MSDN / WDK headers.
- **Internal** — used only inside ReactOS or between components, never exported to apps.
- **Undocumented/native** — reachable from apps via `Nt*`/`Zw*` or by ordinal, but undocumented.

For NT 6.1 compatibility this distinction matters: Win7 applications often call `Nt*`/`Zw*` directly, so "undocumented native" behaviour is in scope, not out of scope.

### 4. Map dependencies

For each function you plan to touch, list:

- Its **callers** (in-tree and, where relevant, which DLLs call it).
- Its **callees**.
- The **structs** it operates on and where those structs are defined.
- Any **build-system** reference (`CMakeLists.txt`) that would need updating if a file were added/removed.

### 5. Check the NT 5.2 → NT 6.1 delta

The ReactOS code base historically targeted the NT 5.2 era. Known classes of NT 6.1 (Win7) change to investigate:

- New/changed **system calls** and their numbering in the `SSN` table (`ntoskrnl/.../ssnt.c` or equivalent).
- **KUSER_SHARED_DATA** layout changes (WoW64 vs native).
- New **structures**: `PEB`, `TEB`, `RTL_USER_PROCESS_PARAMETERS`, `KPROCESS`, `OBJECT_HEADER`.
- **`WOW64`** differences — highly relevant, since the available reference VM is x86 while Win7 x64 uses WOW64 for 32-bit apps.
- **APISETS** (`apiset.schema`) — a major Win7 mechanism for API forwarding.
- **Heap changes** in `ntdll`.
- **DirectX / DWM / Aero** — out of scope unless explicitly targeted.

Do **not** assert any of these as present or absent without reading the source.

### 6. Document the execution path

Produce a written trace, for example:

```
App call:  CreateFileW
  -> kernel32.dll!CreateFileW          (dll/win32ss/kernel32/client/file.c)
  -> ntdll!NtCreateFile               (dll/ntdll/ntcreatefile.c)
  -> syscall stub -> NtCreateFile      (ntoskrnl/ex/ntcreatefile.c)
  -> I/O manager dispatch             (ntoskrnl/io/iomgr.c)
  -> ObReferenceObjectByHandle        (ntoskrnl/ob/)
  -> FsCreateFile -> CCB -> Cache     (ntoskrnl/fs/, cc/)
  -> IRP to volume/filesystem driver  (drivers/storage/)
```

Include real file paths and, where useful, `file:line` references.

## Output format

```
## <Feature or API>
Revision analysed:  <git describe>
Component:          <subsystem + path>

### Execution path
1. <step>  ->  path/file.c:line

### Public surface
- Exported name(s):
- Interface class: documented | internal | undocumented-native

### Key structures
- <STRUCT> — defined in <path>

### Dependencies
- Callees: ...
- Callers: ...
- Build references: ...

### NT 6.1 (Win7) relevance
- Observed in ReactOS: ...
- Observed on Win7 reference: ...   (mark as NOT_TESTED if unverified)

### Confidence
- FACT: ...
- HYPOTHESIS: ...
- NOT INVESTIGATED: ...
```

## Rules

- Always read a file in full before describing its behaviour.
- Never infer behaviour from a function name alone.
- Never claim compatibility without a reproducible observation.
- Record `NOT_TESTED` rather than leaving a gap implicit.