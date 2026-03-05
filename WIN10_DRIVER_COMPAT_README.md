Windows 10 Driver Compatibility Layer for ReactOS
============================================================

OVERVIEW
--------
This document describes the Windows 10 driver compatibility layer implemented
in ReactOS, which allows drivers compiled for Windows 10 / Windows Server 2016+
to run on ReactOS with hardware support.

ARCHITECTURE
------------

1. DETECTION PHASE (ntoskrnl/io/iomgr/compat_shims.c)
   ├─ When a driver is loaded, IopInitializeCompatShim() is called
   ├─ Analyzes PE headers to detect target Windows version
   ├─ Sets compatibility flags based on detected version
   └─ Creates a DRIVER_COMPAT_CONTEXT for each driver

2. COMPATIBILITY CONTEXT
   ├─ COMPAT_FLAG_WIN10_DRIVER: Driver targets Windows 10+
   ├─ COMPAT_FLAG_WIN7_DRIVER: Driver targets Windows 7+
   ├─ COMPAT_FLAG_NEEDS_VALIDATION: IRP must be validated
   └─ COMPAT_FLAG_LEGACY_API_COMPAT: API compatibility enabled

3. VALIDATION PHASE
   ├─ IopValidateIrpMajorFunction() intercepts driver I/O
   ├─ Validates CREATE_FILE parameters for Win10-specific flags
   ├─ Handles FILE_INFORMATION_CLASS compatibility
   └─ Logs incompatible API calls for debugging

4. DRIVER LOADING FLOW
   ─────────────────────
   Load .sys file (MmLoadSystemImage)
             │
             ▼
   Create DRIVER_OBJECT (ObCreateObject)
             │
             ▼
   Initialize Compat Context (IopInitializeCompatShim) ◄─── NEW
             │
             ▼
   Call DriverEntry()
             │
             ▼
   Log Compatibility Info (IopLogDriverCompatibility) ◄─── NEW
             │
             ▼
   Ready Device Objects (IopReadyDeviceObjects)

SUPPORTED FEATURES
------------------

✓ Windows 10 Driver Detection
  - Automatically detects drivers compiled for Windows 10
  - Analyzes PE headers for subsystem version info
  - Maintains backward compatibility with existing drivers

✓ API Compatibility Shims
  - File Create Options (FILE_OPEN_NO_RECALL, etc.)
  - File Information Classes
  - Registry Access
  - Device I/O Control codes

✓ Memory Access Validation
  - Checks buffer sizes and alignment
  - Validates pointer validity
  - Prevents invalid memory operations

✓ Logging & Debugging
  - Logs driver compatibility details at load time
  - Debug output for incompatible API calls
  - Helps developers identify compatibility issues

KNOWN COMPATIBLE API SETS
--------------------------

1. Memory Management
   • ExAllocatePoolWithTag
   • ExFreePoolWithTag
   • ExAllocatePool
   • ExReAllocatePoolWithTag
   • MmAllocateContiguousMemory
   • MmAllocateNonCachedMemory

2. I/O Management
   • IoCreateDevice
   • IoDeleteDevice
   • IoAttachDevice
   • IoGetCurrentIrpStackLocation
   • IoSkipCurrentIrpStackLocation
   • IoCallDriver
   • IoCompleteRequest

3. Registry Access
   • ZwOpenKey / ZwCloseKey
   • ZwQueryValueKey
   • ZwSetValueKey
   • ZwEnumerateKey
   • RtlQueryRegistryValues

4. Synchronization
   • KeInitializeEvent
   • KeSetEvent
   • KeWaitForSingleObject
   • KeInitializeSpinLock
   • KeAcquireSpinLock / KeReleaseSpinLock

5. String Operations
   • RtlInitUnicodeString
   • RtlCopyUnicodeString
   • RtlCompareUnicodeString
   • RtlAppendUnicodeStringToString

INSTALLATION OF WIN10 DRIVERS
-----------------------------

Method 1: Automatic Detection (Recommended)
─────────────────────────────────────────────
Simply copy the driver .sys file to System32\Drivers\
The compatibility layer will automatically:
1. Load the driver
2. Detect its Windows version target
3. Apply appropriate compatibility shims
4. Log compatibility information

Method 2: Manual INF Installation
─────────────────────────────────────
Use the Windows 10 Compatibility INF:
  media/inf/win10_driver_compat.inf

Steps:
  1. Open Device Manager
  2. Right-click and select "Install Driver"
  3. Choose "Browse for driver software on this computer"
  4. Navigate to media\inf\
  5. Select win10_driver_compat.inf

Registry Settings:
  HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Win10Compatibility
  ├─ EnableWin10APIs: Enable Windows 10 API support (1=yes)
  ├─ EnableIoValidation: Validate driver I/O operations (1=yes)
  ├─ EnableFileInfoCompat: Handle new file info classes (1=yes)
  ├─ LogIncompatibleCalls: Debug logging (1=yes)
  └─ LegacyDriverMode: Compatibility for legacy drivers (1=yes)

DEBUGGING COMPATIBILITY ISSUES
------------------------------

Enable Debug Output:
  In ReactOS boot menu, add kernel parameter:
  /DEBUG /DEBUGPORT=SERIAL /BAUDRATE=115200
  (Requires serial port connection)

View Driver Compatibility Info:
  The system automatically logs driver information:
  "Driver: <name>"
  "API Version: 0x<version>"
  "  - Windows 10 compatible"
  "  - Requires IRP validation"
  etc.

Check Registry for Errors:
  HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\<DriverName>
  
  Look for entries like:
  - Start: Load order (1=boot, 2=system, 3=auto, 4=demand)
  - Status: Last load result (not 0=error)
  - ImagePath: Full path to .sys file

COMPATIBILITY WRAPPER EXAMPLE
-----------------------------

When a driver calls an API that may have Windows 10-specific parameters:

Before (unmodified):
  NTSTATUS status = ZwCreateFile(&handle, 
                                 FILE_GENERIC_READ | 0x00400000);  // Win10 flag!
  
After (with compatibility layer):
  IopValidateCreateFile() intercepts the call
  Detects FILE_OPEN_NO_RECALL flag (0x00400000)
  Strips unsupported flags
  Passes cleaned parameters to ReactOS handler
  Returns compatible status code

LIMITATIONS & NOTES
-------------------

1. Not all Windows 10 features are supported:
   • Windows 10 UWP (Universal Windows Platform) drivers not supported
   • Hardware-specific features may not work
   • Some device types may not be fully compatible

2. Newer API sets from Windows 10 may need additional shims:
   • Monitor the debug output for incompatible calls
   • Report issues to ReactOS developers

3. Legacy drivers (pre-Windows 7) get extra validation to ensure stability

4. The compatibility layer is transparent to well-behaved drivers:
   • Drivers following Windows API guidelines work unchanged
   • No modification to driver binary necessary

FUTURE IMPROVEMENTS
-------------------

Planned additions:
□ Windows 11 driver support
□ Hardware verification layer (GPU, NIC, Storage drivers)
□ Automatic driver installation from Windows 10 media
□ Driver update management
□ Performance optimization for frequently used APIs
□ PnP/Power Management enhancements

TESTING CHECKLIST
-----------------

When adding a Windows 10 driver:

[  ] Driver loads without errors
[  ] Compatible context created successfully
[  ] Debug log shows correct API version detection
[  ] Device appears in Device Manager
[  ] Attached to correct bus (PCI, USB, etc.)
[  ] Device is functional (test basic operations)
[  ] No system crashes during operation
[  ] Registry entries created correctly
[  ] Unload/reload cycle works properly

TECHNICAL DETAILS
-----------------

File Locations:
  Driver Module:      ntoskrnl/io/iomgr/compat_shims.c
  Header:             ntoskrnl/io/iomgr/compat_shims.h
  Modified:           ntoskrnl/io/iomgr/driver.c
  INF Profile:        media/inf/win10_driver_compat.inf

Function Call Stack (Normal Driver Load):
  NtLoadDriver()
    └─ IopDoLoadUnloadDriver()
        └─ IopLoadUnloadDriverWorker()
            └─ IopLoadDriver()
                └─ IopInitializeDriverModule()
                    ├─ IopInitializeCompatShim() ◄─ NEW
                    ├─ DriverEntry()
                    └─ IopLogDriverCompatibility() ◄─ NEW

Exported Functions:
  From ntoskrnl/ntoskrnl.spec (~500+ APIs)
  Compatible with Windows 10 driver specifications

REFERENCES
----------

- ReactOS Architecture: https://reactos.org/wiki/Architectural_Overview
- Windows Driver Kit (WDK) Documentation
- NTOSKRNL Kernel Implementation
- Driver Model: WDM (Windows Driver Model) Compliant

SUPPORT
-------

For issues or questions:
  1. Check debug output for specific error messages
  2. Review this documentation
  3. Test with known-working windows drivers first
  4. Report detailed information to ReactOS developers:
     - Driver name and version
     - Target Windows version
     - Error messages from debug log
     - System configuration (CPU, RAM, devices)

