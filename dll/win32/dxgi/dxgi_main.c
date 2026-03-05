#define WIN32_NO_STATUS
#include <windef.h>
#include <winbase.h>
#include <winerror.h>

#include <wine/debug.h>

WINE_DEFAULT_DEBUG_CHANNEL(dxgi);

HRESULT WINAPI CreateDXGIFactory(const GUID *riid, void **ppFactory)
{
    UNREFERENCED_PARAMETER(riid);
    TRACE("CreateDXGIFactory stub called\n");
    if (ppFactory) *ppFactory = NULL;
    return E_NOTIMPL;
}

HRESULT WINAPI CreateDXGIFactory1(const GUID *riid, void **ppFactory)
{
    UNREFERENCED_PARAMETER(riid);
    TRACE("CreateDXGIFactory1 stub called\n");
    if (ppFactory) *ppFactory = NULL;
    return E_NOTIMPL;
}

HRESULT WINAPI CreateDXGIFactory2(UINT Flags, const GUID *riid, void **ppFactory)
{
    UNREFERENCED_PARAMETER(Flags);
    UNREFERENCED_PARAMETER(riid);
    TRACE("CreateDXGIFactory2 stub called\n");
    if (ppFactory) *ppFactory = NULL;
    return E_NOTIMPL;
}
