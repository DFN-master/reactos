/**
 * UUID wrapper for Windows (ReactOS isohybrid EFI support)
 * 
 * This file provides Windows-compatible UUID generation
 * using native RPC runtime (rpcrt4.dll) instead of libuuid
 */

#ifndef _UUID_WINDOWS_H
#define _UUID_WINDOWS_H

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <rpc.h>

// Don't define uuid_t as typedef - leads to conflicts with Windows types
// Instead, let the code use unsigned char arrays directly

// Generate a random UUID using Windows UuidCreate  
static inline void uuid_generate(unsigned char *uuid)
{
    UuidCreate((GUID*)uuid);
}

static inline void uuid_clear(unsigned char *uuid)
{
    memset(uuid, 0, 16);
}

#endif // _WIN32

#endif // _UUID_WINDOWS_H
