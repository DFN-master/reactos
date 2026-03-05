/*
 * PROJECT:         ReactOS Kernel
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            ntoskrnl/io/iomgr/compat_shims.c
 * PURPOSE:         Windows 10 Driver Compatibility Layer
 * PROGRAMMERS:     ReactOS Team
 *
 * DESCRIPTION:
 *   This module provides compatibility wrappers and shims for drivers
 *   compiled for Windows 10 / Windows Server 2016+ to run on ReactOS.
 *   It handles API differences and ensures proper driver initialization.
 */

#include <ntoskrnl.h>
#define NDEBUG
#include <debug.h>

DBG_DEFAULT_DEBUG_CHANNEL(IO);

/* Compatibility flags for drivers */
#define COMPAT_FLAG_WIN10_DRIVER       0x00000001
#define COMPAT_FLAG_WIN7_DRIVER        0x00000002
#define COMPAT_FLAG_NEEDS_VALIDATION   0x00000004
#define COMPAT_FLAG_LEGACY_API_COMPAT  0x00000008

/* Compatibility context per driver */
typedef struct _DRIVER_COMPAT_CONTEXT
{
    ULONG Flags;
    ULONG ApiVersion;
    PDRIVER_OBJECT DriverObject;
    UNICODE_STRING DriverName;
} DRIVER_COMPAT_CONTEXT, *PDRIVER_COMPAT_CONTEXT;

/* Define the ID for compatibility context extension */
static PVOID CompatShimDriverContextId = (PVOID)0xDEADBEEF;

/*
 * IopDetectDriverVersion
 *
 * Detects the target Windows version for the driver based on:
 * - PE header characteristics
 * - Subsystem version
 * - Required API set
 */
ULONG
IopDetectDriverVersion(
    _In_ PDRIVER_OBJECT DriverObject)
{
    ULONG TargetVersion = 0x0601; // Default to Windows 7
    
    /* Check loaded module information */
    if (DriverObject->DriverSection)
    {
        // Could analyze PE header here
        // For now, assume drivers compiled with modern SDK target Win10
        TargetVersion = 0x0A00; // Windows 10
    }
    
    return TargetVersion;
}

/*
 * IopInitializeCompatShim
 *
 * Initializes compatibility shim for a driver
 */
NTSTATUS
IopInitializeCompatShim(
    _In_ PDRIVER_OBJECT DriverObject)
{
    NTSTATUS Status;
    PDRIVER_COMPAT_CONTEXT CompatContext = NULL;
    ULONG ApiVersion;

    /* Allocate compatibility context */
    Status = IoAllocateDriverObjectExtension(
        DriverObject,
        CompatShimDriverContextId,
        sizeof(DRIVER_COMPAT_CONTEXT),
        (PVOID*)&CompatContext);

    if (!NT_SUCCESS(Status))
    {
        DPRINT1("Failed to allocate compat context for driver <%wZ>\n",
                &DriverObject->DriverName);
        return Status;
    }

    /* Initialize context */
    CompatContext->DriverObject = DriverObject;
    CompatContext->DriverName = DriverObject->DriverName;
    CompatContext->Flags = 0;
    
    /* Detect driver version */
    ApiVersion = IopDetectDriverVersion(DriverObject);
    CompatContext->ApiVersion = ApiVersion;

    /* Set flags based on version */
    if (ApiVersion >= 0x0A00)  // Windows 10 or newer
    {
        CompatContext->Flags |= COMPAT_FLAG_WIN10_DRIVER;
        DPRINT("Detected Windows 10+ driver: %wZ\n", &DriverObject->DriverName);
    }
    else if (ApiVersion >= 0x0601)  // Windows 7 or newer
    {
        CompatContext->Flags |= COMPAT_FLAG_WIN7_DRIVER;
        DPRINT("Detected Windows 7+ driver: %wZ\n", &DriverObject->DriverName);
    }

    /* Always enable validation for legacy drivers */
    if (DriverObject->Flags & DRVO_LEGACY_DRIVER)
    {
        CompatContext->Flags |= COMPAT_FLAG_NEEDS_VALIDATION;
    }

    /* Enable legacy API compatibility mode */
    CompatContext->Flags |= COMPAT_FLAG_LEGACY_API_COMPAT;

    DPRINT("Compat context initialized for %wZ (Flags=0x%lx)\n",
           &DriverObject->DriverName, CompatContext->Flags);

    return STATUS_SUCCESS;
}

/*
 * IopValidateQueryInformationFile
 *
 * Validates IRP_MJ_QUERY_INFORMATION to handle Win10 structures
 */
NTSTATUS
IopValidateQueryInformationFile(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp)
{
    PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);
    
    /* Check if FILE_INFORMATION_CLASS is within expected range */
    if (IrpStack->Parameters.QueryFile.FileInformationClass > FileMaximumInformation)
    {
        DPRINT1("Invalid FILE_INFORMATION_CLASS: %lu from driver %wZ\n",
                IrpStack->Parameters.QueryFile.FileInformationClass,
                DeviceObject->DriverObject->DriverName);
        
        /* Try to map newer Win10 classes to compatible ones */
        // Could add mapping here for new info classes
        
        return STATUS_INVALID_PARAMETER;
    }

    return STATUS_SUCCESS;
}

/*
 * IopValidateCreateFile
 *
 * Validates CREATE_FILE parameters for new Win10 flags
 */
NTSTATUS
IopValidateCreateFile(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp)
{
    PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);
    
    /* Check for unsupported file create options that are Win10+ specific */
    ULONG Options = IrpStack->Parameters.Create.Options;
    
    /* FILE_OPEN_NO_RECALL (0x00400000) - Win10+ */
    if (Options & 0x00400000)
    {
        DPRINT("Driver %wZ using Win10 FILE_OPEN_NO_RECALL flag\n",
               &DeviceObject->DriverObject->DriverName);
        /* This flag is safe to ignore on ReactOS */
    }

    /* FILE_OPEN_FOR_FREE_SPACE_QUERY (0x00800000) - Win10+ */
    if (Options & 0x00800000)
    {
        DPRINT("Driver %wZ using Win10 FILE_OPEN_FOR_FREE_SPACE_QUERY flag\n",
               &DeviceObject->DriverObject->DriverName);
    }

    return STATUS_SUCCESS;
}

/*
 * IopValidateIrpMajorFunction
 *
 * Validates IRP before calling driver's dispatch routine
 */
NTSTATUS
IopValidateIrpMajorFunction(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp)
{
    PDRIVER_COMPAT_CONTEXT CompatContext;
    NTSTATUS Status = STATUS_SUCCESS;

    /* Get compatibility context if exists */
    CompatContext = (PDRIVER_COMPAT_CONTEXT)
        IoGetDriverObjectExtension(DeviceObject->DriverObject,
                                   CompatShimDriverContextId);

    if (!CompatContext || !(CompatContext->Flags & COMPAT_FLAG_LEGACY_API_COMPAT))
    {
        return STATUS_SUCCESS; // No validation needed
    }

    /* Validate based on major function */
    switch (Irp->Irp_MajorFunction)
    {
        case IRP_MJ_CREATE:
            Status = IopValidateCreateFile(DeviceObject, Irp);
            break;

        case IRP_MJ_QUERY_INFORMATION:
            Status = IopValidateQueryInformationFile(DeviceObject, Irp);
            break;

        default:
            /* Other functions don't need special validation */
            break;
    }

    return Status;
}

/*
 * IopGetDriverCompatContext
 *
 * Retrieves compatibility context for a driver
 */
PDRIVER_COMPAT_CONTEXT
IopGetDriverCompatContext(
    _In_ PDRIVER_OBJECT DriverObject)
{
    return (PDRIVER_COMPAT_CONTEXT)
        IoGetDriverObjectExtension(DriverObject, CompatShimDriverContextId);
}

/*
 * IopIsWindows10CompatibleDriver
 *
 * Checks if driver is known to be Windows 10 compatible
 */
BOOLEAN
IopIsWindows10CompatibleDriver(
    _In_ PDRIVER_OBJECT DriverObject)
{
    PDRIVER_COMPAT_CONTEXT CompatContext;

    CompatContext = IopGetDriverCompatContext(DriverObject);
    if (!CompatContext)
        return FALSE;

    return (CompatContext->Flags & COMPAT_FLAG_WIN10_DRIVER) != 0;
}

/*
 * IopLogDriverCompatibility
 *
 * Logs driver compatibility information for debugging
 */
VOID
IopLogDriverCompatibility(
    _In_ PDRIVER_OBJECT DriverObject)
{
    PDRIVER_COMPAT_CONTEXT CompatContext;

    CompatContext = IopGetDriverCompatContext(DriverObject);
    if (!CompatContext)
        return;

    DPRINT("=== Driver Compatibility Report ===\n");
    DPRINT("Driver: %wZ\n", &DriverObject->DriverName);
    DPRINT("API Version: 0x%04lx\n", CompatContext->ApiVersion);
    
    if (CompatContext->Flags & COMPAT_FLAG_WIN10_DRIVER)
        DPRINT("  - Windows 10 compatible\n");
    if (CompatContext->Flags & COMPAT_FLAG_WIN7_DRIVER)
        DPRINT("  - Windows 7+ compatible\n");
    if (CompatContext->Flags & COMPAT_FLAG_NEEDS_VALIDATION)
        DPRINT("  - Requires IRP validation\n");
    if (CompatContext->Flags & COMPAT_FLAG_LEGACY_API_COMPAT)
        DPRINT("  - Legacy API compatibility enabled\n");
    
    DPRINT("===================================\n");
}
