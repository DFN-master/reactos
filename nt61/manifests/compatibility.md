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
# (empty — no item has yet been selected or evidenced)
# First item will be added only after the baseline is established and
# the first experiment is chosen with evidence.
items: []
```

## Selection log

| Date | Decision | Rationale |
|------|----------|-----------|
| 2026-10-18 | No item selected | Baseline not yet established; RAM blocker unresolved; first experiment must be chosen on evidence. |

## Notes

- Do **not** add an item here until `rea-researcher` has produced an evidence file and `nt61-architect` has located the ReactOS counterpart.
- State changes beyond `NOT_TESTED` require a citation to `research/evidence/`.