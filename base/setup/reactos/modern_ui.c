/*
 * ReactOS Modern Setup - Aero-style UI Implementation
 * Copyright (C) 2026 ReactOS Team
 *
 * Modern visual effects, Aero Glass, and animations
 */

#include "reactos.h"
#include "modern_setup.h"
#include <wingdi.h>

/* Global modern setup context */
static MODERN_SETUP_CONTEXT g_ModernContext = {0};

/*
 * Initialize Modern Setup UI
 */
BOOL InitializeModernSetup(PMODERN_SETUP_CONTEXT Context)
{
    Context->IsAeroEnabled = IsAeroGlassAvailable();
    Context->IsHighDPI = IsHighDPIAware();
    Context->CurrentPage = 0;
    Context->TotalPages = 7;
    Context->AnimState = ANIM_NONE;
    Context->AnimationProgress = 0;

    /* Create modern fonts */
    Context->hTitleFont = CreateModernFont(16, TRUE, FALSE);      // 16pt Bold
    Context->hSubtitleFont = CreateModernFont(12, FALSE, FALSE);  // 12pt Regular
    Context->hNormalFont = CreateModernFont(9, FALSE, FALSE);     // 9pt Regular

    /* Create gradient background brush */
    Context->hBackgroundBrush = CreateSolidBrush(AERO_BLUE_LIGHT);

    return TRUE;
}

/*
 * Cleanup Modern Setup resources
 */
VOID CleanupModernSetup(PMODERN_SETUP_CONTEXT Context)
{
    if (Context->hTitleFont)
        DeleteObject(Context->hTitleFont);
    if (Context->hSubtitleFont)
        DeleteObject(Context->hSubtitleFont);
    if (Context->hNormalFont)
        DeleteObject(Context->hNormalFont);
    if (Context->hBackgroundBrush)
        DeleteObject(Context->hBackgroundBrush);
}

/*
 * Check if Aero Glass is available
 */
BOOL IsAeroGlassAvailable(VOID)
{
#ifdef USE_AERO_GLASS
    HMODULE hDwmapi;
    typedef HRESULT (WINAPI *PFNDWMISCOMPOSITIONENABLED)(BOOL*);
    PFNDWMISCOMPOSITIONENABLED pfnDwmIsCompositionEnabled;
    BOOL bEnabled = FALSE;

    hDwmapi = LoadLibraryW(L"dwmapi.dll");
    if (hDwmapi)
    {
        pfnDwmIsCompositionEnabled = (PFNDWMISCOMPOSITIONENABLED)
            GetProcAddress(hDwmapi, "DwmIsCompositionEnabled");
        
        if (pfnDwmIsCompositionEnabled)
        {
            pfnDwmIsCompositionEnabled(&bEnabled);
        }
        
        FreeLibrary(hDwmapi);
    }
    
    return bEnabled;
#else
    return FALSE;
#endif
}

/*
 * Enable Aero Glass effect on window
 */
BOOL EnableAeroGlassEffect(HWND hWnd)
{
#ifdef USE_AERO_GLASS
    HMODULE hDwmapi;
    typedef HRESULT (WINAPI *PFNDWMEXTENDFRAMEINTOCLIENTAREA)(HWND, const MARGINS*);
    PFNDWMEXTENDFRAMEINTOCLIENTAREA pfnDwmExtendFrameIntoClientArea;
    MARGINS margins = {-1, -1, -1, -1}; // Extend glass to entire window
    
    if (!IsAeroGlassAvailable())
        return FALSE;

    hDwmapi = LoadLibraryW(L"dwmapi.dll");
    if (hDwmapi)
    {
        pfnDwmExtendFrameIntoClientArea = (PFNDWMEXTENDFRAMEINTOCLIENTAREA)
            GetProcAddress(hDwmapi, "DwmExtendFrameIntoClientArea");
        
        if (pfnDwmExtendFrameIntoClientArea)
        {
            pfnDwmExtendFrameIntoClientArea(hWnd, &margins);
            FreeLibrary(hDwmapi);
            return TRUE;
        }
        
        FreeLibrary(hDwmapi);
    }
#endif
    return FALSE;
}

/*
 * Create modern font (Segoe UI or fallback to Tahoma)
 */
HFONT CreateModernFont(INT PointSize, BOOL Bold, BOOL Italic)
{
    LOGFONTW lf = {0};
    HDC hdc = GetDC(NULL);
    
    lf.lfHeight = -MulDiv(PointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    lf.lfWeight = Bold ? FW_SEMIBOLD : FW_NORMAL;
    lf.lfItalic = Italic;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = CLEARTYPE_QUALITY;
    lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
    
#ifdef USE_SEGOE_UI
    /* Try Segoe UI first (Vista+) */
    wcscpy(lf.lfFaceName, L"Segoe UI");
#else
    /* Fallback to Tahoma (XP compatible) */
    wcscpy(lf.lfFaceName, L"Tahoma");
#endif
    
    ReleaseDC(NULL, hdc);
    return CreateFontIndirectW(&lf);
}

/*
 * Draw Aero-style gradient background
 */
VOID DrawAeroBackground(HDC hdc, RECT *pRect)
{
    TRIVERTEX vertex[2];
    GRADIENT_RECT gRect;
    
    /* Top vertex (light blue) */
    vertex[0].x = pRect->left;
    vertex[0].y = pRect->top;
    vertex[0].Red = GetRValue(AERO_BLUE_LIGHT) << 8;
    vertex[0].Green = GetGValue(AERO_BLUE_LIGHT) << 8;
    vertex[0].Blue = GetBValue(AERO_BLUE_LIGHT) << 8;
    vertex[0].Alpha = 0x0000;
    
    /* Bottom vertex (darker blue) */
    vertex[1].x = pRect->right;
    vertex[1].y = pRect->bottom;
    vertex[1].Red = GetRValue(AERO_BLUE_MID) << 8;
    vertex[1].Green = GetGValue(AERO_BLUE_MID) << 8;
    vertex[1].Blue = GetBValue(AERO_BLUE_MID) << 8;
    vertex[1].Alpha = 0x0000;
    
    gRect.UpperLeft = 0;
    gRect.LowerRight = 1;
    
    /* Draw vertical gradient */
    GradientFill(hdc, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
}

/*
 * Draw modern button with hover effect
 */
VOID DrawModernButton(HDC hdc, RECT *pRect, LPCWSTR Text, BOOL IsHovered, BOOL IsPressed)
{
    COLORREF bgColor, borderColor, textColor;
    HBRUSH hBrush, hOldBrush;
    HPEN hPen, hOldPen;
    HFONT hFont, hOldFont;
    
    /* Determine colors based on state */
    if (IsPressed)
    {
        bgColor = AERO_BLUE_DARK;
        borderColor = AERO_BLUE_ACCENT;
        textColor = RGB(255, 255, 255);
    }
    else if (IsHovered)
    {
        bgColor = AERO_BLUE_ACCENT;
        borderColor = AERO_BLUE_DARK;
        textColor = RGB(255, 255, 255);
    }
    else
    {
        bgColor = AERO_BLUE_MID;
        borderColor = AERO_BORDER;
        textColor = AERO_TEXT_PRIMARY;
    }
    
    /* Create drawing resources */
    hBrush = CreateSolidBrush(bgColor);
    hPen = CreatePen(PS_SOLID, 1, borderColor);
    hFont = CreateModernFont(9, FALSE, FALSE);
    
    /* Draw button background */
    hOldBrush = SelectObject(hdc, hBrush);
    hOldPen = SelectObject(hdc, hPen);
    RoundRect(hdc, pRect->left, pRect->top, pRect->right, pRect->bottom, 5, 5);
    
    /* Draw button text */
    hOldFont = SelectObject(hdc, hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, textColor);
    DrawTextW(hdc, Text, -1, pRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    /* Cleanup */
    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldFont);
    DeleteObject(hBrush);
    DeleteObject(hPen);
    DeleteObject(hFont);
}

/*
 * Draw modern progress bar
 */
VOID DrawModernProgressBar(HDC hdc, RECT *pRect, INT Progress)
{
    RECT fillRect = *pRect;
    HBRUSH hBackBrush, hFillBrush;
    HPEN hBorderPen, hOldPen;
    INT fillWidth;
    
    /* Calculate fill width (0-100%) */
    fillWidth = (pRect->right - pRect->left) * Progress / 100;
    fillRect.right = fillRect.left + fillWidth;
    
    /* Draw background */
    hBackBrush = CreateSolidBrush(RGB(230, 230, 230));
    FillRect(hdc, pRect, hBackBrush);
    DeleteObject(hBackBrush);
    
    /* Draw progress fill with gradient */
    TRIVERTEX vertex[2];
    GRADIENT_RECT gRect;
    
    vertex[0].x = fillRect.left;
    vertex[0].y = fillRect.top;
    vertex[0].Red = GetRValue(AERO_BLUE_ACCENT) << 8;
    vertex[0].Green = GetGValue(AERO_BLUE_ACCENT) << 8;
    vertex[0].Blue = GetBValue(AERO_BLUE_ACCENT) << 8;
    vertex[0].Alpha = 0x0000;
    
    vertex[1].x = fillRect.right;
    vertex[1].y = fillRect.bottom;
    vertex[1].Red = GetRValue(AERO_BLUE_DARK) << 8;
    vertex[1].Green = GetGValue(AERO_BLUE_DARK) << 8;
    vertex[1].Blue = GetBValue(AERO_BLUE_DARK) << 8;
    vertex[1].Alpha = 0x0000;
    
    gRect.UpperLeft = 0;
    gRect.LowerRight = 1;
    GradientFill(hdc, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
    
    /* Draw border */
    hBorderPen = CreatePen(PS_SOLID, 1, AERO_BORDER);
    hOldPen = SelectObject(hdc, hBorderPen);
    Rectangle(hdc, pRect->left, pRect->top, pRect->right, pRect->bottom);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBorderPen);
}

/*
 * Apply modern theme to window
 */
VOID ApplyModernTheme(HWND hWnd)
{
    /* Enable modern visual styles */
    SetWindowTheme(hWnd, L"Explorer", NULL);
    
    /* Enable DPI scaling */
    if (IsHighDPIAware())
    {
        /* Set DPI awareness context (Windows 10+) */
        /* For Vista/7, this is handled automatically */
    }
}

/*
 * Center window on screen
 */
VOID CenterWindowOnScreen(HWND hWnd)
{
    RECT rcWindow, rcDesktop;
    INT xPos, yPos, wWidth, wHeight, sWidth, sHeight;
    
    GetWindowRect(hWnd, &rcWindow);
    GetWindowRect(GetDesktopWindow(), &rcDesktop);
    
    wWidth = rcWindow.right - rcWindow.left;
    wHeight = rcWindow.bottom - rcWindow.top;
    sWidth = rcDesktop.right - rcDesktop.left;
    sHeight = rcDesktop.bottom - rcDesktop.top;
    
    xPos = (sWidth - wWidth) / 2;
    yPos = (sHeight - wHeight) / 2;
    
    SetWindowPos(hWnd, HWND_TOP, xPos, yPos, 0, 0, SWP_NOSIZE);
}

/*
 * Check if high DPI aware
 */
BOOL IsHighDPIAware(VOID)
{
    HDC hdc = GetDC(NULL);
    INT dpi = GetDeviceCaps(hdc, LOGPIXELSX);
    ReleaseDC(NULL, hdc);
    return (dpi > 96); // Standard DPI is 96
}
