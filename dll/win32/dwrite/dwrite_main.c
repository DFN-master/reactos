#define WIN32_NO_STATUS
#include <windef.h>
#include <winbase.h>
#include <winerror.h>

#include <wine/debug.h>

WINE_DEFAULT_DEBUG_CHANNEL(dwrite);

HRESULT WINAPI DWriteCreateFactory(INT factoryType, const void *iid, void **factory)
{
    UNREFERENCED_PARAMETER(factoryType);
    UNREFERENCED_PARAMETER(iid);

    TRACE("DWriteCreateFactory stub called\n");

    if (factory) *factory = NULL;
    return E_NOTIMPL;
}
