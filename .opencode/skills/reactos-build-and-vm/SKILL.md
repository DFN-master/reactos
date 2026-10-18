---
name: reactos-build-and-vm
description: Build ReactOS with CMake and MSVC for i386 and amd64, and boot the result in a dedicated VirtualBox VM with log collection. Use when compiling the tree, producing a bootcd, or validating a build boots without modifying reference VMs.
license: GPL-2.0
compatibility: opencode
metadata:
  audience: reactos-nt61-project
  workflow: build-and-vm
  opencode/autoinvoke: "true"
---

# ReactOS Build and VM

## Purpose

Produce a reproducible build of the ReactOS tree per architecture, and boot it in a VM that is **owned by this project**.

## Environment facts (verified 2026-10-18 — re-verify if stale)

| Item | Value |
|---|---|
| Repo | `E:\Projeto ReactOS\reactos` |
| CMake | `4.3.2` |
| Ninja | `1.13.0` — **`C:\Users\Douglas\AppData\Roaming\Python\Python314\Scripts\ninja.exe`, NOT on PATH** |
| MSVC | VS2022 Community, toolset `14.44.35207`, `cl.exe` x86 + x64 |
| `vcvarsall.bat` | `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat` |
| VirtualBox | `7.2.6r172322` — **`C:\Program Files\Oracle\VirtualBox\VBoxManage.exe`, NOT on PATH** |
| Host RAM | 31.9 GB total; **only ~0.5 GB free at audit time** |
| Disk free | C: 44.7 GB · D: 134.9 GB · E: 595.5 GB |

> **BLOCKER:** a full ReactOS build plus a running VM needs far more than 0.5 GB free RAM. Free memory on the host **before** starting a baseline build.

## Build procedure (MSVC — the documented alternative to RosBE)

Run everything inside one `cmd.exe` shell that has already been initialised with `vcvarsall`, because `cl.exe` is not on `PATH` by default.

```cmd
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
set PATH=%PATH%;C:\Users\Douglas\AppData\Roaming\Python\Python314\Scripts
cd /d E:\Projeto ReactOS\reactos
cmake --preset msvc-i386-debug
ninja
```

Key points:

- `vcvarsall x64` sets up a 64-bit **host** toolchain. That is independent of the ReactOS **target** `ARCH`. You build `ARCH=i386` targets from a 64-bit host all the time.
- Use `vcvarsall.bat x86` only if a 32-bit host toolchain is actually needed.
- `cmake --preset <name>` writes to the preset's `binaryDir`. Presets already separate architectures:
  - `msvc-i386-*` → `output-VS-i386`
  - `msvc-amd64-*` → `output-VS-amd64`
- **Keep build directories separate per architecture and per configuration.** Do not reuse one build dir for i386 and amd64.
- Build incrementally with `ninja <module>` when iterating; use plain `ninja` for a full baseline.
- `ninja bootcd` produces the bootable ISO in the build directory.
- `ninja install` with `ROS_INSTALL` set copies into an existing installation.

ReactOS source ships a CMake version requirement (`cmake_minimum_required(VERSION 3.17.0)`), and the presets require CMake >= 3.19. The installed 4.3.2 satisfies both, but **a very new CMake can surface new strictness checks** — if configure fails on policy warnings, record the exact error rather than guessing.

## Build discipline

1. Record, for every build: preset name, CMake version, MSVC toolset version, git revision, start/end time, exit status.
2. **A successful compile is not a validation.** It proves nothing about behaviour.
3. Never "fix" a failing build by deleting or skipping a test, or by weakening a compiler error setting.
4. If a build fails, capture the first real error, not the last line of output.
5. Do not modify production code merely to make a build go green.

## VM rules — safety first

### VMs that must never be touched

| VM | Status | Rule |
|---|---|---|
| `Win7Test` | **Windows 7 reference** | **READ-ONLY.** Never boot for modification, never attach/disk, never delete, never snapshot-over. |
| `Win7USB` | Broken (disk file missing) | Do not repair or modify without explicit authorisation. |
| `Windows 10`, `Ubuntu Server - Lara` | Unrelated | Do not use for ReactOS testing. |

### Hard rules

1. **Never attach, write to, resize, or delete `D:\Win7Test.vdi`.** It is the only real Windows 7 disk on this host.
2. **Never boot the reference VM for modification.** Read-only observation only, and only with authorisation.
3. **Never use a Windows 7 disk as a ReactOS test disk.** Create a new VMDK/VDI.
4. **The ReactOS test VM must be created fresh**, on its own new disk, on a drive with free space (prefer `D:\` or `E:\`).
5. **Take a snapshot immediately after creating the ReactOS VM**, so you have a known-good restore point before experimenting.
6. Do not start two VMs that reference the same medium. Note that `Win7Test` and `Win7USB` currently share a medium UUID (`19176b2b-...`) whose file is missing — a pre-existing hazard; do not attempt to repair it unasked.

### Safe interaction pattern

Use only read-only `VBoxManage` queries for inspection:
```
VBoxManage list vms
VBoxManage showvminfo <vm> --machinereadable
VBoxManage snapshot <vm> list
VBoxManage list runningvms
```

For starting/stopping a VM **that you created**, prefer headless and detached:
```
VBoxManage startvm "<vm>" --headless
VBoxManage controlvm "<vm>" acpipowerbutton
```

Do not use `VBoxManage controlvm ... poweroff` as a first resort; `acpipowerbutton` lets the guest shut down cleanly and avoids filesystem corruption in the guest.

### Log collection

ReactOS debug output is written to files inside the guest (typically on the booted media, e.g. `ReactOS\debug.log`). To collect logs you must mount the guest disk or export the file. Note:

- Mounting a disk is a **write-capable** operation on that disk — only ever do it on a **ReactOS VM you created**.
- Never mount the reference Windows 7 disk.
- Alternative that avoids disk access: enable a VirtualBox serial port and capture to a host file, then configure ReactOS debug output to the serial console. This requires ReactOS debug-build serial support and is the preferred non-invasive route.

## Failure catalogue — record these, do not hide them

| Symptom | What it means | Action |
|---|---|---|
| `cl.exe` not found | `vcvarsall` not run in this shell | Re-run in same shell |
| `ninja` not found | Ninja dir not on `PATH` | Prepend the Python Scripts dir |
| Configure fails on policy | Newer CMake is stricter | Record exact error |
| VM fails to start with medium error | Disk file missing | Stop; do not recreate the reference disk |
| Guest boots then reboots loop | Kernel/driver fault | Collect debug log, do not guess |
| Guest boots cleanly | **Only** proves boot works | Record as boot result, not as compatibility |

## Output format

Record per architecture:

```
## Build report — ARCH=<i386|amd64>
- Preset: msvc-<arch>-<config>
- Git revision: <describe>
- CMake: <version>
- MSVC toolset: <version>
- Ninja: <version>
- Build directory: <path>
- Started / Finished: <timestamps>
- Exit status: <0 = success>
- Output artifact: <path to .iso / files>

## VM boot report
- VM: <name>  (must be a ReactOS-owned VM)
- Disk: <newly created path>
- Snapshot taken: yes/no (<name>)
- Boot result: booted | failed | untested
- Debug log collected: yes/no (<path>)
- Observations: <fact-based>
- Not established: <explicitly what is still unproven>
```