/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS System Libraries
 * FILE:            dll/win32/vcruntime140/vcruntime140_main.c
 * PURPOSE:         MSVC 2017+ Runtime stub for VS Code compatibility
 * PROGRAMMERS:     ReactOS Development Team
 */

#include <windows.h>
#include <stdio.h>

#include <wine/debug.h>
WINE_DEFAULT_DEBUG_CHANNEL(vcruntime140);

/* Most functions are forwarded to msvcrt.dll via .spec file */

/* Stub implementations for functions that don't exist in msvcrt */

void CDECL __CppXcptFilter(LONG code, PVOID info)
{
    TRACE("(%lx, %p) - stub\n", code, info);
}

void CDECL __std_exception_copy(PVOID dst, PVOID src)
{
    TRACE("(%p, %p) - stub\n", dst, src);
    if (dst && src)
        memcpy(dst, src, sizeof(PVOID) * 3); // Simple exception object copy
}

void CDECL __std_exception_destroy(PVOID exception)
{
    TRACE("(%p) - stub\n", exception);
}

void CDECL __std_terminate(void)
{
    ERR("std::terminate() called - terminating process\n");
    ExitProcess(3);
}

int CDECL __std_type_info_compare(PVOID lhs, PVOID rhs)
{
    TRACE("(%p, %p) - stub\n", lhs, rhs);
    return (lhs == rhs) ? 0 : ((lhs < rhs) ? -1 : 1);
}

void CDECL __std_type_info_destroy_list(PVOID list)
{
    TRACE("(%p) - stub\n", list);
}

size_t CDECL __std_type_info_hash(PVOID info)
{
    TRACE("(%p) - stub\n", info);
    return (size_t)info; // Simple identity hash
}

const char* CDECL __std_type_info_name(PVOID info, PVOID buffer)
{
    TRACE("(%p, %p) - stub\n", info, buffer);
    return info ? (const char*)((ULONG_PTR*)info)[1] : NULL;
}

void CDECL __telemetry_main_invoke_trigger(PVOID trigger)
{
    TRACE("(%p) - stub (telemetry disabled)\n", trigger);
}

void CDECL __telemetry_main_return_trigger(PVOID trigger)
{
    TRACE("(%p) - stub (telemetry disabled)\n", trigger);
}

DWORD WINAPI __vcrt_GetModuleFileNameW(HMODULE module, LPWSTR filename, DWORD size)
{
    TRACE("(%p, %p, %lu)\n", module, filename, size);
    return GetModuleFileNameW(module, filename, size);
}

HMODULE WINAPI __vcrt_GetModuleHandleW(LPCWSTR module)
{
    TRACE("(%ls)\n", module);
    return GetModuleHandleW(module);
}

BOOL WINAPI __vcrt_InitializeCriticalSectionEx(CRITICAL_SECTION* cs, DWORD spin, DWORD flags)
{
    TRACE("(%p, %lu, %lx) - using InitializeCriticalSection fallback\n", cs, spin, flags);
    // ReactOS might not have InitializeCriticalSectionEx, use basic version
    InitializeCriticalSection(cs);
    if (spin)
        SetCriticalSectionSpinCount(cs, spin);
    return TRUE;
}

HMODULE WINAPI __vcrt_LoadLibraryExW(LPCWSTR filename, HANDLE file, DWORD flags)
{
    TRACE("(%ls, %p, %lx)\n", filename, file, flags);
    return LoadLibraryExW(filename, file, flags);
}

void CDECL _configure_narrow_argv(int mode)
{
    TRACE("(%d) - stub\n", mode);
}

void CDECL _configure_wide_argv(int mode)
{
    TRACE("(%d) - stub\n", mode);
}

typedef int (CDECL *_onexit_t)(void);

int CDECL _crt_at_quick_exit(_onexit_t func)
{
    TRACE("(%p) - stub\n", func);
    return 0;
}

int CDECL _crt_atexit(_onexit_t func)
{
    TRACE("(%p)\n", func);
    return atexit(func);
}

int CDECL _execute_onexit_table(PVOID table)
{
    TRACE("(%p) - stub\n", table);
    return 0;
}

char** CDECL _get_initial_narrow_environment(void)
{
    extern char **_environ;
    TRACE("stub\n");
    return _environ;
}

WCHAR** CDECL _get_initial_wide_environment(void)
{
    extern WCHAR **_wenviron;
    TRACE("stub\n");
    return _wenviron;
}

void CDECL _initialize_narrow_environment(void)
{
    TRACE("stub\n");
}

int CDECL _initialize_onexit_table(PVOID table)
{
    TRACE("(%p) - stub\n", table);
    return 0;
}

void CDECL _initialize_wide_environment(void)
{
    TRACE("stub\n");
}

int CDECL _register_onexit_function(PVOID table, _onexit_t func)
{
    TRACE("(%p, %p) - stub\n", table, func);
    return _crt_atexit(func);
}

void CDECL _register_thread_local_exe_atexit_callback(_onexit_t func)
{
    TRACE("(%p) - stub\n", func);
}

int CDECL _seh_filter_dll(LONG code, PVOID info)
{
    TRACE("(%lx, %p) - stub\n", code, info);
    return EXCEPTION_EXECUTE_HANDLER;
}

int CDECL _seh_filter_exe(LONG code, PVOID info)
{
    TRACE("(%lx, %p) - stub\n", code, info);
    return EXCEPTION_EXECUTE_HANDLER;
}

void CDECL _set_app_type(int type)
{
    TRACE("(%d) - stub\n", type);
}

typedef void (CDECL *new_handler_func)(void);

new_handler_func CDECL _set_new_handler(new_handler_func handler)
{
    TRACE("(%p) - stub\n", handler);
    return NULL;
}

typedef void (CDECL *invalid_parameter_handler_func)(const WCHAR*, const WCHAR*, const WCHAR*, unsigned int, uintptr_t);

invalid_parameter_handler_func CDECL _set_invalid_parameter_handler(invalid_parameter_handler_func handler)
{
    TRACE("(%p) - stub\n", handler);
    return NULL;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    TRACE("(%p, %lu, %p)\n", hinstDLL, fdwReason, lpvReserved);

    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hinstDLL);
            TRACE("vcruntime140.dll loaded (stub implementation)\n");
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
