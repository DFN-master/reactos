/*
 * PROJECT:     ReactOS API tests
 * LICENSE:     LGPL-2.1-or-later (https://spdx.org/licenses/LGPL-2.1-or-later)
 * PURPOSE:     Test for GetSystemFileCacheSize
 * COPYRIGHT:   Copyright 2026 ReactOS Contributors
 */

#include <apitest.h>
#include <windows.h>

START_TEST(GetSystemFileCacheSize)
{
    SIZE_T MinSize = 0, MaxSize = 0;
    DWORD Flags = 0;
    BOOL Result;

    /* Basic call with all output parameters */
    Result = GetSystemFileCacheSize(&MinSize, &MaxSize, &Flags);
    ok(Result == TRUE, "GetSystemFileCacheSize returned %d (expected TRUE)\n", Result);
    ok(MinSize > 0 || MaxSize > 0, "Both MinSize and MaxSize are 0\n");

    /* NULL output parameters (should not crash) */
    Result = GetSystemFileCacheSize(NULL, NULL, NULL);
    ok(Result == TRUE, "GetSystemFileCacheSize with NULL params returned %d\n", Result);

    /* Partial output parameters */
    MinSize = 0;
    Result = GetSystemFileCacheSize(&MinSize, NULL, NULL);
    ok(Result == TRUE, "GetSystemFileCacheSize (MinSize only) returned %d\n", Result);
}
