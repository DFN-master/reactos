/*
 * PROJECT:     ReactOS DXGI Stub Library
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     Minimal DXGI stub for Electron/Chromium compatibility
 * COPYRIGHT:   Copyright 2026 ReactOS Team
 *
 * DESCRIPTION:
 *   DXGI (DirectX Graphics Infrastructure) provides device and swapchain
 *   management for Direct3D. Electron/Chromium query DXGI to enumerate
 *   GPU adapters and create rendering surfaces.
 *   
 *   This stub returns E_NOTIMPL to force software rendering.
 */

#define WIN32_NO_STATUS
#include <windef.h>
#include <winbase.h>
#include <winerror.h>

#define WINE_DEFAULT_DEBUG_CHANNEL(dxgi_stub)
#include <wine/debug.h>

WINE_DEFAULT_DEBUG_CHANNEL(dxgi_stub);

typedef const void *REFIID;

/*
 * @implemented (stub)
 * 
 * Creates a DXGI factory for enumerating adapters.
 * Returns E_NOTIMPL - no GPU enumeration support.
 */
HRESULT
WINAPI
CreateDXGIFactory(
    _In_ REFIID riid,
    _Out_ void **ppFactory)
{
    DPRINT1("CreateDXGIFactory() - STUB: Returning E_NOTIMPL\n");

    if (ppFactory)
        *ppFactory = NULL;

    return E_NOTIMPL;
}

/*
 * @implemented (stub)
 */
HRESULT
WINAPI
CreateDXGIFactory1(
    _In_ REFIID riid,
    _Out_ void **ppFactory)
{
    DPRINT1("CreateDXGIFactory1() - STUB: Returning E_NOTIMPL\n");

    if (ppFactory)
        *ppFactory = NULL;

    return E_NOTIMPL;
}

/*
 * @implemented (stub)
 */
HRESULT
WINAPI
CreateDXGIFactory2(
    UINT Flags,
    _In_ REFIID riid,
    _Out_ void **ppFactory)
{
    DPRINT1("CreateDXGIFactory2(Flags=0x%08X) - STUB: Returning E_NOTIMPL\n", Flags);

    if (ppFactory)
        *ppFactory = NULL;

    return E_NOTIMPL;
}
