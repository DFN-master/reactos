---
name: reactos-safe-implementation
description: Make small, scoped, evidence-driven changes to ReactOS source without breaking existing behaviour, preserving existing functions, structures and comments. Use when actually editing kernel, driver or user-mode code.
license: GPL-2.0
compatibility: opencode
metadata:
  audience: reactos-nt61-project
  workflow: implementation
  opencode/autoinvoke: "true"
---

# ReactOS Safe Implementation

## Purpose

Change as little as possible, as correctly as possible, with a test proving the change does what it claims.

This is an **operating-system** code base. A change that looks small can break booting.

## Order of operations — never skip a step

1. **Read the file in full before editing it.** Not a grep hit. The whole file, and its immediate callers.
2. **Check the working tree.** If there are pre-existing uncommitted changes you did not make, **stop and ask**. Do not overwrite them.
3. **Confirm the gap is real.** A gap needs evidence from `rea-reverse-engineering` and `reactos-architecture`. No evidence, no change.
4. **Write the acceptance criteria.** See `reactos-regression-testing`.
5. **Write the test that fails today.**
6. **Make the smallest change that satisfies the criteria.**
7. **Build** (`reactos-build-and-vm`).
8. **Run the test** (`reactos-regression-testing`).
9. **Boot the VM** if the change is kernel, driver or boot-adjacent.
10. **Record the diff and the results.** Do not commit unless authorised.

## Absolute prohibitions

These produce fake progress and are not acceptable:

- **No stub that fakes success.** Replacing a real operation with `return STATUS_SUCCESS;` is forbidden, even temporarily. If an operation is unimplementable now, leave the existing behaviour and report `BLOCKED`.
- **No silently removed functionality.** Do not delete, comment out, `#if 0`-out, or bypass existing code to make something pass.
- **No broad refactors.** Do not reformat, rename, restructure or "clean up" files you are not changing behaviour in.
- **No disabled tests.** No skipping, no `#ifdef`-ing out failures.
- **No invented APIs.** Never add a function, structure, constant or syscall number you have not verified exists in Windows 7.
- **No proprietary code.** Never paste decompiled Microsoft code, leaked NT source, or anything derived from them. Reimplement from understood behaviour.
- **No silent behaviour change.** If existing callers could observe a difference, that is a change — document it or don't make it.
- **No comment deletion.** Preserve existing comments and coding style; see `CODING_STYLE.md`.

## Change classification and required review

Classify every change before making it:

| Class | Area | Extra requirement |
|---|---|---|
| **C1** | Docs, research, tests outside the tree | Normal review. |
| **C2** | User-mode DLL / library logic | Normal review + regression suite. |
| **C3** | Kernel Executive subsystem (ob, se, io, fs, cc) | **Specialised review required.** Regression suite + VM boot + debug log. |
| **C4** | MM, ke, scheduler, boot, HAL | **Specialised review required, strongly discouraged in early phases.** VM boot mandatory, debug log mandatory, explicit risk assessment. |
| **C5** | Driver | **Specialised review required.** Must boot and load cleanly on the target VM. |

If a change would need C4, stop. Escalate. Early compatibility work should be able to stay in C2/C3.

## Preserving existing behaviour

Before editing, enumerate what the current code guarantees, then ensure your change preserves each guarantee:

- Function signatures and calling conventions — do not change them unless the change *is* the API.
- Return values in **all** branches, including error branches.
- Side effects: registry keys, files, handles, object references, IRP completion.
- Structure layouts — changing a struct layout has ABI consequences for drivers. Treat as C4.
- Export lists (`.def` files, `spec` files, CMake source lists). A new file must be registered in `CMakeLists.txt`; a removed file must be unregistered.

## Scope discipline

- Touch only what the task requires. Unrelated cleanup is a separate change.
- Keep diffs small enough to review line by line.
- If you discover other bugs, **report them separately**; do not fix them inline.
- If the change turns out to need more than the authorised scope, stop and report rather than expanding.

## Self-review before reporting

```
[ ] Full file read before editing?
[ ] Working tree clean of pre-existing user changes?
[ ] Gap backed by evidence, with artifact and revision cited?
[ ] Acceptance criteria written?
[ ] Test exists and failed before the change?
[ ] Change is the smallest thing that works?
[ ] No stub faking success?
[ ] No removed/disabled functionality?
[ ] No unrelated edits in the diff?
[ ] Callers all still consistent?
[ ] CMakeLists.txt updated if files changed?
[ ] Style matches surrounding code?
[ ] Build passes?
[ ] Regression suite run, no REGRESSION?
[ ] VM booted if C3/C4/C5?
[ ] Change class declared?
[ ] Risks and limitations written down?
```

## Report format

```
## Change report — <id>
- Class: C1|C2|C3|C4|C5
- Scope: <one line>
- Evidence for the gap: <links>

### Files changed
| File | What changed | Why |
|------|--------------|-----|

### Behaviour
- Before: <observable behaviour>
- After:  <observable behaviour>
- Unchanged and verified: <list>

### Verification
- Build: <status, arch>
- Test: <test id>, before FAIL / after PASS
- Regression: <gate result>
- VM boot: <result, or N/A with reason>

### Risks and limitations
- <explicit>

### Not done
- <explicit, so the next agent does not assume it>
```