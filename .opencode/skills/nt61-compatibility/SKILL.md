---
name: nt61-compatibility
description: Track and progressively implement Windows 7 SP1 (NT 6.1) compatibility gaps in ReactOS with an explicit evidence-backed inventory and per-architecture state tracking. Use when selecting the next compatibility target or when recording the status of an API, service or interface.
license: GPL-2.0
compatibility: opencode
metadata:
  audience: reactos-nt61-project
  workflow: compatibility-tracking
  opencode/autoinvoke: "true"
---

# NT 6.1 Compatibility

## Purpose

Maintain a rigorous, evidence-backed inventory of NT 6.1 compatibility in ReactOS, and drive one small verified gap to closure at a time.

Target: **Windows 7 SP1**, the NT 6.1 family.

## Architecture rule — the most important rule in this project

**A result on x86 is NOT proof for x64, and vice versa.**

Windows 7 exists in both 32-bit and 64-bit forms. They differ substantially:
- x64 has no `WOW64`-under-x86 story; instead, 32-bit apps run under **WOW64 on x64**.
- System structures, syscall numbering, and exception handling differ.
- The ReactOS build exposes `ARCH=i386` and `ARCH=amd64` as **separate builds** with separate CMake presets.

Therefore every status value must be architecture-qualified. Never write plain `TESTED` — write `TESTED_X86` or `TESTED_X64`.

## State machine

Use exactly these states, and only these:

| State | Meaning |
|---|---|
| `NOT_TESTED` | Nothing has been observed. The default. |
| `INVESTIGATING` | Analysis in progress; no conclusion yet. |
| `SPECIFIED` | Expected behaviour and acceptance criteria written down, not yet implemented. |
| `IMPLEMENTED` | Code exists in ReactOS, but not yet validated against Win7. |
| `TESTED_X86` | Verified **on x86 only**. |
| `TESTED_X64` | Verified **on x64 only**. |
| `TESTED_X86_X64` | Verified on **both**, as two separate observations. |
| `REGRESSION` | Previously working behaviour broke. |
| `BLOCKED` | Cannot proceed; blocker must be stated. |

Transitions that are **not** allowed:
- `IMPLEMENTED` → `TESTED_*` without a reproducible observation on the stated architecture.
- `TESTED_X86` → `TESTED_X64` by assumption or by code inspection alone.

## Selection criteria — choose by evidence, not by wish

Pick the next target only when it is justified by baseline findings. Prefer, in order:

1. **High application impact** — a gap that visibly breaks real Win32 applications.
2. **Small, bounded surface** — one function or one small subsystem, provable in one test.
3. **Low coupling** — few callers/callees, so regression risk is low.
4. **Observable** — a clear, reproducible pass/fail signal exists.

Reject candidates that are:
- Kernel-wide or architectural (boot, MM, scheduler) at this stage.
- Vague ("improve Win32 compatibility") — never a valid entry.
- Chosen merely because a function name looks missing.

## Workflow for each compatibility item

1. **Identify** the API or behaviour and why it matters to real applications.
2. **Document** the reference behaviour from the Windows 7 system (use `rea-reverse-engineering`).
3. **Locate** the ReactOS implementation (use `reactos-architecture`).
4. **Classify**: `PRESENT` / `PARTIAL` / `DIVERGENT` / `ABSENT` — each with a code citation.
5. **Write the acceptance criteria** *before* implementing anything.
6. **Write a reproducible test** that fails today.
7. Implement the **smallest** change that satisfies the criteria (use `reactos-safe-implementation`).
8. Build and run the test on the relevant architecture(s).
9. Update the inventory with evidence and the new state.

## Inventory schema

Store at `nt61/manifests/compatibility.yaml` (or `.md` if a schema is not yet agreed). One record per item:

```yaml
- id: NT61-0001
  name: <short functional name>
  component: <subsystem>
  reactos_paths:
    - <path/file.c>
  architectures: [x86, x64]
  state: NOT_TESTED
  classification: NOT_INVESTIGATED   # PRESENT | PARTIAL | DIVERGENT | ABSENT
  evidence: []
    # - research/evidence/2026-01-01-foo.md
  dependencies: []
    # - NT61-0002
  tests:
    - tests/compatibility/<test-id>
  win7_result: NOT_TESTED
  reactos_result: NOT_TESTED
  observed_divergence: null
  risks: []
  next_action: <single concrete action>
```

## Rules

- Every `state` beyond `NOT_TESTED` must cite evidence. No evidence, no state change.
- `win7_result` and `reactos_result` are recorded **separately and independently**. Never fill in one from the other.
- Store specifications for larger items in `nt61/specifications/<id>-<slug>.md`.
- Record divergences; do not paper over them.
- Prefer one item fully closed over five items half-done.