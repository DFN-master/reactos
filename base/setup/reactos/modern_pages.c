/*
 * ReactOS Modern Setup - Welcome Page (Windows 7 Style)
 * Copyright (C) 2026 ReactOS Team
 */

#include "reactos.h"
#include "modern_setup.h"
#include "modern_resource.h"
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

/* ===============================================
   PARTITION GRAPH PAGE
   Visual disk/partition selector
   =============================================== */
INT_PTR CALLBACK PartitionGraphPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    PMODERN_SETUP_CONTEXT pContext;
    
    switch (message)
    {
        case WM_INITDIALOG:
        {
            pContext = (PMODERN_SETUP_CONTEXT)((LPPROPSHEETPAGE)lParam)->lParam;
            SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)pContext);
            
            /* Apply modern theme */
            ApplyModernTheme(hDlg);
            
            /* Initialize partition list */
            HWND hListView = GetDlgItem(hDlg, IDC_PARTITION_LIST);
            if (hListView)
            {
                /* Set extended style with modern visual effects */
                ListView_SetExtendedListViewStyle(hListView, 
                    LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
                
                /* Add columns */
                LVCOLUMNW lvc;
                lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
                
                lvc.iSubItem = 0;
                lvc.pszText = L"Partition";
                lvc.cx = 120;
                ListView_InsertColumn(hListView, 0, &lvc);
                
                lvc.iSubItem = 1;
                lvc.pszText = L"Type";
                lvc.cx = 100;
                ListView_InsertColumn(hListView, 1, &lvc);
                
                lvc.iSubItem = 2;
                lvc.pszText = L"Size";
                lvc.cx = 80;
                ListView_InsertColumn(hListView, 2, &lvc);
                
                lvc.iSubItem = 3;
                lvc.pszText = L"Free Space";
                lvc.cx = 80;
                ListView_InsertColumn(hListView, 3, &lvc);
            }
            
            return TRUE;
        }
        
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hDlg, &ps);
            
            /* Draw custom partition graph */
            HWND hGraph = GetDlgItem(hDlg, IDC_PARTITION_GRAPH);
            if (hGraph)
            {
                RECT rcGraph;
                GetWindowRect(hGraph, &rcGraph);
                ScreenToClient(hDlg, (LPPOINT)&rcGraph.left);
                ScreenToClient(hDlg, (LPPOINT)&rcGraph.right);
                
                /* Draw partition visualization */
                DrawPartitionGraph(hdc, &rcGraph);
            }
            
            EndPaint(hDlg, &ps);
            return TRUE;
        }
        
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDC_PARTITION_CREATE:
                    /* Show create partition dialog */
                    MessageBoxW(hDlg, L"Create partition feature coming soon", 
                               L"ReactOS Setup", MB_OK | MB_ICONINFORMATION);
                    break;
                    
                case IDC_PARTITION_DELETE:
                    /* Delete selected partition */
                    MessageBoxW(hDlg, L"Delete partition feature coming soon", 
                               L"ReactOS Setup", MB_OK | MB_ICONINFORMATION);
                    break;
                    
                case IDC_PARTITION_FORMAT:
                    /* Format selected partition */
                    MessageBoxW(hDlg, L"Format partition feature coming soon", 
                               L"ReactOS Setup", MB_OK | MB_ICONINFORMATION);
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
                    
                default:
                    break;
            }
            break;
        }
    }
    
    return FALSE;
}

/* ===============================================
   SUMMARY PAGE
   Review installation settings before proceeding
   =============================================== */
INT_PTR CALLBACK SummaryPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    PMODERN_SETUP_CONTEXT pContext;
    
    switch (message)
    {
        case WM_INITDIALOG:
        {
            pContext = (PMODERN_SETUP_CONTEXT)((LPPROPSHEETPAGE)lParam)->lParam;
            SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)pContext);
            
            /* Build summary text */
            WCHAR szSummary[1024];
            swprintf(szSummary, 1024,
                L"Installation Type:\n"
                L"  • Express Install (Recommended Settings)\n\n"
                L"Destination:\n"
                L"  • Drive: C:\\ (Disk 0, Partition 1)\n"
                L"  • File System: NTFS\n"
                L"  • Partition Size: 15 GB\n\n"
                L"Language & Region:\n"
                L"  • Language: English (United States)\n"
                L"  • Keyboard: US\n\n"
                L"Components:\n"
                L"  • ReactOS Core System\n"
                L"  • Essential Drivers\n"
                L"  • Basic Applications\n"
                L"  • Modern DLL Package (22 proprietary DLLs)\n\n"
                L"Boot Configuration:\n"
                L"  • Boot Mode: UEFI with GPT\n"
                L"  • Install Boot Loader: Yes");
            
            SetDlgItemTextW(hDlg, IDC_SUMMARY_DETAILS, szSummary);
            
            /* Set installation size and time estimate */
            SetDlgItemTextW(hDlg, IDC_SUMMARY_INSTALL_SIZE, L"Installation size: 2.5 GB");
            SetDlgItemTextW(hDlg, IDC_SUMMARY_TIME_EST, L"Estimated time: 15-30 minutes");
            
            return TRUE;
        }
        
        case WM_NOTIFY:
        {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            
            switch (pnmh->code)
            {
                case PSN_SETACTIVE:
                    /* Change Next button to "Install" */
                    PropSheet_SetWizButtons(GetParent(hDlg), PSWIZB_BACK | PSWIZB_NEXT);
                    return TRUE;
                    
                case PSN_WIZNEXT:
                    /* User clicked Install - proceed to installation */
                    return FALSE;
                    
                default:
                    break;
            }
            break;
        }
    }
    
    return FALSE;
}

/* ===============================================
   COMPLETION PAGE
   Installation success/failure screen
   =============================================== */
INT_PTR CALLBACK CompletionPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    PMODERN_SETUP_CONTEXT pContext;
    static UINT_PTR RebootTimerID = 0;
    static INT RebootCountdown = 10;
    
    switch (message)
    {
        case WM_INITDIALOG:
        {
            pContext = (PMODERN_SETUP_CONTEXT)((LPPROPSHEETPAGE)lParam)->lParam;
            SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)pContext);
            
            /* Apply modern fonts */
            HWND hTitle = GetDlgItem(hDlg, IDC_COMPLETE_TITLE);
            if (hTitle)
                SendMessageW(hTitle, WM_SETFONT, (WPARAM)pContext->hTitleFont, TRUE);
            
            /* Set completion message */
            SetDlgItemTextW(hDlg, IDC_COMPLETE_MESSAGE,
                L"ReactOS has been successfully installed on your computer.\n\n"
                L"The computer needs to restart to complete the installation. "
                L"When you restart, you will see the ReactOS welcome screen.\n\n"
                L"Please remove any installation media (CD/DVD/USB) before restarting.");
            
            /* Check auto-reboot by default */
            CheckDlgButton(hDlg, IDC_COMPLETE_REBOOT, BST_CHECKED);
            
            /* Start reboot countdown timer */
            RebootTimerID = SetTimer(hDlg, 2, 1000, NULL); // 1 second interval
            RebootCountdown = 10;
            
            return TRUE;
        }
        
        case WM_TIMER:
        {
            if (wParam == 2 && IsDlgButtonChecked(hDlg, IDC_COMPLETE_REBOOT) == BST_CHECKED)
            {
                RebootCountdown--;
                
                WCHAR szText[256];
                swprintf(szText, 256, 
                    L"Restart computer automatically in %d seconds", RebootCountdown);
                SetDlgItemTextW(hDlg, IDC_COMPLETE_REBOOT, szText);
                
                if (RebootCountdown <= 0)
                {
                    KillTimer(hDlg, RebootTimerID);
                    RebootTimerID = 0;
                    
                    /* Initiate system restart */
                    // ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_INSTALLATION);
                }
            }
            break;
        }
        
        case WM_COMMAND:
        {
            if (LOWORD(wParam) == IDC_COMPLETE_REBOOT)
            {
                /* User toggled auto-reboot */
                if (IsDlgButtonChecked(hDlg, IDC_COMPLETE_REBOOT) == BST_UNCHECKED)
                {
                    /* Stopped countdown */
                    if (RebootTimerID)
                    {
                        KillTimer(hDlg, RebootTimerID);
                        RebootTimerID = 0;
                    }
                    SetDlgItemTextW(hDlg, IDC_COMPLETE_REBOOT, 
                        L"Restart computer automatically in 10 seconds");
                }
                else
                {
                    /* Restarted countdown */
                    RebootCountdown = 10;
                    RebootTimerID = SetTimer(hDlg, 2, 1000, NULL);
                }
            }
            break;
        }
        
        case WM_DESTROY:
        {
            if (RebootTimerID)
                KillTimer(hDlg, RebootTimerID);
            break;
        }
        
        case WM_NOTIFY:
        {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            
            switch (pnmh->code)
            {
                case PSN_SETACTIVE:
                    /* Show only Finish button */
                    PropSheet_SetWizButtons(GetParent(hDlg), PSWIZB_FINISH);
                    return TRUE;
                    
                case PSN_WIZFINISH:
                    /* User clicked Finish */
                    // Initiate shutdown or reboot based on checkbox
                    return FALSE;
                    
                default:
                    break;
            }
            break;
        }
    }
    
    return FALSE;
}
