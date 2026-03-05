@echo off
REM =====================================================
REM Script para copiar DLLs proprietárias após boot
REM =====================================================
REM Executar como: copy-proprietary-dlls.cmd

REM Diretório de origem (CD/ISO raiz)
set SOURCE=D:\
if not exist "%SOURCE%\DLLs" (
  REM Tentar raiz do CD
  for %%A in (D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
    if exist "%%A:\reactos\system32\smss.exe" (
      set SOURCE=%%A:\
      goto :found_source
    )
  )
)

:found_source
REM Diretório de destino (system32)
set DEST=%SystemRoot%\system32

echo =====================================================
echo  Copiando DLLs proprietárias...
echo =====================================================
echo Origem: %SOURCE%DLLs
echo Destino: %DEST%
echo.

REM Lista de DLLs proprietárias a copiar
set DLLS=^
  bcrypt.dll ^
  bcryptprimitives.dll ^
  concrt140.dll ^
  cryptsp.dll ^
  evr.dll ^
  mf.dll ^
  mfcore.dll ^
  mfplat.dll ^
  mfplay.dll ^
  mfreadwrite.dll ^
  msvcp140.dll ^
  msvcp140_1.dll ^
  msvcp140_2.dll ^
  msvcp140_atomic_wait.dll ^
  msvcp140_codecvt_ids.dll ^
  ncrypt.dll ^
  PhotoMetadataHandler.dll ^
  ucrtbase.dll ^
  vccorlib140.dll ^
  vcruntime140.dll ^
  vcruntime140_1.dll ^
  WindowsCodecs.dll

for %%D in (%DLLS%) do (
  if exist "%SOURCE%DLLs\%%D" (
    echo Copiando: %%D
    copy "%SOURCE%DLLs\%%D" "%DEST%\%%D" /Y >nul
  ) else (
    echo AVISO: %%D não encontrado em %SOURCE%DLLs
  )
)

echo.
echo =====================================================
echo  Cópia concluída!
echo =====================================================
pause
