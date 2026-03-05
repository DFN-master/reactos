/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS System Libraries
 * FILE:            dll/win32/vscode_compat/gpu_stubs.c
 * PURPOSE:         GPU/Rendering API stubs for VS Code compatibility
 * PROGRAMMERS:     ReactOS Development Team
 */

#include <windows.h>
#include <wine/debug.h>

WINE_DEFAULT_DEBUG_CHANNEL(vscode_compat);

/* GPU detection/initialization stubs - all return E_NOTIMPL */

/* Direct3D 11 CreateDevice */
HRESULT WINAPI D3D11CreateDevice(
    void *pAdapter,
    DWORD DriverType,
    HMODULE Software,
    UINT Flags,
    const DWORD *pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    void **ppDevice,
    DWORD *pFeatureLevel,
    void **ppImmediateContext)
{
    TRACE("D3D11CreateDevice() stub - returning E_NOTIMPL\n");
    
    if (ppDevice)
        *ppDevice = NULL;
    if (pFeatureLevel)
        *pFeatureLevel = 0x9100; /* D3D_FEATURE_LEVEL_9_1 */
    if (ppImmediateContext)
        *ppImmediateContext = NULL;
    
    return E_NOTIMPL;
}

/* Direct3D 11 CreateDeviceAndSwapChain */
HRESULT WINAPI D3D11CreateDeviceAndSwapChain(
    void *pAdapter,
    DWORD DriverType,
    HMODULE Software,
    UINT Flags,
    const DWORD *pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    void *pSwapChainDesc,
    void **ppSwapChain,
    void **ppDevice,
    DWORD *pFeatureLevel,
    void **ppImmediateContext)
{
    TRACE("D3D11CreateDeviceAndSwapChain() stub - returning E_NOTIMPL\n");
    
    if (ppDevice)
        *ppDevice = NULL;
    if (ppSwapChain)
        *ppSwapChain = NULL;
    if (pFeatureLevel)
        *pFeatureLevel = 0x9100; /* D3D_FEATURE_LEVEL_9_1 */
    if (ppImmediateContext)
        *ppImmediateContext = NULL;
    
    return E_NOTIMPL;
}

/* DXGI CreateFactory */
HRESULT WINAPI CreateDXGIFactory(
    const GUID *riid,
    void **ppFactory)
{
    TRACE("CreateDXGIFactory() stub - returning E_NOTIMPL\n");
    
    if (ppFactory)
        *ppFactory = NULL;
    
    return E_NOTIMPL;
}

/* DXGI CreateFactory1 */
HRESULT WINAPI CreateDXGIFactory1(
    const GUID *riid,
    void **ppFactory)
{
    TRACE("CreateDXGIFactory1() stub - returning E_NOTIMPL\n");
    
    if (ppFactory)
        *ppFactory = NULL;
    
    return E_NOTIMPL;
}

/* DXGI CreateFactory2 */
HRESULT WINAPI CreateDXGIFactory2(
    UINT Flags,
    const GUID *riid,
    void **ppFactory)
{
    TRACE("CreateDXGIFactory2() stub - returning E_NOTIMPL\n");
    
    if (ppFactory)
        *ppFactory = NULL;
    
    return E_NOTIMPL;
}

/* Direct2D CreateFactory */
HRESULT WINAPI D2D1CreateFactory(
    DWORD factoryType,
    const GUID *riid,
    void *pFactoryOptions,
    void **ppIFactory)
{
    TRACE("D2D1CreateFactory() stub - returning E_NOTIMPL\n");
    
    if (ppIFactory)
        *ppIFactory = NULL;
    
    return E_NOTIMPL;
}

/* DirectWrite CreateFactory */
HRESULT WINAPI DWriteCreateFactory(
    DWORD factoryType,
    const GUID *iid,
    void **factory)
{
    TRACE("DWriteCreateFactory() stub - returning E_NOTIMPL\n");
    
    if (factory)
        *factory = NULL;
    
    return E_NOTIMPL;
}
