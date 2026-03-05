#pragma once

#include <windows.h>
#include "resource.h"

typedef struct _INFO
{
    HINSTANCE hInstance;
    INT cx;
    INT cy;

} INFO, *PINFO;

extern PINFO g_pInfo;



/* Windows Server 2003 / XP design (classic) */
HDC NT5_DrawBaseBackground(HDC hdcDesktop);
VOID NT5_CreateLogoffScreen(LPWSTR lpText, HDC hdcMem);
VOID NT5_RefreshLogoffScreenText(LPWSTR lpText, HDC hdcMem);

/* Windows 7 / Vista design (modern Aero Glass) */
HDC NT6_DrawBaseBackground(HDC hdcDesktop);
VOID NT6_CreateLogoffScreen(LPWSTR lpText, HDC hdcMem);
VOID NT6_RefreshLogoffScreenText(LPWSTR lpText, HDC hdcMem);
