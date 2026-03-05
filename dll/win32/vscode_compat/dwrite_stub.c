/*
 * PROJECT:     ReactOS DirectWrite Stub Library
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     Minimal DirectWrite stub for Electron text rendering
 * COPYRIGHT:   Copyright 2026 ReactOS Team
 *
 * DESCRIPTION:
 *   DirectWrite provides modern text rendering and font management.
 *   Electron uses this for high-quality text display.
 *   
 *   Stub returns E_NOTIMPL to force GDI/Uniscribe fallback.
 */

#define WIN32_NO_STATUS
#include <windef.h>
#include <winbase.h>

#define WINE_DEFAULT_DEBUG_CHANNEL(dwrite_stub)
#include <wine/debug.h>

WINE_DEFAULT_DEBUG_CHANNEL(dwrite_stub);

/*
 * @implemented (stub)
 * 
 * Creates a DirectWrite factory for text rendering.
 * Returns E_NOTIMPL - forces GDI/USP10 fallback.
 */
HRESULT
WINAPI
DWriteCreateFactory(
    int factoryType,
    const void *iid,
    void **factory)
{
    DPRINT1("DWriteCreateFactory() - STUB: Returning E_NOTIMPL (GDI/USP10 fallback)\n");

    if (factory)
        *factory = NULL;

    return E_NOTIMPL;
}
