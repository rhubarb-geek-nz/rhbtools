rem Copyright 2012, Roger Brown
rem All rights reserved
rem $Id: makemsi.bat 1 2014-02-06 21:56:41Z rhubarb-geek-nz $

set PATH=%PATH%;"%ProgramFiles%\makemsi"

pushd %1

call "%ProgramFiles%\makemsi\mm.cmd" %2

if errorlevel 1 goto :failed

popd

goto :goodend

:failed

exit 1

:goodend
