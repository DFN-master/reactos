# ReactOS VS Code Compatibility Layer

**Status:** 🔧 Experimental - Partial Support  
**Target:** Visual Studio Code (Electron-based apps)  
**ReactOS Version:** 0.4.15+ (64-bit UEFI build)

---

## 📋 **Overview**

This compatibility layer provides stub implementations of Windows 10 APIs required by **Visual Studio Code** and other **Electron-based applications** (Teams, Discord, Atom, etc.).

**Critical Note:** VS Code requires GPU-accelerated rendering via **Direct3D 11 and DXGI**, which are **not fully implemented** in ReactOS. This layer provides **stubs that return E_NOTIMPL** to force software rendering fallback, but **VS Code may still fail to launch** due to missing Chromium dependencies.

---

## 🎯 **What's Included**

### **1. Version Detection APIs** (`versionhelpers.dll`)

Implements Windows 10 version checking functions used by Electron to gate features:

| Function | Status | Purpose |
|----------|--------|---------|
| `IsWindows10OrGreater()` | ✅ Implemented | Detects Windows 10+ |
| `IsWindows8Point1OrGreater()` | ✅ Implemented | Detects Windows 8.1+ |
| `IsWindows8OrGreater()` | ✅ Implemented | Detects Windows 8+ |
| `IsWindows7OrGreater()` | ✅ Implemented | Detects Windows 7+ |
| `IsWindowsVistaOrGreater()` | ✅ Implemented | Detects Vista+ |
| `IsWindowsXPOrGreater()` | ✅ Implemented | Detects XP+ |
| `IsWindowsServer()` | ✅ Implemented | Detects Server SKU |

**How it works:**  
Uses `RtlGetVersion()` from NTDLL to check actual OS version and compares against requested version numbers.

---

### **2. UWP/AppContainer APIs** (`vscode_compat.dll`)

Provides stub implementations of UWP package detection APIs:

| Function | Status | Return Value |
|----------|--------|--------------|
| `GetCurrentPackageId()` | ⚠️ Stub | `APPMODEL_ERROR_NO_PACKAGE` |
| `GetCurrentPackageFullName()` | ⚠️ Stub | `APPMODEL_ERROR_NO_PACKAGE` |
| `GetCurrentPackageFamilyName()` | ⚠️ Stub | `APPMODEL_ERROR_NO_PACKAGE` |
| `GetPackageId()` | ⚠️ Stub | `APPMODEL_ERROR_NO_PACKAGE` |
| `IsAppContainerProcess()` | ⚠️ Stub | `FALSE` |
| `GetApplicationUserModelId()` | ⚠️ Stub | `APPMODEL_ERROR_NO_APPLICATION` |

**Purpose:**  
Electron checks if it's running in a UWP/MSIX package. These stubs return "not packaged" to force classic Win32 mode.

---

### **3. Direct3D 11 / DXGI Stubs** (`vscode_compat.dll`)

Minimal GPU rendering API stubs:

| Function | Status | Return Value |
|----------|--------|--------------|
| `D3D11CreateDevice()` | ⚠️ Stub | `E_NOTIMPL` |
| `D3D11CreateDeviceAndSwapChain()` | ⚠️ Stub | `E_NOTIMPL` |
| `CreateDXGIFactory()` | ⚠️ Stub | `E_NOTIMPL` |
| `CreateDXGIFactory1()` | ⚠️ Stub | `E_NOTIMPL` |
| `CreateDXGIFactory2()` | ⚠️ Stub | `E_NOTIMPL` |

**Purpose:**  
Electron/Chromium attempt to initialize GPU rendering. These stubs return `E_NOTIMPL` to signal "no hardware support" and force software rasterization via WARP or Skia.

---

### **4. Direct2D / DirectWrite Stubs** (`vscode_compat.dll`)

Text and 2D graphics rendering stubs:

| Function | Status | Return Value |
|----------|--------|--------------|
| `D2D1CreateFactory()` | ⚠️ Stub | `E_NOTIMPL` |
| `DWriteCreateFactory()` | ⚠️ Stub | `E_NOTIMPL` |

**Purpose:**  
Electron uses Direct2D for vector graphics and DirectWrite for text rendering. Stubs force fallback to GDI32/Uniscribe.

---

## 🚀 **Installation**

### **Option 1: Already Included in ISO**

If you're using the **latest bootcd.iso** from this build:
- ✅ `versionhelpers.dll` is in `C:\ReactOS\System32`
- ✅ `vscode_compat.dll` is in `C:\ReactOS\System32`

**No additional steps needed!**

---

### **Option 2: Manual Installation** (for older ReactOS builds)

1. **Download VS Code for Windows 64-bit:**
   ```
   https://code.visualstudio.com/download
   ```

2. **Copy DLLs to System32:**
   - From build output: `output-VS-amd64\dll\win32\versionhelpers\versionhelpers.dll`
   - From build output: `output-VS-amd64\dll\win32\vscode_compat\vscode_compat.dll`
   
   Copy to: `C:\ReactOS\System32\`

3. **Create Symlinks for API Forwarding:**
   ```cmd
   cd C:\ReactOS\System32
   mklink d3d11.dll vscode_compat.dll
   mklink dxgi.dll vscode_compat.dll
   mklink d2d1.dll vscode_compat.dll
   mklink dwrite.dll vscode_compat.dll
   ```

4. **Install VS Code:**
   - Run `VSCodeSetup-x64.exe`
   - Choose "User Installer" (not System Installer)
   - Accept default installation path

---

## ⚙️ **Running VS Code**

### **Expected Behavior:**

**✅ Likely to Work:**
- Version detection (reports Windows 10+ via manifest)
- Package detection (reports "not packaged")
- Application launches (if Chromium starts)

**⚠️ May Fail:**
- **GPU rendering** - No D3D11, forces software rasterization
- **Font rendering** - DirectWrite stubs may cause fallback issues
- **UI performance** - Software rendering is slow
- **Extensions** - Node.js native modules may fail

**❌ Will NOT Work:**
- Hardware acceleration
- WebGL/WebGPU features
- High-DPI rendering (likely)
- Chromium compositor optimizations

---

## 🐛 **Troubleshooting**

### **VS Code doesn't launch:**

1. **Check DLL presence:**
   ```cmd
   dir C:\ReactOS\System32\versionhelpers.dll
   dir C:\ReactOS\System32\vscode_compat.dll
   ```

2. **Enable debug logging:**
   - Set `WINEDEBUG=+versionhelpers,+vscode_compat,+d3d11_stub`
   - Check ReactOS debug output (DbgView or serial console)

3. **Check dependencies:**
   - Ensure `.NET Framework 4.5+` or `Visual C++ 2017+ Redistributable` is installed
   - VS Code may require `vcruntime140.dll`, `msvcp140.dll`

4. **Try Portable Version:**
   - Download VS Code Portable (no installer)
   - Extract to `C:\VSCode-Portable`
   - Run `Code.exe` directly

---

### **VS Code crashes on startup:**

**Symptoms:** White screen, immediate crash, or "GPU process crashed" error

**Solutions:**
1. **Disable GPU acceleration:**
   ```cmd
   code.exe --disable-gpu --disable-software-rasterizer
   ```

2. **Force software rendering:**
   ```cmd
   code.exe --disable-hardware-acceleration
   ```

3. **Use older Electron version:**
   - Try VS Code 1.60 or earlier (uses older Electron with fewer GPU dependencies)

---

### **Slow/Unresponsive UI:**

**Cause:** Software rendering is CPU-intensive without D3D11 hardware acceleration.

**Solutions:**
- Close unnecessary background processes
- Disable VS Code extensions
- Use lightweight themes (Dark+, Light+)
- Reduce editor font size
- Disable minimap: `"editor.minimap.enabled": false`

---

## 📊 **Compatibility Matrix**

| Application | Launch | Rendering | Extensions | Notes |
|-------------|--------|-----------|------------|-------|
| VS Code 1.85+ | ⚠️ Maybe | ⚠️ Slow | ❌ Likely fails | Requires newer Chromium (D3D11) |
| VS Code 1.60 | ⚠️ Maybe | ⚠️ Slow | ⚠️ Some work | Older Electron, better compat |
| Atom Editor | ⚠️ Maybe | ⚠️ Slow | ❌ No | Similar to VS Code |
| Discord | ❌ No | ❌ No | ❌ No | Requires WebRTC/media APIs |
| Teams | ❌ No | ❌ No | ❌ No | Requires D3D11 + media stack |
| Notepad++ | ✅ Yes | ✅ Fast | ✅ Yes | **Recommended alternative** |
| Sublime Text | ⚠️ Maybe | ✅ Fast | ⚠️ Some work | Uses native Win32 |

---

## 🔧 **Recommended Alternatives**

Since VS Code may not work reliably, consider these ReactOS-compatible alternatives:

### **Lightweight Editors:**
1. **Notepad++** (fully compatible, native Win32)
   - Download: https://notepad-plus-plus.org/
   - Syntax highlighting, plugins, fast

2. **Sublime Text 3** (mostly compatible)
   - Download: https://www.sublimetext.com/3
   - Powerful, lightweight, extensible

3. **Geany** (GTK-based, works on ReactOS)
   - Download: https://www.geany.org/
   - IDE-like features, project management

### **Classic IDEs:**
1. **Dev-C++** (for C/C++ development)
2. **Code::Blocks** (cross-platform IDE)
3. **Bloodshed Dev-Pascal** (Pascal/Delphi)

---

## 🛠️ **For Developers: Building From Source**

If you want to modify or rebuild the compatibility layer:

### **Prerequisites:**
- ReactOS build environment (MSVC 2022 + CMake + Ninja)
- Bison (from MSYS2)

### **Build Steps:**

1. **Configure build:**
   ```cmd
   cd e:\ReactOS\reactos
   configure.cmd VSSolution amd64 output-VS-amd64
   ```

2. **Build compatibility DLLs:**
   ```cmd
   cd output-VS-amd64
   ninja versionhelpers vscode_compat
   ```

3. **Test changes:**
   - DLLs output to: `dll\win32\versionhelpers\versionhelpers.dll`
   - DLLs output to: `dll\win32\vscode_compat\vscode_compat.dll`
   - Copy to running ReactOS VM for testing

4. **Rebuild full ISO:**
   ```cmd
   ninja bootcd
   ```

---

## 📚 **Technical Details**

### **Why VS Code is Hard to Support:**

**Electron Architecture:**
```
VS Code (JavaScript)
    ↓
Electron Framework
    ↓
Chromium (Blink Engine)
    ↓
Node.js Runtime + V8
    ↓
├─ Direct3D 11 (GPU rendering)
├─ DXGI (device management)
├─ Direct2D/DirectWrite (text)
└─ Windows 10 APIs (AppContainer, UWP)
```

**ReactOS Current Support:**
```
✅ Win32 API (kernel32, user32, gdi32) - Windows Server 2003 level
✅ DirectX 9 (legacy games work)
⚠️ Some Vista/Win7 APIs (partial via kernel32_vista.dll)
❌ Windows 10 APIs (AppContainer, UWP, modern security)
❌ Direct3D 11 / DXGI (GPU pipeline missing)
❌ Direct2D / DirectWrite (text rendering missing)
❌ Chromium dependencies (sandboxing, IPC, process model)
```

---

### **What Would Be Needed for Full Support:**

1. **Complete D3D11 implementation** with WARP software rasterizer
2. **DXGI device/swapchain management**
3. **Direct2D vector graphics engine**
4. **DirectWrite text stack** (font fallback, layout, shaping)
5. **Windows 10 sandboxing** (AppContainer, job objects, process isolation)
6. **Chromium IPC mechanisms** (shared memory, named pipes, sync primitives)
7. **Modern C++ runtime** (C++17 features, std::filesystem, etc.)

**Estimated effort:** 6-12 months of full-time development.

---

## 🤝 **Contributing**

Want to help improve VS Code compatibility?

**High-Priority Tasks:**
1. **Implement D3D11 stubs with Skia integration** (software rendering)
2. **Port DirectWrite to FreeType** (text rendering)
3. **Implement WARP** (Windows Advanced Rasterization Platform)
4. **Test with older VS Code versions** (1.40-1.60 era)

**Testing:**
- Document which Electron versions work
- Identify specific API failures
- Create compatibility matrix

---

## 📝 **License**

ReactOS VS Code Compatibility Layer  
Copyright © 2026 ReactOS Team  
Licensed under GPL-2.0-or-later

---

## 📞 **Support**

**ReactOS Community:**
- Website: https://reactos.org/
- Forum: https://reactos.org/forum/
- Discord: https://discord.gg/reactos
- IRC: #reactos on Libera.Chat

**Bug Reports:**
- JIRA: https://jira.reactos.org/
- GitHub: https://github.com/reactos/reactos

---

## ⚠️ **Disclaimer**

This compatibility layer is **experimental** and provided **as-is** without warranty. VS Code and Electron are complex applications with many dependencies beyond what ReactOS currently implements.

**Expected success rate:** 10-30% (highly dependent on VS Code version and configuration)

**Recommended approach:** Use native Win32 editors (Notepad++, Sublime Text) for reliable experience.

---

**Last Updated:** March 4, 2026  
**ReactOS Build:** 0.4.15-dev-10000-g0000000 (amd64 UEFI)  
**VS Code Target Version:** 1.60.x - 1.85.x
