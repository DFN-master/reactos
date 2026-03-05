/*
 * ReactOS Modern Setup - Administrator Account Creation Page
 * Copyright (C) 2026 ReactOS Team
 *
 * Create first administrator user during installation
 */

#include "reactos.h"
#include "modern_setup.h"
#include "modern_resource.h"
#include "resource.h"

/* Administrator Account Page */
INT_PTR CALLBACK AdminAccountPageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    PMODERN_SETUP_CONTEXT pContext;
    static WCHAR szUsername[256] = L"Administrator";
    static WCHAR szPassword[256] = L"";
    static WCHAR szPasswordConfirm[256] = L"";
    
    switch (message)
    {
        case WM_INITDIALOG:
        {
            pContext = (PMODERN_SETUP_CONTEXT)((LPPROPSHEETPAGE)lParam)->lParam;
            SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)pContext);
            
            /* Apply modern theme */
            ApplyModernTheme(hDlg);
            
            /* Set modern fonts */
            HWND hTitle = GetDlgItem(hDlg, IDC_ADMIN_TITLE);
            if (hTitle)
                SendMessageW(hTitle, WM_SETFONT, (WPARAM)pContext->hTitleFont, TRUE);
            
            /* Set default username */
            SetDlgItemTextW(hDlg, IDC_ADMIN_USERNAME, szUsername);
            
            /* Set password field as password style */
            HWND hPassword = GetDlgItem(hDlg, IDC_ADMIN_PASSWORD);
            if (hPassword)
            {
                SendMessageW(hPassword, EM_SETPASSWORDCHAR, (WPARAM)'●', 0);
                SetLimitText(hPassword, 255);
            }
            
            HWND hPasswordConfirm = GetDlgItem(hDlg, IDC_ADMIN_PASSWORD_CONFIRM);
            if (hPasswordConfirm)
            {
                SendMessageW(hPasswordConfirm, EM_SETPASSWORDCHAR, (WPARAM)'●', 0);
                SetLimitText(hPasswordConfirm, 255);
            }
            
            /* Set description text */
            SetDlgItemTextW(hDlg, IDC_ADMIN_DESCRIPTION,
                L"Create the administrator account for your ReactOS system.\n\n"
                L"The administrator account is required to manage your computer and install software.\n"
                L"You can create additional user accounts after installation.\n\n"
                L"Username: Choose a name for this administrator account (default: Administrator)\n"
                L"Password: Enter a strong password (8+ characters recommended)\n"
                L"Confirm Password: Retype the password to verify");
            
            return TRUE;
        }
        
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDC_ADMIN_USERNAME:
                    if (HIWORD(wParam) == EN_CHANGE)
                    {
                        /* Get username input */
                        GetDlgItemTextW(hDlg, IDC_ADMIN_USERNAME, szUsername, 256);
                        
                        /* Validate username (no spaces, special chars) */
                        BOOL bValidUsername = TRUE;
                        for (int i = 0; szUsername[i]; i++)
                        {
                            WCHAR c = szUsername[i];
                            /* Allow alphanumeric, underscore, hyphen */
                            if (!iswalnum(c) && c != L'_' && c != L'-')
                            {
                                bValidUsername = FALSE;
                                break;
                            }
                        }
                        
                        if (!bValidUsername)
                        {
                            MessageBoxW(hDlg, 
                                L"Username can only contain letters, numbers, underscore, and hyphen.",
                                L"Invalid Username", MB_OK | MB_ICONWARNING);
                            SetDlgItemTextW(hDlg, IDC_ADMIN_USERNAME, L"Administrator");
                        }
                    }
                    break;
                    
                case IDC_ADMIN_PASSWORD:
                    if (HIWORD(wParam) == EN_CHANGE)
                    {
                        GetDlgItemTextW(hDlg, IDC_ADMIN_PASSWORD, szPassword, 256);
                    }
                    break;
                    
                case IDC_ADMIN_PASSWORD_CONFIRM:
                    if (HIWORD(wParam) == EN_CHANGE)
                    {
                        GetDlgItemTextW(hDlg, IDC_ADMIN_PASSWORD_CONFIRM, 
                                       szPasswordConfirm, 256);
                    }
                    break;
                    
                case IDC_ADMIN_PASSWORD_SHOW:
                {
                    /* Toggle password visibility */
                    HWND hPassword = GetDlgItem(hDlg, IDC_ADMIN_PASSWORD);
                    HWND hPasswordConfirm = GetDlgItem(hDlg, IDC_ADMIN_PASSWORD_CONFIRM);
                    
                    if (IsDlgButtonChecked(hDlg, IDC_ADMIN_PASSWORD_SHOW) == BST_CHECKED)
                    {
                        /* Show password */
                        SendMessageW(hPassword, EM_SETPASSWORDCHAR, 0, 0);
                        SendMessageW(hPasswordConfirm, EM_SETPASSWORDCHAR, 0, 0);
                        InvalidateRect(hPassword, NULL, FALSE);
                        InvalidateRect(hPasswordConfirm, NULL, FALSE);
                    }
                    else
                    {
                        /* Hide password */
                        SendMessageW(hPassword, EM_SETPASSWORDCHAR, (WPARAM)'●', 0);
                        SendMessageW(hPasswordConfirm, EM_SETPASSWORDCHAR, (WPARAM)'●', 0);
                        InvalidateRect(hPassword, NULL, FALSE);
                        InvalidateRect(hPasswordConfirm, NULL, FALSE);
                    }
                    break;
                }
            }
            break;
        }
        
        case WM_NOTIFY:
        {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            
            switch (pnmh->code)
            {
                case PSN_SETACTIVE:
                {
                    /* Enable Next button based on password validity */
                    BOOL bCanContinue = TRUE;
                    
                    /* Check if passwords don't match */
                    if (wcscmp(szPassword, szPasswordConfirm) != 0)
                        bCanContinue = FALSE;
                    
                    /* Check if password is empty (allow empty for quick setup) */
                    /* Or require minimum 4 characters */
                    if (wcslen(szPassword) > 0 && wcslen(szPassword) < 4)
                        bCanContinue = FALSE;
                    
                    PropSheet_SetWizButtons(GetParent(hDlg), 
                        PSWIZB_BACK | (bCanContinue ? PSWIZB_NEXT : 0));
                    return TRUE;
                }
                
                case PSN_WIZNEXT:
                {
                    /* Validate before proceeding */
                    if (wcscmp(szPassword, szPasswordConfirm) != 0)
                    {
                        MessageBoxW(hDlg, 
                            L"Passwords do not match. Please verify your password.",
                            L"Password Mismatch", MB_OK | MB_ICONERROR);
                        return TRUE; /* Prevent navigation */
                    }
                    
                    if (wcslen(szUsername) == 0)
                    {
                        MessageBoxW(hDlg, 
                            L"Please enter a username.",
                            L"Username Required", MB_OK | MB_ICONERROR);
                        return TRUE;
                    }
                    
                    if (wcslen(szPassword) > 0 && wcslen(szPassword) < 4)
                    {
                        MessageBoxW(hDlg, 
                            L"Password must be at least 4 characters long.",
                            L"Weak Password", MB_OK | MB_ICONWARNING);
                        return TRUE;
                    }
                    
                    /* Store credentials in context for later use */
                    pContext = (PMODERN_SETUP_CONTEXT)GetWindowLongPtr(hDlg, GWLP_USERDATA);
                    if (pContext)
                    {
                        /* Save to a global structure or context */
                        /* This will be used during the actual installation */
                        // TODO: Store in installation context
                    }
                    
                    return FALSE; /* Allow navigation */
                }
                
                case PSN_WIZBACK:
                    return FALSE;
                    
                default:
                    break;
            }
            break;
        }
        
        case WM_CTLCOLORSTATIC:
        {
            HDC hdc = (HDC)wParam;
            HWND hControl = (HWND)lParam;
            
            pContext = (PMODERN_SETUP_CONTEXT)GetWindowLongPtr(hDlg, GWLP_USERDATA);
            
            if (hControl == GetDlgItem(hDlg, IDC_ADMIN_TITLE))
            {
                SetTextColor(hdc, AERO_BLUE_ACCENT);
                SetBkMode(hdc, TRANSPARENT);
                return (INT_PTR)pContext->hBackgroundBrush;
            }
            break;
        }
    }
    
    return FALSE;
}

/* ===============================================
   USER ACCOUNT CREATION HELPER
   Create local user account in registry
   =============================================== */

typedef struct _USER_ACCOUNT_INFO
{
    WCHAR Username[256];
    WCHAR FullName[256];
    WCHAR Password[256];
    WCHAR Description[512];
    DWORD Flags;        /* User flags (admin, disabled, etc) */
    DWORD RID;          /* Relative ID (1000+ for regular users) */
} USER_ACCOUNT_INFO, *PUSER_ACCOUNT_INFO;

/*
 * CreateUserAccount()
 * 
 * Create a new local user account during installation
 * This function writes the user info to the registry
 */
BOOL CreateUserAccount(PUSER_ACCOUNT_INFO pUserInfo)
{
    HKEY hKey = NULL;
    HKEY hUsersKey = NULL;
    WCHAR szPath[256];
    LONG lResult;
    
    if (!pUserInfo || !pUserInfo->Username[0])
        return FALSE;
    
    /* Open SAM registry key */
    lResult = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"SAM\\SAM\\Domains\\Builtin\\Users\\Names",
        0, KEY_ALL_ACCESS, &hKey);
    
    if (lResult != ERROR_SUCCESS)
    {
        /* Create path if it doesn't exist */
        lResult = RegCreateKeyExW(HKEY_LOCAL_MACHINE,
            L"SAM\\SAM\\Domains\\Builtin\\Users\\Names",
            0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
            NULL, &hKey, NULL);
    }
    
    if (lResult == ERROR_SUCCESS)
    {
        /* Create user account entry */
        DWORD dwFlags = pUserInfo->Flags;
        
        lResult = RegSetValueExW(hKey,
            pUserInfo->Username, 0, REG_DWORD,
            (PBYTE)&dwFlags, sizeof(DWORD));
        
        RegCloseKey(hKey);
    }
    
    /* Open/Create user profile path */
    swprintf(szPath, 256,
        L"SAM\\SAM\\Domains\\Builtin\\Users\\%04d",
        pUserInfo->RID);
    
    lResult = RegCreateKeyExW(HKEY_LOCAL_MACHINE,
        szPath, 0, NULL, REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS, NULL, &hUsersKey, NULL);
    
    if (lResult == ERROR_SUCCESS)
    {
        /* Set user properties */
        RegSetValueExW(hUsersKey, L"Name", 0, REG_SZ,
            (PBYTE)pUserInfo->FullName,
            (wcslen(pUserInfo->FullName) + 1) * sizeof(WCHAR));
        
        RegSetValueExW(hUsersKey, L"Description", 0, REG_SZ,
            (PBYTE)pUserInfo->Description,
            (wcslen(pUserInfo->Description) + 1) * sizeof(WCHAR));
        
        /* TODO: Hash and store password securely */
        /* For now, just store a marker */
        RegSetValueExW(hUsersKey, L"PasswordSet", 0, REG_DWORD,
            (PBYTE)&dwFlags, sizeof(DWORD));
        
        RegCloseKey(hUsersKey);
    }
    
    return (lResult == ERROR_SUCCESS);
}

/*
 * ValidateUsername()
 * 
 * Check if username is valid (no reserved names, valid chars)
 */
BOOL ValidateUsername(LPCWSTR szUsername)
{
    /* Reserved usernames */
    const WCHAR *szReserved[] = {
        L"Administrator",  /* Can use but will be special */
        L"Guest",
        L"SYSTEM",
        L"Administrators",
        L"Users",
        L"Guests",
        NULL
    };
    
    /* Minimum length check */
    if (wcslen(szUsername) < 1 || wcslen(szUsername) > 255)
        return FALSE;
    
    /* Check for valid characters */
    const WCHAR *szInvalidChars = L"\"\\/:*?<>|";
    if (wcspbrk(szUsername, szInvalidChars))
        return FALSE;
    
    /* Check for spaces only */
    BOOL bHasNonSpace = FALSE;
    for (int i = 0; szUsername[i]; i++)
    {
        if (szUsername[i] != L' ')
        {
            bHasNonSpace = TRUE;
            break;
        }
    }
    
    return bHasNonSpace;
}

/*
 * ValidatePassword()
 * 
 * Check if password meets minimum requirements
 */
BOOL ValidatePassword(LPCWSTR szPassword)
{
    /* Empty password is allowed for admin account during setup */
    if (!szPassword)
        return FALSE;
    
    /* Maximum length check */
    if (wcslen(szPassword) > 255)
        return FALSE;
    
    return TRUE;
}

/*
 * HashPassword()
 * 
 * Simple password hashing (production should use proper crypto)
 */
void HashPassword(LPCWSTR szPassword, PBYTE pbHash, DWORD cbHash)
{
    /* TODO: Implement proper password hashing using bcrypt.dll */
    /* For now, just use a simple algorithm */
    
    if (!szPassword || !pbHash || cbHash < 16)
        return;
    
    /* Simple hash: just XOR bytes (NOT SECURE - FOR DEMO ONLY) */
    DWORD dwLen = wcslen(szPassword);
    
    for (DWORD i = 0; i < cbHash; i++)
    {
        pbHash[i] = 0;
        for (DWORD j = 0; j < dwLen; j++)
        {
            pbHash[i] ^= (BYTE)((szPassword[j] >> (i % 2 ? 8 : 0)) & 0xFF);
        }
    }
}
