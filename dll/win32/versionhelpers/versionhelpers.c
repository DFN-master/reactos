/*
 * PROJECT:     ReactOS Version Helper APIs
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     Windows 10 Version Detection APIs for Modern Application Compatibility
 * COPYRIGHT:   Copyright 2026 ReactOS Team
 *
 * DESCRIPTION:
 *   This module implements Windows 10 version detection helper functions
 *   required by modern applications like VS Code, Electron, Chromium, etc.
 *   
 *   These APIs allow applications to detect Windows version at runtime
 *   for feature gating and compatibility decisions.
 */

#define WIN32_NO_STATUS
#include <windef.h>
#include <winbase.h>
#include <winuser.h>
#include <ndk/rtlfuncs.h>

#include <wine/debug.h>

WINE_DEFAULT_DEBUG_CHANNEL(versionhelpers);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(fdwReason);
    UNREFERENCED_PARAMETER(lpvReserved);
    return TRUE;
}

/* Internal helper to check OS version */
static BOOL
IsWindowsVersionOrGreaterInternal(
    WORD wMajorVersion,
    WORD wMinorVersion,
    WORD wServicePackMajor)
{
    RTL_OSVERSIONINFOEXW osvi;
    NTSTATUS Status;

    ZeroMemory(&osvi, sizeof(RTL_OSVERSIONINFOEXW));
    osvi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);

    /* Get actual OS version */
    Status = RtlGetVersion((PRTL_OSVERSIONINFOW)&osvi);
    if (!NT_SUCCESS(Status))
    {
        ERR("RtlGetVersion failed: 0x%08lx\n", Status);
        return FALSE;
    }

    /* Compare version numbers */
    if (osvi.dwMajorVersion > wMajorVersion)
        return TRUE;
    if (osvi.dwMajorVersion < wMajorVersion)
        return FALSE;

    /* Major version matches, check minor */
    if (osvi.dwMinorVersion > wMinorVersion)
        return TRUE;
    if (osvi.dwMinorVersion < wMinorVersion)
        return FALSE;

    /* Major and minor match, check service pack */
    if (osvi.wServicePackMajor >= wServicePackMajor)
        return TRUE;

    return FALSE;
}

/*
 * @implemented
 */
BOOL
WINAPI
IsWindowsVersionOrGreater(
    WORD wMajorVersion,
    WORD wMinorVersion,
    WORD wServicePackMajor)
{
    BOOL Result = IsWindowsVersionOrGreaterInternal(wMajorVersion, wMinorVersion, wServicePackMajor);
    
        TRACE("IsWindowsVersionOrGreater(%u.%u.%u) = %s\n",
            wMajorVersion, wMinorVersion, wServicePackMajor,
            Result ? "TRUE" : "FALSE");
    
    return Result;
}

/*
 * @implemented
 */
BOOL
WINAPI
IsWindowsXPOrGreater(VOID)
{
    /* Windows XP = 5.1 */
    return IsWindowsVersionOrGreaterInternal(5, 1, 0);
}

/*
 * @implemented
 */
BOOL
WINAPI
IsWindowsXPSP1OrGreater(VOID)
{
    /* Windows XP SP1 = 5.1 SP1 */
    return IsWindowsVersionOrGreaterInternal(5, 1, 1);
}

/*
 * @implemented
 */
BOOL
WINAPI
IsWindowsXPSP2OrGreater(VOID)
{
    /* Windows XP SP2 = 5.1 SP2 */
    return IsWindowsVersionOrGreaterInternal(5, 1, 2);
}

/*
 * @implemented
 */
BOOL
WINAPI
IsWindowsXPSP3OrGreater(VOID)
{
    /* Windows XP SP3 = 5.1 SP3 */
    return IsWindowsVersionOrGreaterInternal(5, 1, 3);
}

/*
 * @implemented
 */
BOOL
WINAPI
IsWindowsVistaOrGreater(VOID)
{
    /* Windows Vista = 6.0 */
    return IsWindowsVersionOrGreaterInternal(6, 0, 0);
}

/*
 * @implemented
 */
BOOL
WINAPI
IsWindowsVistaSP1OrGreater(VOID)
{
    /* Windows Vista SP1 = 6.0 SP1 */
    return IsWindowsVersionOrGreaterInternal(6, 0, 1);
}

/*
 * @implemented
 */
BOOL
WINAPI
IsWindowsVistaSP2OrGreater(VOID)
{
    /* Windows Vista SP2 = 6.0 SP2 */
    return IsWindowsVersionOrGreaterInternal(6, 0, 2);
}

/*
 * @implemented
 */
BOOL
WINAPI
IsWindows7OrGreater(VOID)
{
    /* Windows 7 = 6.1 */
    return IsWindowsVersionOrGreaterInternal(6, 1, 0);
}

/*
 * @implemented
 */
BOOL
WINAPI
IsWindows7SP1OrGreater(VOID)
{
    /* Windows 7 SP1 = 6.1 SP1 */
    return IsWindowsVersionOrGreaterInternal(6, 1, 1);
}

/*
 * @implemented
 */
BOOL
WINAPI
IsWindows8OrGreater(VOID)
{
    /* Windows 8 = 6.2 */
    return IsWindowsVersionOrGreaterInternal(6, 2, 0);
}

/*
 * @implemented
 */
BOOL
WINAPI
IsWindows8Point1OrGreater(VOID)
{
    /* Windows 8.1 = 6.3 */
    return IsWindowsVersionOrGreaterInternal(6, 3, 0);
}

/*
 * @implemented
 */
BOOL
WINAPI
IsWindows10OrGreater(VOID)
{
    /* Windows 10 = 10.0 */
    BOOL Result = IsWindowsVersionOrGreaterInternal(10, 0, 0);
    
    TRACE("IsWindows10OrGreater() = %s\n", Result ? "TRUE" : "FALSE");
    
    return Result;
}

/*
 * @implemented
 */
BOOL
WINAPI
IsWindowsServer(VOID)
{
    RTL_OSVERSIONINFOEXW osvi;
    NTSTATUS Status;

    ZeroMemory(&osvi, sizeof(RTL_OSVERSIONINFOEXW));
    osvi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);

    Status = RtlGetVersion((PRTL_OSVERSIONINFOW)&osvi);
    if (!NT_SUCCESS(Status))
    {
        ERR("RtlGetVersion failed: 0x%08lx\n", Status);
        return FALSE;
    }

    /* Check if this is a server SKU */
    return (osvi.wProductType == VER_NT_DOMAIN_CONTROLLER ||
            osvi.wProductType == VER_NT_SERVER);
}
