/**************************************************************************
 *
 *  Copyright 2008, Roger Brown
 *
 *  This file is part of Roger Brown's Toolkit.
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

/* 
 * $Id: makedefs.c 7 2020-06-06 02:59:05Z rhubarb-geek-nz $
 */

#ifdef _MSC_VER
#	if (_MSC_VER < 1200)
#		define VAR_PLATFORM_HOST win32vc4
#	elif (_MSC_VER < 1400)
#		define VAR_PLATFORM_HOST win32vc6
#	elif (_MSC_VER < 1500)
#		define VAR_PLATFORM_HOST msvs2005
#	elif (_MSC_VER < 1600)
#		define VAR_PLATFORM_HOST msvs2008
#	elif (_MSC_VER < 1700)
#		define VAR_PLATFORM_HOST msvs2010
#	elif (_MSC_VER < 1800)
#		define VAR_PLATFORM_HOST msvs2012
#	elif (_MSC_VER < 1900)
#		define VAR_PLATFORM_HOST msvs2013
#	else
#		define VAR_PLATFORM_HOST win32x86
#	endif
#	if defined(_M_IX86)
#		define VAR_PLATFORM VAR_PLATFORM_HOST
#	else
#		if defined(_WIN64)
#			define VAR_PLATFORM win32x64
#		else
#			define VAR_PLATFORM win32x86
#		endif
#	endif
#	ifdef _DEBUG
#		define VAR_BUILDTYPE Debug
#	else
#		define VAR_BUILDTYPE Release
#	endif
cd makedefs\\VAR_PLATFORM
if ERRORLEVEL 1 exit 1
if "%MAKE%" == "" set MAKE=nmake.exe
%MAKE% ProgramFiles="%ProgramFiles%" PLATFORM=VAR_PLATFORM PLATFORM_HOST=VAR_PLATFORM_HOST BUILDTYPE=VAR_BUILDTYPE %1 %2 %3 %4 %5 %6 %7 %8 %9
if ERRORLEVEL 1 exit 1
echo Build Complete
#else
#	error expecting a Microsoft C Compiler
#endif
