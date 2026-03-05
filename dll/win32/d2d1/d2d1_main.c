#define WIN32_NO_STATUS
#include <windef.h>
#include <winbase.h>
#include <winerror.h>

#include <wine/debug.h>

WINE_DEFAULT_DEBUG_CHANNEL(d2d1);

HRESULT WINAPI D2D1CreateFactory(INT factoryType, const void *pFactoryOptions, void **ppFactory)
{
    UNREFERENCED_PARAMETER(factoryType);
    UNREFERENCED_PARAMETER(pFactoryOptions);

    TRACE("D2D1CreateFactory stub called\n");

    if (ppFactory) *ppFactory = NULL;
    return E_NOTIMPL;
}
