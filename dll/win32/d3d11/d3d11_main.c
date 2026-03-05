#define WIN32_NO_STATUS
#include <windef.h>
#include <winbase.h>
#include <winerror.h>

#include <wine/debug.h>

WINE_DEFAULT_DEBUG_CHANNEL(d3d11);

typedef void IDXGIAdapter;
typedef void ID3D11Device;
typedef void ID3D11DeviceContext;
typedef void IDXGISwapChain;
typedef UINT D3D_DRIVER_TYPE;
typedef UINT D3D_FEATURE_LEVEL;
typedef struct _DXGI_SWAP_CHAIN_DESC { UINT Dummy; } DXGI_SWAP_CHAIN_DESC;

#define D3D_FEATURE_LEVEL_9_1 ((D3D_FEATURE_LEVEL)0x9100)

HRESULT WINAPI D3D11CreateDevice(
    IDXGIAdapter *pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    const D3D_FEATURE_LEVEL *pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    ID3D11Device **ppDevice,
    D3D_FEATURE_LEVEL *pFeatureLevel,
    ID3D11DeviceContext **ppImmediateContext)
{
    UNREFERENCED_PARAMETER(pAdapter);
    UNREFERENCED_PARAMETER(DriverType);
    UNREFERENCED_PARAMETER(Software);
    UNREFERENCED_PARAMETER(Flags);
    UNREFERENCED_PARAMETER(pFeatureLevels);
    UNREFERENCED_PARAMETER(FeatureLevels);
    UNREFERENCED_PARAMETER(SDKVersion);

    TRACE("D3D11CreateDevice stub called, forcing software fallback\n");

    if (ppDevice) *ppDevice = NULL;
    if (pFeatureLevel) *pFeatureLevel = D3D_FEATURE_LEVEL_9_1;
    if (ppImmediateContext) *ppImmediateContext = NULL;

    return E_NOTIMPL;
}

HRESULT WINAPI D3D11CreateDeviceAndSwapChain(
    IDXGIAdapter *pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    const D3D_FEATURE_LEVEL *pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
    IDXGISwapChain **ppSwapChain,
    ID3D11Device **ppDevice,
    D3D_FEATURE_LEVEL *pFeatureLevel,
    ID3D11DeviceContext **ppImmediateContext)
{
    UNREFERENCED_PARAMETER(pAdapter);
    UNREFERENCED_PARAMETER(DriverType);
    UNREFERENCED_PARAMETER(Software);
    UNREFERENCED_PARAMETER(Flags);
    UNREFERENCED_PARAMETER(pFeatureLevels);
    UNREFERENCED_PARAMETER(FeatureLevels);
    UNREFERENCED_PARAMETER(SDKVersion);
    UNREFERENCED_PARAMETER(pSwapChainDesc);

    TRACE("D3D11CreateDeviceAndSwapChain stub called, forcing software fallback\n");

    if (ppSwapChain) *ppSwapChain = NULL;
    if (ppDevice) *ppDevice = NULL;
    if (pFeatureLevel) *pFeatureLevel = D3D_FEATURE_LEVEL_9_1;
    if (ppImmediateContext) *ppImmediateContext = NULL;

    return E_NOTIMPL;
}
