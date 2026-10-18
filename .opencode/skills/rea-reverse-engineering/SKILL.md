---
name: rea-reverse-engineering
description: Use the connected REA MCP server (Ghidra 12.1.3 backend on Windows) to investigate Windows 7 reference binaries and produce reproducible, evidence-backed analysis reports. Use when you need to determine actual observed behaviour of a Windows component instead of guessing.
license: GPL-2.0
compatibility: opencode
metadata:
  audience: reactos-nt61-project
  workflow: reverse-engineering
  opencode/autoinvoke: "true"
---

# REA Reverse Engineering

## Purpose

Establish what a Windows component **actually does**, using the REA MCP tools that are genuinely available on this host — and record it so another agent can reproduce the finding.

## Environment facts (verified 2026-10-18 — re-verify if stale)

| Item | Value |
|---|---|
| MCP server | `rea`, package `rea-agents` v6.5.0 |
| Config location | **global** `~/.config/opencode/opencode.jsonc` → `mcp.rea`, `enabled: true` |
| Working analysis backend | **Ghidra 12.1.3** |
| Ghidra install | `C:\Users\Douglas\Downloads\liadmap\tools\ghidra\ghidra_12.1.3_PUBLIC` |
| JDK | Eclipse Adoptium 21.0.12.1 (64-bit) |
| Hopper | **unavailable on Windows** (`unsupported_host`) |
| IDA | **not configured** |

**Hard limits of this backend on Windows (do not plan around otherwise):**
- Native **x86 / x64 PE** binaries and DLLs only. Raw 6502 also supported.
- **.NET / managed assemblies are NOT supported** by the native provider.
- **Database mutation is disabled** (`unsupported_host`). Metadata edits are session-only and are not persisted to a project.
- Analysis sessions are per-target: opening a new binary replaces the previous one.

Verify current capability with a non-destructive `rea_binary_session` call before relying on any of the above.

## Non-negotiable rules

1. **Decompiled pseudocode is NOT source code.** Never describe it as "the original source", and never present it as such.
2. **Never copy proprietary or leaked Microsoft code into this repository.** ReactOS is GPL-2.0 and the project explicitly rejects contributions derived from leaked NT source. Analysis is for *understanding behaviour*, then reimplementation from understanding.
3. **Separate observation from inference.** Every claim is `[FATO]` (observed in the tool output), `[INFERIDO]` (reasoned from observation), or `[HIPOTESE]` (plausible, unverified).
4. **Record the artifact identity** — exact file, exact path, and digest — for every analysis. Findings without a traceable artifact are worthless.
5. **Never analyse a target you cannot legally access.** Use only binaries the project is entitled to inspect.

## Workflow

### Step 1 — Check capability and open the target

Non-destructive capability probe:
```
rea_binary_session {}
```

Then open the artifact (absolute path required):
```
rea_open_binary { "path": "<ABSOLUTE_PATH>" }
```

If the artifact is a Windows DLL, prefer a **specific architecture**. For a 32-bit Win7 DLL use the native provider; do not open a 64-bit DLL and assume the 32-bit behaviour matches.

### Step 2 — Locate the entry point

Use the address-centric tools. In preference order:

- `rea_procedure_address` — resolve a name to its canonical entry address.
- `rea_search_procedures` / `rea_search_strings` — find candidates.
- `rea_xrefs` — find references to a known address.
- `rea_analyze_function` — get the full dossier (pseudocode, assembly, calls, references, strings) in one call.

`rea_analyze_function` gives the most complete picture; prefer it over piecemeal calls when you want a single comprehensive record.

### Step 3 — Trace behaviour, not just presence

A function existing is **not** evidence of compatibility. You must establish:

- What it **returns** in each branch (success, failure, error code).
- What **side effects** it has (registry, file, handle table, memory).
- Whether **error paths** are implemented or are stubs returning success.
- Whether it matches the documented contract at all.

### Step 4 — Compare with ReactOS

Open the corresponding ReactOS source (use the `reactos-architecture` skill) and compare **behaviour**, not names:

| Question | How to answer |
|---|---|
| Same signature? | Compare header declarations |
| Same semantics? | Compare return values and error codes |
| Same side effects? | Trace both |
| Same error codes? | Enumerate branches on both sides |

### Step 5 — Write the evidence file

Store findings in `research/evidence/`. Use the format below.

## Output format

Store as `research/evidence/<YYYY-MM-DD>-<subject>.md`.

```
# Evidence: <subject>

## Artifact
- File: <absolute path>
- Digest (SHA-256): <if known>
- Architecture: x86 | x64
- Claimed product/version: <only if verified>
- Source of artifact: <legitimate path>

## Tooling
- REA: rea-agents 6.5.0
- Backend: Ghidra 12.1.3
- Commands run: <exact list>

## Observations
### [FATO] <what was directly observed>
  - Address: <0x...>
  - Procedure: <name>
  - Evidence: <pseudocode excerpt / disassembly / call list>

### [INFERIDO] <what follows from the observation>

### [HIPOTESE] <what is plausible but unverified>

## Limits of this analysis
- <e.g. "only x86 analysed; x64 not verified">
- <e.g. "caller context unknown; behaviour may depend on caller">

## ReactOS comparison
- Corresponding source: <path/file.c:line>
- Matches: <...>
- Diverges: <...>
- Untested: <...>
```

## Rules for the report

- Include the **exact tool calls** so the analysis is reproducible.
- State architecture explicitly. x86 evidence does **not** transfer to x64.
- Always fill "Limits of this analysis". An empty limits section means the analysis is incomplete.
- Store analysis **conclusions**, not the raw tool dumps. Summarise; keep excerpts minimal and targeted.
- Never write a proprietary snippet into any repository file. Quote only what is necessary to justify a behavioural statement, and prefer paraphrase when a paraphrase is sufficient.