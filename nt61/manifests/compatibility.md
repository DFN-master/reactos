# ReactOS NT 6.1 — Compatibility Inventory

**Status:** empty manifest. No compatibility items have been investigated yet.
**Last updated:** 2026-10-18
**Schema:** see `nt61/manifests/` and the `nt61-compatibility` skill.

## How to read this file

- Each item is a small, bounded, verifiable compatibility gap or behaviour.
- `state` follows the machine defined in `nt61-compatibility`:
  `NOT_TESTED → INVESTIGATING → SPECIFIED → IMPLEMENTED → TESTED_X86 / TESTED_X64 / TESTED_X86_X64`, plus `REGRESSION`, `BLOCKED`.
- `win7_result` and `reactos_result` are recorded **independently**.
- A result on x86 is **NOT** proof for x64.

## Items

```yaml
items:
  - id: NT61-0001
    name: SystemProcessorIdleInformation (NtQuerySystemInformation, class 42)
    component: ntoskrnl (Executive, ex/sysinfo.c)
    reactos_paths:
      - ntoskrnl/ex/sysinfo.c (lines 2076-2088)
    architectures: [x86, x64]
    state: INVESTIGATING
    classification: ABSENT   # ReactOS returns STATUS_NOT_IMPLEMENTED
    evidence:
      - research/evidence/ (pending — REA analysis of ntoskrnl case not yet isolated)
    dependencies:
      - KeNumberProcessors
      - PROCESSOR_POWER_INFORMATION struct
    tests:
      - modules/rostests/apitests/ntdll/NtQuerySystemInformation.c (to be extended)
    win7_result: NOT_TESTED
    reactos_result: NOT_TESTED  # STATUS_NOT_IMPLEMENTED
    observed_divergence: "ReactOS returns STATUS_NOT_IMPLEMENTED; Windows 7 returns STATUS_SUCCESS with SYSTEM_POWER_INFORMATION filled"
    risks:
      - C3: kernel Executive change
      - No REA verification of the specific case yet
      - Hardware-dependent behavior unknown
    next_action: "Isolate SystemProcessorIdleInformation case in Windows 7 ntoskrnl via REA (jump table analysis), then write test and implement"
    specification: nt61/specifications/NT61-0001-systempowerinformation.md

  - id: NT61-0002
    name: GetSystemFileCacheSize (kernel32)
    component: kernel32 (client/sysinfo.c)
    reactos_paths:
      - dll/win32/kernel32/client/sysinfo.c
    architectures: [x86, x64]
    state: IMPLEMENTED
    classification: ABSENT  # was STUB (returned FALSE)
    evidence: []
    dependencies:
      - NtQuerySystemInformation(SystemFileCacheInformation)
    tests: []
    win7_result: NOT_TESTED
    reactos_result: NOT_TESTED  # implemented but not tested
    observed_divergence: "ReactOS returned FALSE (STUB); Windows 7 returns TRUE with cache size values"
    risks:
      - C2: user-mode DLL change (low risk)
      - Field mapping (MinimumWorkingSet -> min, MaximumWorkingSet -> max) not verified on Windows 7
      - NtSetSystemInformation(SystemFileCacheInformation) still not implemented
    next_action: "Write apitest, run on ReactOS VM, verify on Windows 7 reference"
    specification: nt61/specifications/NT61-0002-getsystemfilecachesize.md
```

## Selection log

| Date | Decision | Rationale |
|------|----------|-----------|
| 2026-10-18 | No item selected | Baseline not yet established; RAM blocker unresolved; first experiment must be chosen on evidence. |

## Notes

- Do **not** add an item here until `rea-researcher` has produced an evidence file and `nt61-architect` has located the ReactOS counterpart.
- State changes beyond `NOT_TESTED` require a citation to `research/evidence/`.