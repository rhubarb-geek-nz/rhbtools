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

STDINCL=-I. -I.. -I../../include/$(PLATFORM)/$(BUILDTYPE)  -I../../include -I../../rhbseh/include
SOCKLIBS= 
SHLIBSUFFIX=.a
SHLIBPREFIX=lib
DLLSUFFIX=.a
DLLPREFIX=lib
DLLPATHENV=PATH
STDLIB=
STDSALIBS=-lbsd -lposix
X11LIBS=-lXext -lXt -lX11
STDLIBXX=
COMMONOPT=-D_PLATFORM_UNIX_ -D_PLATFORM_AUX_ -D_PLATFORM_X11_ -DBUILD_STATIC -D_POSIX_SOURCE
STDOPT=$(PLATFORM_CFLAGS) $(MAKEDEFS_CFLAGS) $(COMMONOPT)
STDOPTXX=$(PLATFORM_CXXFLAGS) $(MAKEDEFS_CXXFLAGS) $(COMMONOPT)
TOOLBOXENV=AR=$(AR) RANLIB=$(RANLIB) LIBPREFIX=$(DLLPREFIX) LIBSUFFIX=$(DLLSUFFIX) 
LINKDLL=$(TOOLBOXENV) ../../toolbox/makeliba.sh
LINKDLLXX=$(TOOLBOXENV) ../../toolbox/makeliba.sh
LINKAPP=$(TOOLBOXENV) ../../toolbox/adlib.sh ../../toolbox/collect2.sh $(CC) $(CFLAGS)
LINKAPPXX=$(TOOLBOXENV) ../../toolbox/adlib.sh ../../toolbox/collect2.sh $(CXX) $(CXXFLAGS)
LINKDLL_HEAD=
LINKDLL_TAIL=
LINKAPP_HEAD=
LINKAPP_TAIL=`$(SHLB_REF) rhbseh rhbseh` $(STDSALIBS) -lat -lslots
CONFIG_DEFS=-D_POSIX_SOURCE
CC_DLL=$(CC) $(CFLAGS)
CXX_DLL=$(CXX) $(CXXFLAGS)
CC_EXE=$(CC) $(CFLAGS)
CXX_EXE=$(CXX) $(CXXFLAGS)
PLATFORM_SCFLAGS=-D_PLATFORM_UNIX_ -D_PLATFORM_X11_
