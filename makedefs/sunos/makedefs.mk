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

X11BASE=/opt/X11R6

X11INCL=-I$(X11BASE)/include
X11LIBS=-L$(X11BASE)/lib -lXmu -lXext -lXt -lX11 

STDINCL=-I. -I.. -I../../include/$(PLATFORM)/$(BUILDTYPE) \
		-I../../rhbseh/include -I../../include \
		$(X11INCL)
SOCKLIBS=
RANLIB=ranlib 
SHLIBPREFIX=lib
SHLIBSUFFIX=.a
DLLPREFIX=lib
DLLSUFFIX=.a
DLLPATHENV=PATH
STDLIB=
STDSALIBS=
STDLIBXX=
COMMONOPTS=-DBUILD_STATIC -D_PLATFORM_UNIX_ -D_PLATFORM_X11_ -D_ALL_SOURCE
STDOPT=$(PLATFORM_CFLAGS) $(MAKEDEFS_CFLAGS) $(COMMONOPTS)
STDOPTXX=$(PLATFORM_CXXFLAGS) $(MAKEDEFS_CXXFLAGS) $(COMMONOPTS)
LINKDLL=AR=$(AR) RANLIB=$(RANLIB) LIBPREFIX=lib LIBSUFFIX=.a  ../../toolbox/makeliba.sh
LINKDLLXX=AR=$(AR) RANLIB=$(RANLIB) LIBPREFIX=lib LIBSUFFIX=.a  ../../toolbox/makeliba.sh
LINKAPP=AR=$(AR) RANLIB=$(RANLIB) LIBPREFIX=lib LIBSUFFIX=.a ../../toolbox/adlib.sh ../../toolbox/collect.sh $(CC) 
LINKAPPXX=AR=$(AR) RANLIB=$(RANLIB) LIBPREFIX=lib LIBSUFFIX=.a ../../toolbox/adlib.sh ../../toolbox/collect.sh $(CXX) 
CONFIG_HEAD=
CONFIG_TAIL=`$(SHLB_REF) missing missing`
LINKDLL_HEAD= 
LINKDLL_TAIL=
LINKAPP_HEAD= 
LINKAPP_TAIL=`$(SHLB_REF) rhbseh rhbseh` $(CONFIG_TAIL)
CC_EXE=$(CC) $(CFLAGS) 
CC_DLL=$(CC) $(CFLAGS) 
CXX_EXE=$(CXX) $(CXXFLAGS) 
CXX_DLL=$(CXX) $(CXXFLAGS) 
MISSING_CFLAGS=-DNEED_MEMMOVE
PLATFORM_SCFLAGS=-D_PLATFORM_UNIX_ -D_PLATFORM_X11_
