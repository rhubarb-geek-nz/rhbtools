@ECHO OFF

rem
rem  Copyright 2008, Roger Brown
rem
rem  This file is part of Roger Brown's Toolkit.
rem
rem  This program is free software: you can redistribute it and/or modify it
rem  under the terms of the GNU Lesser General Public License as published by the
rem  Free Software Foundation, either version 3 of the License, or (at your
rem  option) any later version.
rem 
rem  This program is distributed in the hope that it will be useful, but WITHOUT
rem  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
rem  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
rem  more details.
rem
rem  You should have received a copy of the GNU Lesser General Public License
rem  along with this program.  If not, see <http://www.gnu.org/licenses/>
rem
rem  $Id: makepart.cmd 1 2014-02-06 21:56:41Z rhubarb-geek-nz $

set ACTION=%1
set PART=%2
set MAKEVERB=%3

if "%MAKE%" == "" goto failed

if "%ACTION%" == "makepart" goto makepart

%MAKE% makepart ACTION=%ACTION% PART=%PART%

if errorlevel 1 goto failed

goto complete

:makepart

set PART_MAK=..\..\%PART%\%PLATFORM_MAKEFILE%\%PART%.mak

if not exist %PART_MAK% set PART_MAK=..\..\%PART%\os2\%PART%.mak

if not exist %PART_MAK% goto complete

if not exist ..\..\%PART%\%PLATFORM% mkdir ..\..\%PART%\%PLATFORM%

if not exist ..\..\%PART%\%PLATFORM% goto failed

chdir ..\..\%PART%\%PLATFORM%

if errorlevel 1 goto failed

%MAKE% /F %PART_MAK% %MAKEVERB%

if errorlevel 1 goto failed

if not "%MAKEVERB%" == "clean" goto makeend

if not exist "%BUILDTYPE%" goto makeend

rmdir "%BUILDTYPE%"

if not exist "%BUILDTYPE%" goto makeend

dir "%BUILDTYPE%"

:failed

exit 1

:makeend

chdir ..\..\makedefs\%PLATFORM%

:complete
