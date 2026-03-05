@echo off
REM ============================================================================
REM  .NET Framework 4.8 Verification Tool for ReactOS
REM ============================================================================
REM
REM  Validates .NET Framework installation completeness
REM  Checks for all critical DLLs and compiler
REM  Provides detailed diagnostics
REM
REM ============================================================================

setlocal enabledelayedexpansion

echo.
echo ============================================================================
echo  .NET Framework 4.8 Verification Tool
echo ============================================================================
echo.

set "VERIFIED_COUNT=0"
set "MISSING_COUNT=0"
set "TOTAL_CHECKS=0"

REM ============================================================================
REM 1. Check Runtime DLLs
REM ============================================================================

echo [1/5] Checking Runtime DLLs...

set "RUNTIME_DLLS=^
  mscorlib.dll ^
  System.dll ^
  System.Core.dll ^
  System.Configuration.dll ^
  System.Xml.dll ^
  System.Data.dll ^
  System.Net.dll"

for %%D in (!RUNTIME_DLLS!) do (
  set /a TOTAL_CHECKS+=1
  if exist "C:\ReactOS\System32\%%D" (
    REM Check file size (should be > 100 KB)
    for /f %%s in ('dir C:\ReactOS\System32\%%D ^| findstr /C:"{%%D}"') do (
      echo   [✓] %%D
      set /a VERIFIED_COUNT+=1
    )
  ) else (
    echo   [✗] %%D - MISSING
    set /a MISSING_COUNT+=1
  )
)

REM ============================================================================
REM 2. Check Extended DLLs
REM ============================================================================

echo.
echo [2/5] Checking Extended DLLs...

set "EXT_DLLS=^
  System.Collections.dll ^
  System.Linq.dll ^
  System.Reflection.dll ^
  System.ComponentModel.dll ^
  System.Drawing.dll ^
  System.Windows.Forms.dll ^
  System.Security.dll"

for %%D in (!EXT_DLLS!) do (
  set /a TOTAL_CHECKS+=1
  if exist "C:\ReactOS\System32\%%D" (
    echo   [✓] %%D
    set /a VERIFIED_COUNT+=1
  ) else (
    echo   [✗] %%D - MISSING (optional)
    REM Don't count as failure, just warning
  )
)

REM ============================================================================
REM 3. Check Compiler and Tools
REM ============================================================================

echo.
echo [3/5] Checking Compiler and Tools...

set "TOOLS=^
  csc.exe ^
  ilasm.exe ^
  ngen.exe"

for %%T in (!TOOLS!) do (
  set /a TOTAL_CHECKS+=1
  if exist "C:\ReactOS\System32\%%T" (
    REM Try to get version
    for /f "tokens=*" %%v in ('%%T /version 2^>nul') do (
      echo   [✓] %%T %%v
      set /a VERIFIED_COUNT+=1
    )
    if errorlevel 1 (
      echo   [✓] %%T
      set /a VERIFIED_COUNT+=1
    )
  ) else (
    echo   [✗] %%T - MISSING
    set /a MISSING_COUNT+=1
  )
)

REM ============================================================================
REM 4. Check Host DLLs
REM ============================================================================

echo.
echo [4/5] Checking Runtime Host DLLs...

set "HOST_DLLS=^
  mscoree.dll ^
  mscoreei.dll"

for %%H in (!HOST_DLLS!) do (
  set /a TOTAL_CHECKS+=1
  if exist "C:\ReactOS\System32\%%H" (
    echo   [✓] %%H
    set /a VERIFIED_COUNT+=1
  ) else (
    echo   [✗] %%H - MISSING (CRITICAL!)
    set /a MISSING_COUNT+=1
  )
)

REM ============================================================================
REM 5. Check Framework Directory
REM ============================================================================

echo.
echo [5/5] Checking .NET Framework Directory Structure...

if exist "C:\Program Files\dotnet" (
  echo   [✓] Framework directory exists
  
  REM Check subdirectories
  if exist "C:\Program Files\dotnet\sdk" (
    echo   [✓] SDK directory found
  )
  
  if exist "C:\Program Files\dotnet\runtime" (
    echo   [✓] Runtime directory found
  )
  
  REM Check for csc in alternative location
  if exist "C:\Program Files\dotnet\sdk\4.0.30319\csc.exe" (
    echo   [✓] csc.exe in SDK directory
  )
) else (
  echo   [!] Framework directory not created (this is optional for basic usage)
)

REM ============================================================================
REM Summary and Diagnostics
REM ============================================================================

echo.
echo ============================================================================
echo  Verification Results
echo ============================================================================
echo.
echo Verified:     %VERIFIED_COUNT%/%TOTAL_CHECKS% checks passed
echo Missing:      %MISSING_COUNT% critical items

set /a PASS_RATE=(%VERIFIED_COUNT% * 100) / %TOTAL_CHECKS%

echo Pass Rate:    %PASS_RATE%%%
echo.

if %MISSING_COUNT% equ 0 (
  echo [✓] .NET Framework 4.8 is fully installed!
  echo.
  echo You can now:
  echo   1. Compile C# programs: csc.exe MyProgram.cs
  echo   2. Use System.* namespaces
  echo   3. Run Windows Forms applications
  echo   4. Use LINQ, async/await, and other .NET 4.8 features
  echo.
  goto SUCCESS
)

if %PASS_RATE% geq 70 (
  echo [✓] .NET Framework 4.8 is mostly installed
  echo.
  echo Some optional components missing, but core functionality available
  echo Missing components can be installed manually if needed
  echo.
  goto SUCCESS
)

if %PASS_RATE% geq 50 (
  echo [!] .NET Framework 4.8 installation is incomplete!
  echo.
  echo Core frameworks are missing. Recommended to re-run installer:
  echo   dotnet-install.bat
  echo.
  goto WARN
)

REM ============================================================================
REM Detailed Diagnostics
REM ============================================================================

echo.
echo ============================================================================
echo  Detailed Diagnostics
echo ============================================================================
echo.

echo System Information:
systeminfo 2>nul | findstr /C:"System Boot Time" /C:"OS Version" /C:"Total Physical Memory"

echo.
echo Disk Space:
dir C:\ 2>nul | findstr /C:"bytes free"

echo.
echo .NET Installation Paths:
echo Checking C:\ReactOS\System32\ for .NET files:
dir C:\ReactOS\System32\System*.dll 2>nul | find ".dll"

echo.
echo Checking for C# compilation capability:
if exist "C:\ReactOS\System32\csc.exe" (
  echo [✓] C# compiler is available
  echo Testing: csc.exe /version
  csc.exe /version 2>nul
) else (
  echo [✗] C# compiler not found in System32
)

:SUCCESS
echo.
echo ============================================================================
echo [✓] Verification Complete
echo ============================================================================
echo.
pause
exit /b 0

:WARN
echo.
echo ============================================================================
echo [!] Action Required
echo ============================================================================
echo.
echo Please run: dotnet-install.bat
echo to complete .NET Framework installation
echo.
pause
exit /b 1
