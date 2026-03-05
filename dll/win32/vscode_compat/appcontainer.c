/*
 * PROJECT:     ReactOS UWP/AppContainer Compatibility Layer
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     Windows 10 AppContainer/Package APIs for VS Code compatibility
 * COPYRIGHT:   Copyright 2026 ReactOS Team
 *
 * DESCRIPTION:
 *   This module provides stub implementations of UWP/AppContainer APIs
 *   required by Electron-based applications (VS Code, Teams, etc.).
 *   
 *   Most functions return ERROR_NOT_SUPPORTED or non-packaged status
 *   to allow applications to run in classic Win32 mode.
 */

#define WIN32_NO_STATUS
#include <windef.h>
#include <winbase.h>
#include <winuser.h>
#include <appmodel.h>

#define WINE_DEFAULT_DEBUG_CHANNEL(vscode_compat)
#include <wine/debug.h>

WINE_DEFAULT_DEBUG_CHANNEL(vscode_compat);

/*
 * @implemented (stub)
 * 
 * Returns ERROR_INSUFFICIENT_BUFFER to indicate this is NOT a packaged app.
 * Electron will fall back to classic Win32 behavior.
 */
LONG
WINAPI
GetCurrentPackageId(
    _Inout_ UINT32 *bufferLength,
    _Out_opt_ BYTE *buffer)
{
    DPRINT("GetCurrentPackageId(bufferLength=%p, buffer=%p) - STUB (Not packaged)\n",
           bufferLength, buffer);

    if (!bufferLength)
    {
        return ERROR_INVALID_PARAMETER;
    }

    /* Return "not packaged" error code */
    *bufferLength = 0;
    return APPMODEL_ERROR_NO_PACKAGE;
}

/*
 * @implemented (stub)
 * 
 * Returns ERROR_NOT_SUPPORTED - this process is not in an AppContainer.
 */
LONG
WINAPI
GetCurrentPackageFullName(
    _Inout_ UINT32 *packageFullNameLength,
    _Out_opt_ PWSTR packageFullName)
{
    DPRINT("GetCurrentPackageFullName() - STUB (Not packaged)\n");

    if (!packageFullNameLength)
    {
        return ERROR_INVALID_PARAMETER;
    }

    *packageFullNameLength = 0;
    return APPMODEL_ERROR_NO_PACKAGE;
}

/*
 * @implemented (stub)
 */
LONG
WINAPI
GetCurrentPackageFamilyName(
    _Inout_ UINT32 *packageFamilyNameLength,
    _Out_opt_ PWSTR packageFamilyName)
{
    DPRINT("GetCurrentPackageFamilyName() - STUB (Not packaged)\n");

    if (!packageFamilyNameLength)
    {
        return ERROR_INVALID_PARAMETER;
    }

    *packageFamilyNameLength = 0;
    return APPMODEL_ERROR_NO_PACKAGE;
}

/*
 * @implemented (stub)
 */
LONG
WINAPI
GetCurrentPackagePath(
    _Inout_ UINT32 *pathLength,
    _Out_opt_ PWSTR path)
{
    DPRINT("GetCurrentPackagePath() - STUB (Not packaged)\n");

    if (!pathLength)
    {
        return ERROR_INVALID_PARAMETER;
    }

    *pathLength = 0;
    return APPMODEL_ERROR_NO_PACKAGE;
}

/*
 * @implemented (stub)
 */
LONG
WINAPI
GetPackageId(
    _In_ HANDLE hProcess,
    _Inout_ UINT32 *bufferLength,
    _Out_opt_ BYTE *buffer)
{
    DPRINT("GetPackageId(hProcess=%p) - STUB (Not packaged)\n", hProcess);

    if (!bufferLength)
    {
        return ERROR_INVALID_PARAMETER;
    }

    *bufferLength = 0;
    return APPMODEL_ERROR_NO_PACKAGE;
}

/*
 * @implemented (stub)
 */
LONG
WINAPI
GetPackageFullName(
    _In_ HANDLE hProcess,
    _Inout_ UINT32 *packageFullNameLength,
    _Out_opt_ PWSTR packageFullName)
{
    DPRINT("GetPackageFullName(hProcess=%p) - STUB (Not packaged)\n", hProcess);

    if (!packageFullNameLength)
    {
        return ERROR_INVALID_PARAMETER;
    }

    *packageFullNameLength = 0;
    return APPMODEL_ERROR_NO_PACKAGE;
}

/*
 * @implemented (stub)
 */
LONG
WINAPI
GetPackageFamilyName(
    _In_ HANDLE hProcess,
    _Inout_ UINT32 *packageFamilyNameLength,
    _Out_opt_ PWSTR packageFamilyName)
{
    DPRINT("GetPackageFamilyName(hProcess=%p) - STUB (Not packaged)\n", hProcess);

    if (!packageFamilyNameLength)
    {
        return ERROR_INVALID_PARAMETER;
    }

    *packageFamilyNameLength = 0;
    return APPMODEL_ERROR_NO_PACKAGE;
}

/*
 * @implemented (stub)
 */
LONG
WINAPI
GetApplicationUserModelId(
    _In_ HANDLE hProcess,
    _Inout_ UINT32 *applicationUserModelIdLength,
    _Out_opt_ PWSTR applicationUserModelId)
{
    DPRINT("GetApplicationUserModelId(hProcess=%p) - STUB\n", hProcess);

    if (!applicationUserModelIdLength)
    {
        return ERROR_INVALID_PARAMETER;
    }

    *applicationUserModelIdLength = 0;
    return APPMODEL_ERROR_NO_APPLICATION;
}

/*
 * @implemented (stub)
 */
LONG
WINAPI
GetCurrentApplicationUserModelId(
    _Inout_ UINT32 *applicationUserModelIdLength,
    _Out_opt_ PWSTR applicationUserModelId)
{
    DPRINT("GetCurrentApplicationUserModelId() - STUB\n");

    if (!applicationUserModelIdLength)
    {
        return ERROR_INVALID_PARAMETER;
    }

    *applicationUserModelIdLength = 0;
    return APPMODEL_ERROR_NO_APPLICATION;
}

/*
 * @implemented (stub)
 * 
 * Returns FALSE - current process is NOT in an AppContainer sandbox.
 */
BOOL
WINAPI
IsAppContainerProcess(
    _In_ HANDLE hProcess)
{
    DPRINT("IsAppContainerProcess(hProcess=%p) - STUB (Not sandboxed)\n", hProcess);
    return FALSE;
}

/*
 * @implemented (stub)
 */
LONG
WINAPI
PackageIdFromFullName(
    _In_ PCWSTR packageFullName,
    _In_ UINT32 flags,
    _Inout_ UINT32 *bufferLength,
    _Out_opt_ BYTE *buffer)
{
    DPRINT("PackageIdFromFullName('%S') - STUB\n", packageFullName);

    if (!bufferLength)
    {
        return ERROR_INVALID_PARAMETER;
    }

    return ERROR_NOT_SUPPORTED;
}

/*
 * @implemented (stub)
 */
LONG
WINAPI
PackageFamilyNameFromFullName(
    _In_ PCWSTR packageFullName,
    _Inout_ UINT32 *packageFamilyNameLength,
    _Out_opt_ PWSTR packageFamilyName)
{
    DPRINT("PackageFamilyNameFromFullName('%S') - STUB\n", packageFullName);

    if (!packageFamilyNameLength)
    {
        return ERROR_INVALID_PARAMETER;
    }

    return ERROR_NOT_SUPPORTED;
}

/*
 * @implemented (stub)
 */
LONG
WINAPI
PackageFullNameFromId(
    _In_ const PACKAGE_ID *packageId,
    _Inout_ UINT32 *packageFullNameLength,
    _Out_opt_ PWSTR packageFullName)
{
    DPRINT("PackageFullNameFromId() - STUB\n");

    if (!packageFullNameLength)
    {
        return ERROR_INVALID_PARAMETER;
    }

    return ERROR_NOT_SUPPORTED;
}
