/*
 * ReactOS Modern Setup - Welcome Page (Windows 7 Style)
 * Copyright (C) 2026 ReactOS Team
 */

#include "reactos.h"
#include "modern_setup.h"
#include "resource.h"

/* Welcome Page Procedure */
INT_PTR CALLBACK WelcomePageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    PMODERN_SETUP_CONTEXT pContext;
    
    switch (message)
    {
        case WM_INITDIALOG:
        {
            /* Get context from property sheet */
            pContext = (PMODERN_SETUP_CONTEXT)((LPPROPSHEETPAGE)lParam)->lParam;
            SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)pContext);
            
            /* Apply modern theme */
            ApplyModernTheme(hDlg);
            
            /* Enable Aero Glass if available */
            if (pContext->IsAeroEnabled)
            {
                EnableAeroGlassEffect(GetParent(hDlg));
            }
            
            /* Set modern fonts */
            HWND hTitle = GetDlgItem(hDlg, IDC_WELCOME_TITLE);
            HWND hSubtitle = GetDlgItem(hDlg, IDC_WELCOME_SUBTITLE);
            
            if (hTitle)
                SendMessageW(hTitle, WM_SETFONT, (WPARAM)pContext->hTitleFont, TRUE);
            if (hSubtitle)
                SendMessageW(hSubtitle, WM_SETFONT, (WPARAM)pContext->hSubtitleFont, TRUE);
            
            return TRUE;
        }
        
        case WM_CTLCOLORSTATIC:
        {
            HDC hdc = (HDC)wParam;
            HWND hControl = (HWND)lParam;
            
            pContext = (PMODERN_SETUP_CONTEXT)GetWindowLongPtr(hDlg, GWLP_USERDATA);
            
            /* Check if this is the title or subtitle */
            if (hControl == GetDlgItem(hDlg, IDC_WELCOME_TITLE))
            {
                SetTextColor(hdc, AERO_BLUE_ACCENT);
                SetBkMode(hdc, TRANSPARENT);
                return (INT_PTR)pContext->hBackgroundBrush;
            }
            else if (hControl == GetDlgItem(hDlg, IDC_WELCOME_SUBTITLE))
            {
                SetTextColor(hdc, AERO_TEXT_PRIMARY);
                SetBkMode(hdc, TRANSPARENT);
                return (INT_PTR)pContext->hBackgroundBrush;
            }
            break;
        }
        
        case WM_ERASEBKGND:
        {
            HDC hdc = (HDC)wParam;
            RECT rc;
            
            pContext = (PMODERN_SETUP_CONTEXT)GetWindowLongPtr(hDlg, GWLP_USERDATA);
            GetClientRect(hDlg, &rc);
            
            /* Draw modern gradient background */
            DrawAeroBackground(hdc, &rc);
            
            return TRUE;
        }
        
        case WM_NOTIFY:
        {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            
            switch (pnmh->code)
            {
                case PSN_SETACTIVE:
                    /* Page is being displayed */
                    PropSheet_SetWizButtons(GetParent(hDlg), PSWIZB_NEXT);
                    return TRUE;
                    
                case PSN_WIZNEXT:
                    /* User clicked Next button */
                    return FALSE; // Allow navigation
                    
                default:
                    break;
            }
            break;
        }
    }
    
    return FALSE;
}

/* Express Install Page - Quick setup option */
INT_PTR CALLBACK ExpressInstallPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    PMODERN_SETUP_CONTEXT pContext;
    static BOOL bExpressMode = TRUE;
    
    switch (message)
    {
        case WM_INITDIALOG:
        {
            pContext = (PMODERN_SETUP_CONTEXT)((LPPROPSHEETPAGE)lParam)->lParam;
            SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)pContext);
            
            /* Set default to Express Install */
            CheckDlgButton(hDlg, IDC_INSTALL_EXPRESS, BST_CHECKED);
            
            return TRUE;
        }
        
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDC_INSTALL_EXPRESS:
                    bExpressMode = TRUE;
                    /* Enable/disable custom options */
                    EnableWindow(GetDlgItem(hDlg, IDC_CUSTOM_PARTITION), FALSE);
                    EnableWindow(GetDlgItem(hDlg, IDC_CUSTOM_LANGUAGE), FALSE);
                    break;
                    
                case IDC_INSTALL_CUSTOM:
                    bExpressMode = FALSE;
                    /* Enable custom options */
                    EnableWindow(GetDlgItem(hDlg, IDC_CUSTOM_PARTITION), TRUE);
                    EnableWindow(GetDlgItem(hDlg, IDC_CUSTOM_LANGUAGE), TRUE);
                    break;
            }
            break;
        }
        
        case WM_NOTIFY:
        {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            
            switch (pnmh->code)
            {
                case PSN_SETACTIVE:
                    PropSheet_SetWizButtons(GetParent(hDlg), PSWIZB_BACK | PSWIZB_NEXT);
                    return TRUE;
                    
                case PSN_WIZNEXT:
                    /* Save express mode setting */
                    pContext = (PMODERN_SETUP_CONTEXT)GetWindowLongPtr(hDlg, GWLP_USERDATA);
                    // Save bExpressMode to context
                    return FALSE;
                    
                default:
                    break;
            }
            break;
        }
    }
    
    return FALSE;
}

/* Modern Progress Page with animated progress bar */
INT_PTR CALLBACK ProgressPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    PMODERN_SETUP_CONTEXT pContext;
    static INT Progress = 0;
    static UINT_PTR TimerID = 0;
    
    switch (message)
    {
        case WM_INITDIALOG:
        {
            pContext = (PMODERN_SETUP_CONTEXT)((LPPROPSHEETPAGE)lParam)->lParam;
            SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)pContext);
            
            /* Start progress animation timer */
            TimerID = SetTimer(hDlg, 1, 100, NULL); // Update every 100ms
            Progress = 0;
            
            return TRUE;
        }
        
        case WM_TIMER:
        {
            /* Update progress animation */
            if (wParam == 1)
            {
                Progress += 2;
                if (Progress > 100)
                    Progress = 100;
                
                /* Redraw progress bar */
                InvalidateRect(GetDlgItem(hDlg, IDC_PROGRESS_BAR), NULL, FALSE);
                
                /* Update status text */
                WCHAR szStatus[256];
                swprintf(szStatus, 256, L"Installing ReactOS... %d%%", Progress);
                SetDlgItemTextW(hDlg, IDC_PROGRESS_STATUS, szStatus);
                
                /* Stop timer when complete */
                if (Progress >= 100)
                {
                    KillTimer(hDlg, TimerID);
                    TimerID = 0;
                }
            }
            break;
        }
        
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hDlg, &ps);
            
            /* Custom draw progress bar */
            HWND hProgressBar = GetDlgItem(hDlg, IDC_PROGRESS_BAR);
            if (hProgressBar)
            {
                RECT rcProgress;
                GetWindowRect(hProgressBar, &rcProgress);
                ScreenToClient(hDlg, (LPPOINT)&rcProgress.left);
                ScreenToClient(hDlg, (LPPOINT)&rcProgress.right);
                
                DrawModernProgressBar(hdc, &rcProgress, Progress);
            }
            
            EndPaint(hDlg, &ps);
            return TRUE;
        }
        
        case WM_DESTROY:
        {
            if (TimerID)
                KillTimer(hDlg, TimerID);
            break;
        }
        
        case WM_NOTIFY:
        {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            
            switch (pnmh->code)
            {
                case PSN_SETACTIVE:
                    /* Disable buttons during installation */
                    PropSheet_SetWizButtons(GetParent(hDlg), 0);
                    return TRUE;
                    
                default:
                    break;
            }
            break;
        }
    }
    
    return FALSE;
}
