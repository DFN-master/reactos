/*
 * PROJECT:     ReactOS Direct3D 11 Stub Library
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     Minimal Direct3D 11 stub for Electron/Chromium compatibility
 * COPYRIGHT:   Copyright 2026 ReactOS Team
 *
 * DESCRIPTION:
 *   This module provides minimal stub implementations of Direct3D 11 APIs
 *   required by Electron/Chromium to initialize GPU rendering.
 *   
 *   Current strategy: Return E_NOTIMPL to force software rendering fallback.
 *   
 *   TODO: Implement WARP (Windows Advanced Rasterization Platform) for
 *         software-based D3D11 rendering as a future enhancement.
 */

#define WIN32_NO_STATUS
#include <windef.h>
#include <winbase.h>
#include <winerror.h>

#define WINE_DEFAULT_DEBUG_CHANNEL(d3d11_stub)
#include <wine/debug.h>

WINE_DEFAULT_DEBUG_CHANNEL(d3d11_stub);

/* Minimal local types so this module builds without full D3D11 headers. */
typedef void IDXGIAdapter;
typedef void ID3D11Device;
typedef void ID3D11DeviceContext;
typedef void IDXGISwapChain;
typedef UINT D3D_DRIVER_TYPE;
typedef UINT D3D_FEATURE_LEVEL;
typedef struct _DXGI_SWAP_CHAIN_DESC
{
    UINT Dummy;
} DXGI_SWAP_CHAIN_DESC;

#define D3D_FEATURE_LEVEL_9_1 ((D3D_FEATURE_LEVEL)0x9100)

/*
 * @implemented (stub)
 * 
 * Creates a D3D11 device and immediate context.
 * Currently returns E_NOTIMPL to force software rendering fallback.
 */
HRESULT
WINAPI
D3D11CreateDevice(
    _In_opt_ IDXGIAdapter *pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    _In_opt_ const D3D_FEATURE_LEVEL *pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    _Out_opt_ ID3D11Device **ppDevice,
    _Out_opt_ D3D_FEATURE_LEVEL *pFeatureLevel,
    _Out_opt_ ID3D11DeviceContext **ppImmediateContext)
{
    DPRINT1("D3D11CreateDevice() - STUB: Returning E_NOTIMPL (force software fallback)\n");
    DPRINT1("  DriverType=%d, Flags=0x%08X, FeatureLevels=%u, SDKVersion=%u\n",
            DriverType, Flags, FeatureLevels, SDKVersion);

    /* Return E_NOTIMPL to signal no hardware D3D11 support */
    /* Electron/Chromium will fall back to software rasterization */
    
    if (ppDevice)
        *ppDevice = NULL;
    
    if (pFeatureLevel)
        *pFeatureLevel = D3D_FEATURE_LEVEL_9_1;
    
    if (ppImmediateContext)
        *ppImmediateContext = NULL;

    return E_NOTIMPL;
}

/*
 * @implemented (stub)
 */
HRESULT
WINAPI
D3D11CreateDeviceAndSwapChain(
    _In_opt_ IDXGIAdapter *pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    _In_opt_ const D3D_FEATURE_LEVEL *pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    _In_opt_ const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
    _Out_opt_ IDXGISwapChain **ppSwapChain,
    _Out_opt_ ID3D11Device **ppDevice,
    _Out_opt_ D3D_FEATURE_LEVEL *pFeatureLevel,
    _Out_opt_ ID3D11DeviceContext **ppImmediateContext)
{
    DPRINT1("D3D11CreateDeviceAndSwapChain() - STUB: Returning E_NOTIMPL\n");

    if (ppSwapChain)
        *ppSwapChain = NULL;
    
    if (ppDevice)
        *ppDevice = NULL;
    
    if (pFeatureLevel)
        *pFeatureLevel = D3D_FEATURE_LEVEL_9_1;
    
    if (ppImmediateContext)
        *ppImmediateContext = NULL;

    return E_NOTIMPL;
}
