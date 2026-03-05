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

// Define uuid_t as GUID for Windows
typedef GUID uuid_t;

// Generate a random UUID using Windows UuidCreate
static inline void uuid_generate(uuid_t* uuid)
{
    UuidCreate((UUID*)uuid);
}

// Note: No need for uuid_parse, uuid_unparse, etc. 
// We only need uuid_generate for isohybrid

#endif // _WIN32

#endif // _UUID_WINDOWS_H
