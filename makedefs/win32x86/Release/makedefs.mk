#
#  Copyright 2008, Roger Brown
#
#  This file is part of Roger Brown's Toolkit.
#
#  This program is free software: you can redistribute it and/or modify it
#  under the terms of the GNU Lesser General Public License as published by the
#  Free Software Foundation, either version 3 of the License, or (at your
#  option) any later version.
# 
#  This program is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#  more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>
#
#  $Id: makedefs.mk 7 2020-06-06 02:59:05Z rhubarb-geek-nz $

COMMONOPTS=/W3 /MD /Zp8 /Zi /DNDEBUG /DWIN32 /D_WIN32_DCOM /D_CRT_SECURE_NO_DEPRECATE /D_CRT_NONSTDC_NO_DEPRECATE /D_PLATFORM_LITTLE_ENDIAN_ /D_PLATFORM_WIN32_ /D_WINSOCK_DEPRECATED_NO_WARNINGS 
STDOPT=$(COMMONOPTS) /WX 
STDOPTXX=$(COMMONOPTS)  /EHa
CC=cl.exe /nologo
CXX=$(CC)  
CC_OUT_EXE=/Zi /Fe
CC_OUT_DLL=/Zi /LD /Fe
LDFLAGS=/INCREMENTAL:NO /NOLOGO /DEBUG
MKTYPLIB=midl.exe /mktyplib203
MKTYPLIB_FLAGS=
CONFIG_OPTS=/WX $(COMMONOPTS) ws2_32.lib
RCFLAGS=/DNDEBUG
PLATFORM_DEF=win32
POSTLINK_EXE=..\..\makedefs\$(PLATFORM)\postlink.bat 1
POSTLINK_DLL=..\..\makedefs\$(PLATFORM)\postlink.bat 2

!INCLUDE ..\..\makedefs\$(PLATFORM_DEF)\makedefs.mk
