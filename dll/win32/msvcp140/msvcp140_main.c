/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS System Libraries
 * FILE:            dll/win32/msvcp140/msvcp140_main.c
 * PURPOSE:         MSVC 2017+ C++ Standard Library stub
 * PROGRAMMERS:     ReactOS Development Team
 */

#include <windows.h>
#include <wine/debug.h>
WINE_DEFAULT_DEBUG_CHANNEL(msvcp140);

/* Exception throwing functions */

void CDECL _Xbad_alloc(void)
{
    ERR("bad_alloc exception thrown - out of memory\n");
    RaiseException(0xE06D7363, 0, 0, NULL); // C++ exception
}

void CDECL _Xlength_error(const char* message)
{
    ERR("length_error exception: %s\n", message);
    RaiseException(0xE06D7363, 0, 0, NULL);
}

void CDECL _Xout_of_range(const char* message)
{
    ERR("out_of_range exception: %s\n", message);
    RaiseException(0xE06D7363, 0, 0, NULL);
}

void CDECL _Xruntime_error(const char* message)
{
    ERR("runtime_error exception: %s\n", message);
    RaiseException(0xE06D7363, 0, 0, NULL);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    TRACE("(%p, %lu, %p)\n", hinstDLL, fdwReason, lpvReserved);

    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hinstDLL);
            TRACE("msvcp140.dll loaded (minimal stub implementation)\n");
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
