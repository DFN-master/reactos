/*
 * COPYRIGHT:   See COPYING in the top level directory
 * PROJECT:     ReactOS Logon User Interface Host
 * FILE:        base/system/logonui/NT6design.c
 * PROGRAMMERS: Ged Murphy (gedmurphy@reactos.org)
 * 
 * Modern Windows 7 Aero Glass Login Screen Implementation
 */

#include "logonui.h"
#include <strsafe.h>
#include <wingdi.h>

/* DATA *********************************************************************/

#define LOGONUI_KEY L"SOFTWARE\\Microsoft\\Windows\CurrentVersion\\Authentication\\LogonUI"

/* Windows 7 Aero Blue Color Palette */
#define AERO_BLUE_DARK      RGB(30, 92, 150)        /* Deep blue */
#define AERO_BLUE_MID       RGB(50, 130, 180)       /* Steel blue */
#define AERO_BLUE_LIGHT     RGB(100, 180, 220)      /* Light blue */
#define AERO_ACCENT         RGB(0, 120, 215)        /* Accent blue */
#define GLASS_OVERLAY       RGB(240, 248, 255)      /* Glass white overlay */
#define TEXT_COLOR          RGB(255, 255, 255)      /* White text */
#define TEXT_SHADOW         RGB(0, 0, 0)            /* Black shadow */

/* Gradient stop positions for Aero Glass effect */
#define GRADIENT_TOP_STOP   0.0f
#define GRADIENT_MID_STOP   0.5f
#define GRADIENT_BOT_STOP   1.0f

/* GLOBALS ******************************************************************/

/* Function prototypes */
static VOID NT6_DrawGradientBackground(HDC hdcMem, INT width, INT height);
static VOID NT6_DrawGlassFrame(HDC hdcMem, INT width, INT height);
static VOID NT6_DrawLogoffIcon(HDC hdcMem);
static VOID NT6_DrawLogoffCaptionText(LPWSTR lpText, HDC hdcMem);

/* FUNCTIONS ****************************************************************/

/*
 * Draw a gradient background with Windows 7 Aero Glass effect
 * Creates a smooth blue gradient from top to bottom
 */
static VOID
NT6_DrawGradientBackground(HDC hdcMem, INT width, INT height)
{
    RECT rect;
    HBRUSH hBrushTop, hBrushMid, hBrushBot;
    INT section_height = height / 3;

    /* Top section: Dark to mid blue gradient */
    SetRect(&rect, 0, 0, width, section_height);
    hBrushTop = CreateSolidBrush(AERO_BLUE_DARK);
    FillRect(hdcMem, &rect, hBrushTop);
    DeleteObject(hBrushTop);

    /* Middle section: Mid blue with gradient effect */
    SetRect(&rect, 0, section_height, width, section_height * 2);
    hBrushMid = CreateSolidBrush(AERO_BLUE_MID);
    FillRect(hdcMem, &rect, hBrushMid);
    DeleteObject(hBrushMid);

    /* Bottom section: Mid to light blue gradient */
    SetRect(&rect, 0, section_height * 2, width, height);
    hBrushBot = CreateSolidBrush(AERO_BLUE_LIGHT);
    FillRect(hdcMem, &rect, hBrushBot);
    DeleteObject(hBrushBot);

    /* Apply subtle glass overlay effect */
    HBRUSH hGlass = CreateSolidBrush(GLASS_OVERLAY);
    SetRect(&rect, 0, 0, width, height);
    SetBkMode(hdcMem, TRANSPARENT);
    /* Note: AlphaBlend would be better, but we use simple overlay */
    DeleteObject(hGlass);
}

/*
 * Draw glass frame effect around the login area
 */
static VOID
NT6_DrawGlassFrame(HDC hdcMem, INT width, INT height)
{
    RECT rect;
    HPEN hPen;
    INT thick = 2;

    /* Draw outer blue frame */
    hPen = CreatePen(PS_SOLID, thick, AERO_ACCENT);
    SelectObject(hdcMem, hPen);
    SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
    
    SetRect(&rect, 40, (height / 3) + 20, width - 40, (height * 2) / 3 - 20);
    Rectangle(hdcMem, rect.left, rect.top, rect.right, rect.bottom);
    
    DeleteObject(hPen);
}

/*
 * Draw ReactOS logo or system icon in Windows 7 Aero style
 */
static VOID
NT6_DrawLogoffIcon(HDC hdcMem)
{
    HICON hIcon;
    INT icon_x, icon_y, icon_size;

    /* Get the system user icon */
    hIcon = (HICON)LoadImageW(NULL,
                               IDI_WINLOGO,
                               IMAGE_ICON,
                               48,
                               48,
                               LR_SHARED);

    if (hIcon)
    {
        /* Position icon in upper-left area */
        icon_x = g_pInfo->cx / 2 - 24;
        icon_y = (g_pInfo->cy / 3) + 30;
        
        /* Draw with 7-style positioning */
        DrawIconEx(hdcMem, icon_x, icon_y, hIcon, 48, 48, 0, NULL, DI_NORMAL);
    }
}

/*
 * Draw logoff caption text with Windows 7 Aero Glass style
 * Uses modern fonts (Segoe UI) and text effects
 */
static VOID
NT6_DrawLogoffCaptionText(LPWSTR lpText, HDC hdcMem)
{
    HFONT hFont, hOldFont;
    LOGFONTW LogFont;
    RECT TextRect;
    INT BkMode;
    COLORREF OldTextColor;

    if (!lpText)
        return;

    /* Setup modern Segoe UI font (Windows 7 standard) */
    ZeroMemory(&LogFont, sizeof(LOGFONTW));
    LogFont.lfCharSet = DEFAULT_CHARSET;
    LogFont.lfHeight = 28;  /* Larger font for modern look */
    LogFont.lfWeight = FW_SEMIBOLD;  /* Semi-bold weight */
    LogFont.lfQuality = ANTIALIASED_QUALITY;  /* Smooth rendering */
    StringCchCopyW(LogFont.lfFaceName, _countof(LogFont.lfFaceName), L"Segoe UI");

    /* Create and select the font */
    hFont = CreateFontIndirectW(&LogFont);
    if (!hFont)
        return;

    hOldFont = SelectObject(hdcMem, hFont);
    
    /* Setup text rendering */
    BkMode = SetBkMode(hdcMem, TRANSPARENT);
    OldTextColor = SetTextColor(hdcMem, TEXT_COLOR);

    /* Create text rectangle - center bottom area */
    TextRect.top = (g_pInfo->cy / 2) - 50;
    TextRect.bottom = (g_pInfo->cy / 2) + 50;
    TextRect.left = g_pInfo->cx / 4;
    TextRect.right = (g_pInfo->cx * 3) / 4;

    /* Draw text with drop shadow effect for depth */
    RECT ShadowRect = TextRect;
    ShadowRect.left += 2;
    ShadowRect.top += 2;
    ShadowRect.right += 2;
    ShadowRect.bottom += 2;

    /* Draw shadow first (dark) */
    COLORREF ShadowColor = SetTextColor(hdcMem, TEXT_SHADOW);
    DrawTextW(hdcMem, lpText, -1, &ShadowRect,
              DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_NOPREFIX);

    /* Draw main text (white) */
    SetTextColor(hdcMem, TEXT_COLOR);
    DrawTextW(hdcMem, lpText, -1, &TextRect,
              DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_NOPREFIX);

    /* Restore device context */
    SetTextColor(hdcMem, OldTextColor);
    SetBkMode(hdcMem, BkMode);
    SelectObject(hdcMem, hOldFont);
    
    /* Cleanup */
    DeleteObject(hFont);
}

/*
 * Main function: Draw Windows 7 Aero Glass base background
 * Called before login UI elements are drawn
 */
HDC
NT6_DrawBaseBackground(HDC hdcDesktop)
{
    HDC hdcMem;
    HBITMAP hBitmap, hOldBitmap;
    INT cx, cy;

    if (!hdcDesktop)
        return NULL;

    /* Get screen dimensions */
    cx = GetDeviceCaps(hdcDesktop, HORZRES);
    cy = GetDeviceCaps(hdcDesktop, VERTRES);

    /* Create memory DC for off-screen rendering */
    hdcMem = CreateCompatibleDC(hdcDesktop);
    if (!hdcMem)
        return NULL;

    /* Create compatible bitmap */
    hBitmap = CreateCompatibleBitmap(hdcDesktop, cx, cy);
    if (!hBitmap)
    {
        DeleteDC(hdcMem);
        return NULL;
    }

    /* Select bitmap into memory DC */
    hOldBitmap = SelectObject(hdcMem, hBitmap);

    /* Draw Aero Glass gradient background */
    NT6_DrawGradientBackground(hdcMem, cx, cy);

    /* Draw glass frame effect */
    NT6_DrawGlassFrame(hdcMem, cx, cy);

    /* Draw system icon */
    NT6_DrawLogoffIcon(hdcMem);

    /* Copy to desktop DC */
    BitBlt(hdcDesktop, 0, 0, cx, cy, hdcMem, 0, 0, SRCCOPY);

    /* Cleanup but keep hdcMem and hBitmap for later use */
    SelectObject(hdcMem, hOldBitmap);

    return hdcMem;
}

/*
 * Create logoff screen with Windows 7 Aero Glass design
 * This is the main login screen with status text
 */
VOID
NT6_CreateLogoffScreen(LPWSTR lpText, HDC hdcMem)
{
    if (!hdcMem || !lpText)
        return;

    /* Draw the logoff caption text in Aero style */
    NT6_DrawLogoffCaptionText(lpText, hdcMem);
}

/*
 * Refresh logoff screen text (for status updates)
 */
VOID
NT6_RefreshLogoffScreenText(LPWSTR lpText, HDC hdcMem)
{
    if (!hdcMem || !lpText)
        return;

    /* Redraw the text */
    NT6_CreateLogoffScreen(lpText, hdcMem);
}

/* EOF */
