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
#  $Id: makedefs.mk 1 2014-02-06 21:56:41Z rhubarb-geek-nz $

PLATFORM_DEF=win32
CC=cl.exe /nologo
CXX=$(CC)
COMMONOPTS=/MD /DNDEBUG /DWIN32 /Zp8 /W3 /D_PLATFORM_LITTLE_ENDIAN_ /D_PLATFORM_WIN32_ 
STDOPT=$(COMMONOPTS) /WX
STDOPTXX=$(COMMONOPTS) /GX
RPCINCL=/Ic:\toolkits\msrpc\include
MKTYPLIB=mktyplib.exe
MKTYPLIB_FLAGS=
CONFIG_OPTS=/WX $(COMMONOPTS) wsock32.lib
RCFLAGS=/DNDEBUG
MORE_INCLS=c:\Toolkits\openssl\include;..\..\rhbxtw32\include

!INCLUDE ..\..\makedefs\$(PLATFORM_DEF)\makedefs.mk
