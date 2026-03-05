/*
 * ReactOS Modern Setup - Windows 7 Style Installer
 * Copyright (C) 2026 ReactOS Team
 *
 * Modern Aero-style Setup UI with enhanced user experience
 */

#ifndef _REACTOS_MODERN_SETUP_H_
#define _REACTOS_MODERN_SETUP_H_

#include <windows.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <dwmapi.h>

/* Modern Setup Configuration */
#define MODERN_SETUP_VERSION    L"1.0.0"
#define USE_AERO_GLASS          1  // Enable Aero Glass effects if available
#define USE_ANIMATIONS          1  // Enable smooth transitions
#define USE_SEGOE_UI            1  // Use Segoe UI font (fallback to Tahoma)
#define AUTO_DETECT_LANGUAGE    1  // Auto-detect system language

/* Modern Color Scheme (Windows 7 Aero Blue) */
#define AERO_BLUE_LIGHT         RGB(225, 238, 255)
#define AERO_BLUE_MID           RGB(180, 215, 255)
#define AERO_BLUE_DARK          RGB(100, 170, 245)
#define AERO_BLUE_ACCENT        RGB(0, 120, 215)
#define AERO_TEXT_PRIMARY       RGB(0, 0, 0)
#define AERO_TEXT_SECONDARY     RGB(80, 80, 80)
#define AERO_BORDER             RGB(130, 135, 144)

/* Modern UI Elements */
typedef struct _MODERN_PAGE_INFO
{
    LPCWSTR Title;
    LPCWSTR Subtitle;
    LPCWSTR Description;
    UINT IconResourceId;
    BOOL ShowProgressBar;
    BOOL EnableAeroGlass;
} MODERN_PAGE_INFO, *PMODERN_PAGE_INFO;

/* Animation States */
typedef enum _ANIMATION_STATE
{
    ANIM_FADE_IN,
    ANIM_FADE_OUT,
    ANIM_SLIDE_LEFT,
    ANIM_SLIDE_RIGHT,
    ANIM_NONE
} ANIMATION_STATE;

/* Modern Setup Context */
typedef struct _MODERN_SETUP_CONTEXT
{
    HWND hMainWindow;
    HFONT hTitleFont;       // 16pt Segoe UI Semibold
    HFONT hSubtitleFont;    // 12pt Segoe UI
    HFONT hNormalFont;      // 9pt Segoe UI
    HBRUSH hBackgroundBrush;
    BOOL IsAeroEnabled;
    BOOL IsHighDPI;
    INT CurrentPage;
    INT TotalPages;
    ANIMATION_STATE AnimState;
    UINT AnimationProgress; // 0-100
} MODERN_SETUP_CONTEXT, *PMODERN_SETUP_CONTEXT;

/* Function Prototypes */

// Modern UI Initialization
BOOL InitializeModernSetup(PMODERN_SETUP_CONTEXT Context);
VOID CleanupModernSetup(PMODERN_SETUP_CONTEXT Context);

// Aero Glass & DWM
BOOL EnableAeroGlassEffect(HWND hWnd);
BOOL IsAeroGlassAvailable(VOID);
VOID DrawAeroBackground(HDC hdc, RECT *pRect);

// Modern Fonts
HFONT CreateModernFont(INT PointSize, BOOL Bold, BOOL Italic);
HFONT GetSegoeuiFont(INT PointSize);

// Custom Drawing
VOID DrawModernButton(HDC hdc, RECT *pRect, LPCWSTR Text, BOOL IsHovered, BOOL IsPressed);
VOID DrawModernProgressBar(HDC hdc, RECT *pRect, INT Progress);
VOID DrawPartitionGraph(HDC hdc, RECT *pRect, PPARTENTRY Partition);

// Animations
VOID StartPageTransition(PMODERN_SETUP_CONTEXT Context, INT NewPage, ANIMATION_STATE AnimType);
VOID UpdateAnimation(PMODERN_SETUP_CONTEXT Context);

// Modern Wizard Pages
INT_PTR CALLBACK WelcomePageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ExpressInstallPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK PartitionGraphPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SummaryPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ProgressPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Wizard Page Procedures
INT_PTR CALLBACK WelcomePageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ExpressInstallPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK AdminAccountPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK PartitionGraphPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SummaryPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ProgressPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK CompletionPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Helper Functions
VOID CenterWindowOnScreen(HWND hWnd);
VOID ApplyModernTheme(HWND hWnd);
BOOL IsHighDPIAware(VOID);
VOID ScaleRectForDPI(RECT *pRect);

#endif /* _REACTOS_MODERN_SETUP_H_ */
