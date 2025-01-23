set MESSAGE=%1
call svntools\chkindir.bat %MESSAGE% .
if errorlevel 1 goto failed
products\win32vc6\debug\tools\find.exe . -type d | products\win32vc6\debug\tools\xargs.exe -n 1 svntools\chkindir.bat %MESSAGE%
:failed
