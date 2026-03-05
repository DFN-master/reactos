/*
 * PROJECT:     ReactOS Direct2D Stub Library
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     Minimal Direct2D stub for Electron text/graphics rendering
 * COPYRIGHT:   Copyright 2026 ReactOS Team
 *
 * DESCRIPTION:
 *   Direct2D provides GPU-accelerated 2D graphics rendering.
 *   Electron uses this for UI elements and vector graphics.
 *   
 *   Stub returns E_NOTIMPL to force GDI32 fallback.
 */

#define WIN32_NO_STATUS
#include <windef.h>
#include <winbase.h>

#define WINE_DEFAULT_DEBUG_CHANNEL(d2d1_stub)
#include <wine/debug.h>

WINE_DEFAULT_DEBUG_CHANNEL(d2d1_stub);

/*
 * @implemented (stub)
 * 
 * Creates a Direct2D factory for rendering.
 * Returns E_NOTIMPL - forces GDI fallback.
 */
HRESULT
WINAPI
D2D1CreateFactory(
    int factoryType,
    const void *pFactoryOptions,
    void **ppFactory)
{
    DPRINT1("D2D1CreateFactory() - STUB: Returning E_NOTIMPL (GDI fallback)\n");

    if (ppFactory)
        *ppFactory = NULL;

    return E_NOTIMPL;
}
