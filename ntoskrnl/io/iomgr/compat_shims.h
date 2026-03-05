/*
 * PROJECT:         ReactOS Kernel
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            ntoskrnl/io/iomgr/compat_shims.h
 * PURPOSE:         Windows 10 Driver Compatibility Layer Header
 */

#ifndef _COMPAT_SHIMS_H_
#define _COMPAT_SHIMS_H_

/* Function declarations for compatibility shims */

typedef struct _DRIVER_COMPAT_CONTEXT DRIVER_COMPAT_CONTEXT, *PDRIVER_COMPAT_CONTEXT;

NTSTATUS
IopInitializeCompatShim(
    _In_ PDRIVER_OBJECT DriverObject);

NTSTATUS
IopValidateIrpMajorFunction(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp);

PDRIVER_COMPAT_CONTEXT
IopGetDriverCompatContext(
    _In_ PDRIVER_OBJECT DriverObject);

BOOLEAN
IopIsWindows10CompatibleDriver(
    _In_ PDRIVER_OBJECT DriverObject);

VOID
IopLogDriverCompatibility(
    _In_ PDRIVER_OBJECT DriverObject);

#endif  /* _COMPAT_SHIMS_H_ */
