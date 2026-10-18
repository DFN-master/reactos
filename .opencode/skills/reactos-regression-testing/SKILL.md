---
name: reactos-regression-testing
description: Design, run and report reproducible ReactOS regression tests that separate pre-existing failures from newly introduced regressions, with results tracked per architecture. Use when validating a change or when deciding whether a change is safe to propose.
license: GPL-2.0
compatibility: opencode
metadata:
  audience: reactos-nt61-project
  workflow: regression-testing
  opencode/autoinvoke: "true"
---

# ReactOS Regression Testing

## Purpose

Prove that a change did not break existing behaviour, and **prove** that a new behaviour works — on the specific architecture claimed.

## Core principles

1. **A baseline is mandatory.** Every test must have a recorded **before** result. A test without a baseline cannot detect a regression.
2. **Never disable, skip, quarantine or weaken a test to make a run pass.** If a test genuinely must not run, mark it explicitly as `BLOCKED` with a reason.
3. **Never silently fix the code under test.** If testing reveals a bug in production code, report it. Do not patch it in the same step.
4. **Architecture-qualify everything.** x86 results do not transfer to x64.
5. **Distinguish pre-existing failure from regression.** This is the single most important classification in this project.

## Test tiers

| Tier | Runs on host | Runs in VM | Purpose |
|---|---|---|---|
| **T1 — Static** | yes | no | Build succeeds, config generates, no new warnings of substance. |
| **T2 — Unit** | yes | no | Logic testable in isolation, outside the OS. |
| **T3 — User-mode API** | no | yes (in ReactOS VM) | An API called by a test app behaves as specified. |
| **T4 — Boot / integration** | no | yes | System boots; changed subsystem initialises. |

Tier 3 and 4 require a booted ReactOS VM (see `reactos-build-and-vm`). Record clearly when a tier could not be executed and why.

## Baseline procedure

Before any change:

1. Record git revision.
2. Build `ARCH=i386` → record status + artifact.
3. Boot the ReactOS VM → record status + debug log.
4. Run the applicable existing tests → record pass/fail **per test, per architecture**.
5. Store as `tests/regression/baseline-<revision>.md`.

This baseline is the reference for all future comparisons. A test failing at baseline is **pre-existing**, not a regression.

## Regression classification

For every test result, classify as exactly one of:

| Classification | Meaning |
|---|---|
| `PASS` | Passing now, and passing at baseline. |
| `REGRESSION` | Passing at baseline, failing now. **Blocks approval.** |
| `PREEXISTING_FAILURE` | Failing at baseline and still failing. Not caused by the change. |
| `NEWLY_PASSING` | Failing at baseline, passing now. Usually good — verify it is not accidental. |
| `NOT_RUN` | Not executed. Reason mandatory. |
| `INCONCLUSIVE` | Could not determine. Reason mandatory. |

`REGRESSION` may only be downgraded to `PREEXISTING_FAILURE` with explicit evidence that the test was already broken at the exact baseline revision.

## Test design rules

- **Small.** One test asserts one behaviour.
- **Reproducible.** Same result on repeated runs. Record flake observations.
- **Self-contained.** No network, no external services, no dependence on manual steps.
- **Observable.** A clear pass/fail signal with a captured value. Not "it seemed to work".
- **Version-agnostic in intent.** Record the Windows version observed on the reference.

## Test file conventions

```
tests/compatibility/<test-id>/
    README.md        # what this tests, acceptance criteria, how to run
    test.c           # the test program (ReactOS/Win32 C, built for the target)
    expected.md      # expected observable result, per architecture
    actual-<arch>.md # recorded result, per architecture

tests/regression/
    baseline-<revision>.md
    <suite-name>.md
```

Build tests as ordinary ReactOS modules so they compile inside the existing build system. Do not invent a parallel build mechanism.

## Acceptance criteria (define before implementing)

Every compatibility change needs written, checkable criteria:

```
- AC1: Calling <API> with <input> returns <value>.      (verifiable by: T3 test <id>)
- AC2: On failure it sets last error to <code>.         (verifiable by: T3 test <id>)
- AC3: Existing behaviour <X> is unchanged.              (verifiable by: <existing test>)
```

If a criterion cannot be made verifiable, it is not a criterion — rewrite it.

## Approval gate

A change **may** be proposed for integration only when:

- All applicable tiers were run on the claimed architecture, or have a stated `NOT_RUN` reason.
- Zero `REGRESSION`.
- Every new behaviour has a passing test.
- `PREEXISTING_FAILURE` items are enumerated and unchanged from baseline.
- Build succeeds.
- If boot-dependent: the VM boots, with the debug log captured.

If any critical test fails without justification, **block approval**. Do not approve "with caveats".

## Output format

```
## Regression report — <change-id>
- Git baseline: <describe>
- Git change:   <describe>
- Architectures: x86 <status>, x64 <status>

### Per-architecture results
| Test | Tier | Baseline | Now | Classification |
|------|------|----------|-----|----------------|
| ...  | T1   | PASS     | PASS | PASS           |

### Regressions
- <none>  OR  list with evidence

### Pre-existing failures (unchanged)
- <list>

### Not run / inconclusive
- <test>: reason

### Gate
APPROVED | BLOCKED  — <one-line justification>
```

## Rules

- Never edit a test to match a buggy implementation.
- Never copy the implementation's logic into the test; the test must assert the *specified* behaviour, independently.
- Record results even when they are ugly.
- A clean report from an unrun suite is not a pass. State it as `NOT_RUN`.