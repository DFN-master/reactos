---
name: reactos-git-integration
description: Control branches, commits and integration in the ReactOS fork safely, preserving pre-existing local work and requiring explicit authorisation before commit, merge, push or history rewrite. Use when preparing a change for integration or managing task branches.
license: GPL-2.0
compatibility: opencode
metadata:
  audience: reactos-nt61-project
  workflow: git-integration
  opencode/autoinvoke: "true"
---

# ReactOS Git Integration

## Purpose

Keep every change traceable, isolated, reviewable, and reversible — without ever losing work or performing an unauthorised write to a remote.

## Repository facts (verified 2026-10-18 — re-verify if stale)

| Item | Value |
|---|---|
| Path | `E:\Projeto ReactOS\reactos` |
| Branch | `master` |
| Base revision | `0345c201b4f02c63d5c69b7c871fc285e0a1e285` |
| `git describe` | `0.4.16-dev-3517-g0345c201b4f` |
| `origin` | `https://github.com/DFN-master/reactos.git` |
| `upstream` | `https://github.com/reactos/reactos.git` (configured, **not fetched locally**) |
| Working tree at audit | clean |
| Submodules | none |
| License | GPL-2.0 |

`HEAD` was identical to `origin/master` at audit time. **Do not assume this is still true.**

## Standing prohibitions

Without explicit, current authorisation, you must **not**:

- `git commit`
- `git push` (to any remote)
- `git merge` / `git rebase`
- `git reset --hard`, `git checkout --`, `git clean`
- `git rebase`, `git cherry-pick`, `git revert`
- create, delete, or rewrite any branch or tag
- `git fetch` from a remote (changes local refs)
- alter `origin` / `upstream` URLs or fetch refspecs
- `git config` changes
- force-push anything, ever

These are hard limits. If work seems finished and you cannot commit, **report the prepared state instead** and let the human decide.

## Always safe (read-only)

```
git status
git diff
git log --oneline -10
git show
git rev-parse HEAD
git describe --tags --always
git branch --show-current
git remote -v
git diff --stat
```

Read-only commands never need permission.

## Pre-flight before touching the working tree

Always run and record:

```
git status --porcelain
```

- **Clean** → proceed.
- **Has changes you recognise as your own** → note them; do not discard.
- **Has changes you do NOT recognise** → **stop and ask.** These are the user's pre-existing work. Never overwrite, never stash without permission, never reset.

This check is mandatory before every edit session, not just at project start.

## Branch strategy

- The default branch `master` is **not** a working branch. Do not commit to it.
- One isolated branch per task/change:
  ```
  nt61/<issue-id>-<short-slug>
  ```
  Example: `nt61/NT61-0001-ntquerysystemtime-info`
- Creating a branch from the current `HEAD` is **only** allowed once explicitly authorised. Before that, plan the branch name and report it.
- Never work directly on `master`, `upstream/master`, or any ReactOS release branch.

## Change scoping

- One task, one branch. Do not mix unrelated fixes.
- Keep commits small and descriptive (see below).
- Never fold unrelated files into a change "because they were already there".
- Files created by this project (`research/`, `nt61/`, `tests/`, `agents/`, `.opencode/`) belong to this work and are currently **untracked**.

## Commit message convention

Match the existing repository style — check recent history first:

```
git log --oneline -20
```

ReactOS commits typically look like:
```
[SUBSYS] Short imperative description (#NNNN)
```

Proposed format for this project (state it before applying):

```
nt61: <component> - <short description>

Change-ID: NT61-XXXX
Issue: <link or TBD>

<why the change is needed, 1-3 lines>

Evidence: <research/evidence/...>
Tests:   <tests/compatibility/...>

No functional change outside <path>.
```

## Required pre-commit dossier

Present this to the human **before** any commit or push is considered:

```
### Branch
  source: <branch>
  destination: <branch or remote/ref>

### Files changed
  <list>

### Diff summary
  <git diff --stat>

### Justification
  <why>

### Build result
  ARCH=i386:  <pass/fail/not run>
  ARCH=amd64: <pass/fail/not run>

### Test result
  <per-architecture, with REGRESSION count>

### VM boot result (if applicable)
  <result, or N/A with reason>

### Risks and limitations
  <explicit>

### Suggested commit message
  <exact message>
```

Do not proceed to commit without an explicit instruction to do so.

## Integrity rules

- Never discard uncommitted work. `git stash` is a hidden form of state change — ask first.
- Never force-push, under any circumstances.
- Never rewrite published history.
- If a merge or rebase produces conflicts, stop and report. Do not resolve conflicts by discarding a side.
- If `git status` shows unexpected deletions, stop immediately.
- If the upstream relationship matters, a `git fetch upstream` is required — request authorisation rather than guessing.
- If another agent works concurrently, each must use its own branch; detect conflicting edits and report rather than merging blindly.

## Reporting

Always end Git work with:

```
## Git report
- Working tree: clean | <files listed>
- Branch: <current>
- HEAD: <sha>
- Commits created: none | <list>
- Remotes contacted: none | <which>
- Pushed: no
- Recommended next git action: <proposed, awaiting authorisation>
```