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
#  $Id: Makefile 1 2014-02-06 21:56:41Z rhubarb-geek-nz $

!IF "$(PLATFORM)" == ""
PLATFORM=os2
!ENDIF

!IF "$(BUILDTYPE)" == ""
BUILDTYPE=default
!ENDIF

!IF "$(PLATFORM_MAKEFILE)" == ""
PLATFORM_MAKEFILE=os2
!ENDIF

MAKEDEFS=..\..\makedefs\$(PLATFORM)\$(BUILDTYPE)\makedefs.mk
MAKEDEFS_LST=..\$(PLATFORM)\$(BUILDTYPE)\makedefs.lst

!INCLUDE $(MAKEDEFS)

BASEPARTS=find xargs depends lcc

DIRLIST=$(OUTDIR_BASE) \
        $(OUTDIR_BIN) \
 		$(OUTDIR_ETC) \
		$(OUTDIR_TOOLS) \
		$(OUTDIR_LIB) \
		$(OUTDIR_DLL)

all:	mkdir $(DIRLIST) $(BASEPARTS) $(MAKEDEFS_LST) allparts
	echo Built $@ for $(PLATFORM)\$(BUILDTYPE)

clean:
	set MAKE=$(MAKE)
	set PLATFORM=$(PLATFORM)
	set PLATFORM_MAKEFILE=$(PLATFORM_MAKEFILE)
	set BUILDTYPE=$(BUILDTYPE) 
	set MAKEDEFS=$(MAKEDEFS)
	cd ..\..\makedefs\os2
	if exist $(MAKEDEFS_LST) if exist $(OUTDIR_TOOLS)\xargs.exe $(OUTDIR_TOOLS)\xargs.exe -n 1 makepart.cmd clean <$(MAKEDEFS_LST) 
	for %%P in ( $(BASEPARTS) mkdir ) do $(MAKE) makepart "PART=%%P" "ACTION=$@"
	$(CLEAN) $(MAKEDEFS_LST)

allparts:
	set MAKE=$(MAKE)
	set PLATFORM=$(PLATFORM)
	set BUILDARCH=$(BUILDARCH)
	set BUILDTYPE=$(BUILDTYPE)
	set MAKEDEFS=$(MAKEDEFS)
	set PLATFORM_MAKEFILE=$(PLATFORM_MAKEFILE)
	cd ..\..\makedefs\os2
	$(OUTDIR_TOOLS)\xargs.exe -n 1 <$(MAKEDEFS_LST) makepart.cmd all

$(BASEPARTS) mkdir:
	$(MAKE) makepart "PART=$@" "ACTION=all" 

$(DIRLIST):
	if not exist $@ mkdir $@

makepart:
	set PART=$(PART)
	set ACTION=$(ACTION)
	set MAKE=$(MAKE)
	set PLATFORM=$(PLATFORM)
	set PLATFORM_MAKEFILE=$(PLATFORM_MAKEFILE)
	set BUILDTYPE=$(BUILDTYPE) 
	set MAKEDEFS=$(MAKEDEFS)
	call ..\os2\makepart.cmd makepart $(PART) $(ACTION)

$(MAKEDEFS_LST): find depends
	$(OUTDIR_TOOLS)\find.exe ..\.. -name makedefs.cf | $(OUTDIR_TOOLS)\depends.exe >$@
