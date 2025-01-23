set MAKECAB_RC=0

if "%CABWIZ_EXE%"=="" goto fin

set MAKECAB_RC=1

set INF_FILE=%1
set OUTDIR=%2
set FILESTEM=%3

copy "%INF_FILE%" "%OUTDIR%\%FILESTEM%.inf"

if errorlevel goto fin

pushd "%OUTDIR%"

"%CABWIZ_EXE%" "%FILESTEM%.inf"

set MAKECAB_RC=%ERRORLEVEL%

del "%FILESTEM%.inf"

popd

:fin

exit %MAKECAB_RC%
