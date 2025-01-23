@echo off
set MESSAGE=%1
set WHICHDIR=%2
if exist %WHICHDIR%\.svn "%ProgramFiles%\CollabNet\Subversion Client\svn.exe" ci -m %MESSAGE% %2
