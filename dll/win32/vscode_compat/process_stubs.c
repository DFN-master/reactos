/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS System Libraries
 * FILE:            dll/win32/vscode_compat/process_stubs.c
 * PURPOSE:         Process/compatibility API stubs for VS Code
 * PROGRAMMERS:     ReactOS Development Team
 */

#include <windows.h>
#include <ntstatus.h>
#include <wine/debug.h>

WINE_DEFAULT_DEBUG_CHANNEL(vscode_compat);

/* Forward declarations for types that may not be defined */
#ifndef PROCESS_MITIGATION_POLICY
typedef DWORD PROCESS_MITIGATION_POLICY;
#endif

#ifndef THREAD_INFORMATION_CLASS  
typedef DWORD THREAD_INFORMATION_CLASS;
#endif

/* Process/Security APIs */

/* SetProcessMitigationPolicy - Security hardening for modern apps */
BOOL WINAPI SetProcessMitigationPolicy(
    DWORD MitigationPolicy,
    PVOID lpBuffer,
    SIZE_T dwLength)
{
    TRACE("SetProcessMitigationPolicy stub - returning TRUE\n");
    
    return TRUE;
}

/* GetProcessMitigationPolicy - Query mitigation settings */
BOOL WINAPI GetProcessMitigationPolicy(
    HANDLE hProcess,
    DWORD MitigationPolicy,
    PVOID lpBuffer,
    SIZE_T dwLength)
{
    TRACE("GetProcessMitigationPolicy stub\n");
    
    if (lpBuffer && dwLength)
        ZeroMemory(lpBuffer, dwLength);
    
    return TRUE;
}

/* GetProcessPreferredUILanguages - Electron/V8 language detection */
BOOL WINAPI GetProcessPreferredUILanguages(
    DWORD dwFlags,
    PULONG pulNumLanguages,
    PWSTR pwszLanguagesBuffer,
    PULONG pcchLanguagesBuffer)
{
    static const WCHAR defaultLang[] = L"en-US\0";
    ULONG bufSize = sizeof(defaultLang);
    
    TRACE("GetProcessPreferredUILanguages stub - returning en-US\n");
    
    if (pulNumLanguages)
        *pulNumLanguages = 1;
    
    if (pcchLanguagesBuffer)
    {
        if (*pcchLanguagesBuffer < bufSize)
        {
            *pcchLanguagesBuffer = bufSize;
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        *pcchLanguagesBuffer = bufSize;
    }
    
    if (pwszLanguagesBuffer)
        CopyMemory(pwszLanguagesBuffer, defaultLang, bufSize);
    
    return TRUE;
}

/* GetTickCount64 - High-resolution timer for profiling */
ULONGLONG WINAPI GetTickCount64(void)
{
    LARGE_INTEGER counter, frequency;
    
    TRACE("GetTickCount64 stub - returning high-res tick count\n");
    
    /* Try to use QueryPerformanceCounter for better resolution */
    if (QueryPerformanceFrequency(&frequency) && 
        frequency.QuadPart > 0 &&
        QueryPerformanceCounter(&counter))
    {
        return (counter.QuadPart * 1000) / frequency.QuadPart;
    }
    
    /* Fallback: Convert GetTickCount to 64-bit */
    return (ULONGLONG)GetTickCount();
}

/* SetProcessDEPPolicy - DEP (Data Execution Prevention) for V8 JIT */
BOOL WINAPI SetProcessDEPPolicy(DWORD dwFlags)
{
    TRACE("SetProcessDEPPolicy stub - V8 JIT DEP handling\n");
    return TRUE;
}

/* GetSystemTimePreciseAsFileTime - High-res system time */
void WINAPI GetSystemTimePreciseAsFileTime(LPFILETIME lpSystemTimeAsFileTime)
{
    TRACE("GetSystemTimePreciseAsFileTime stub - returning precise system time\n");
    
    /* Use standard GetSystemTimeAsFileTime - good enough for most cases */
    GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
}

/* NtSetInformationProcess - Direct kernel API for process configuration */
NTSTATUS WINAPI NtSetInformationProcess(
    HANDLE ProcessHandle,
    DWORD ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength)
{
    TRACE("NtSetInformationProcess stub\n");
    
    return STATUS_SUCCESS;
}

/* SetThreadInformation - Thread configuration (Electron threading) */
BOOL WINAPI SetThreadInformation(
    HANDLE hThread,
    DWORD ThreadInfoClass,
    LPVOID ThreadInformation,
    DWORD ThreadInformationSize)
{
    TRACE("SetThreadInformation stub\n");
    
    return TRUE;
}

/* GetThreadInformation - Query thread state */
BOOL WINAPI GetThreadInformation(
    HANDLE hThread,
    DWORD ThreadInfoClass,
    LPVOID ThreadInformation,
    DWORD ThreadInformationSize)
{
    TRACE("GetThreadInformation stub\n");
    
    if (ThreadInformation && ThreadInformationSize)
        ZeroMemory(ThreadInformation, ThreadInformationSize);
    
    return TRUE;
}
