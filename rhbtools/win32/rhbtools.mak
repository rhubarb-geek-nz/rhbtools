#
#  Copyright 2012, Roger Brown
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
#  $Id: rhbtools.mak 1 2014-02-06 21:56:41Z rhubarb-geek-nz $

!include $(MAKEDEFS)

PKGNAME=rhbtools
OUTDIR_TMP=$(OUTDIR_BIN)\..\tmp
TARGET_MSI=$(OUTDIR_DIST)\$(PKGNAME).msi
SRCLIST=socket2 kill assassin binhex unbinhex hexdump chown find findapi wol svninfo depvers depends version tcpiptry regsvr32 when what xargs textconv cpp config idltool inetserv regsvr64
MAKEDEFS_CFS=..\..\makedefs\$(PLATFORM)\$(BUILDTYPE)\makedefs.cfl

all:

clean:
	$(CLEAN) $(TARGET_MSI)  $(OUTDIR_DIST)\$(PKGNAME)-*.msi
	
dist: $(TARGET_MSI)

$(TARGET_MSI): $(OUTDIR_TOOLS)\version.exe $(MMPKGDIR)\$(PKGNAME).mm $(MAKEDEFS_CFS)
	set OUTDIR_DIST=$(OUTDIR_DIST)
	set BUILDTYPE=$(BUILDTYPE)
	set PLATFORM=$(PLATFORM)
	set OLDPATH=$(PATH)
	if exist $(OUTDIR_TMP) rmdir /S /Q $(OUTDIR_TMP)
	mkdir $(OUTDIR_TMP)
	mkdir $(OUTDIR_TMP)\src
	for %d in ( $(SRCLIST) ) do copy ..\..\%d\src\*.c $(OUTDIR_TMP)\src
	dir $(OUTDIR_TMP)\src
	copy $(MMPKGDIR)\*.* $(OUTDIR_TMP)
	"$(RHBTOOLS_BIN)\depvers.exe" --guid "{997A37B6-44E9-469D-8E71-C643E9D2D6F4}" --type msi --name $(PKGNAME) >$(OUTDIR_TMP)\$(PKGNAME).ver <$(MAKEDEFS_CFS)
	type $(OUTDIR_TMP)\$(PKGNAME).ver
	call ..\..\makedefs\win32\makemsi.bat $(OUTDIR_TMP) $(PKGNAME).mm
	copy /Y $(OUTDIR_TMP)\out\$(PKGNAME).mm\MSI\$(PKGNAME)*.msi $(OUTDIR_DIST)
	rmdir /S /Q $(OUTDIR_TMP)
