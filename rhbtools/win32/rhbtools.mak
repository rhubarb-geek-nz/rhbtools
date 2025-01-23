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
#  $Id: rhbtools.mak 34 2022-11-20 18:46:07Z rhubarb-geek-nz $

!include $(MAKEDEFS)

# update code "{997A37B6-44E9-469D-8E71-C643E9D2D6F4}"

PKGNAME=rhbtools
INTDIR=$(BUILDTYPE)
TARGET_MSI=$(OUTDIR_DIST)\$(PKGNAME).msi
TARGET_CAB=$(OUTDIR_DIST)\$(PKGNAME).cab
TARGET_ZIP=$(OUTDIR_DIST)\$(PKGNAME).zip
TARGET_WSX=$(INTDIR)\$(PKGNAME).$(PLATFORM).wsx
TARGET_DDF=$(INTDIR)\$(PKGNAME).$(PLATFORM).ddf
COMMON_DDF=$(INTDIR)\$(PKGNAME).ddf
SRCLIST=kill assassin binhex unbinhex hexdump chown find findapi wol svninfo depvers depends version tcpiptry when what xargs textconv cpp config idltool inetserv 
DDF2WXS_DLL=..\..\toolbox2\ddf2wxs\bin\$(BUILDTYPE)\net6.0\ddf2wxs.dll
MAKEZIP_DLL=..\..\toolbox2\makezip\bin\$(BUILDTYPE)\net6.0\makezip.dll
DEPVERS_H=..\..\include\$(PLATFORM)\depvers.h

all:

clean:
	$(CLEAN) $(TARGET_MSI) $(TARGET_WSX) $(TARGET_ZIP) $(TARGET_CAB) $(TARGET_DDF) $(COMMON_DDF) setup.inf setup.rpt $(PKGNAME).$(PLATFORM).wixobj $(OUTDIR_DIST)\$(PKGNAME).wixpdb
	
dist:  $(TARGET_ZIP) $(TARGET_CAB) $(TARGET_MSI)

$(INTDIR):
	mkdir $@

$(COMMON_DDF): $(INTDIR) ..\win32\$(PKGNAME).mak
	echo .Set CabinetName1=$(PKGNAME).cab > $@
	echo .Set DiskDirectory1=$(OUTDIR_DIST) >> $@
	echo .Set CabinetFileCountThreshold=0 >> $@
	echo .Set FolderFileCountThreshold=0 >> $@
	echo .Set FolderSizeThreshold=0 >> $@
	echo .Set MaxCabinetSize=0 >> $@
	echo .Set MaxDiskFileCount=0 >> $@
	echo .Set MaxDiskSize=0 >> $@
	echo .Set DestinationDir=bin >> $@
	for %d in ( socket $(SRCLIST) ) do if exist $(OUTDIR_BIN)\%d.exe echo $(OUTDIR_BIN)\%d.exe >> $@
	for %d in ( socket $(SRCLIST) ) do if exist $(OUTDIR_TOOLS)\%d.exe echo $(OUTDIR_TOOLS)\%d.exe >> $@
	echo .Set DestinationDir=src >> $@
	for %d in ( $(SRCLIST) ) do if exist ..\..\%d\src\%d.c echo ..\..\%d\src\%d.c >> $@
	echo ..\..\socket2\src\socket.c >> $@
	
$(INTDIR)\$(PKGNAME).win32x64.ddf: $(COMMON_DDF)
	copy /Y $(COMMON_DDF) $@
	echo ..\..\regsvr64\src\regsvr64.c >> $@
	echo .Set DestinationDir=bin >> $@
	echo $(OUTDIR_TOOLS)\regsvr64.exe >> $@

$(INTDIR)\$(PKGNAME).win32x86.ddf: $(COMMON_DDF)
	copy /Y $(COMMON_DDF) $@
	echo ..\..\regsvr32\src\regsvr32.c >> $@
	echo .Set DestinationDir=bin >> $@
	echo $(OUTDIR_TOOLS)\regsvr32.exe >> $@

$(TARGET_WSX): $(TARGET_DDF) "$(DEPVERS_H)" ..\pkg\$(PLATFORM).wxs
	dotnet "$(DDF2WXS_DLL)" -i ..\pkg\$(PLATFORM).wxs -o $@ -d "$(TARGET_DDF)" -h "$(DEPVERS_H)" -p $(PKGNAME)

$(TARGET_MSI): $(OUTDIR_TOOLS)\version.exe $(TARGET_WSX)
	"$(WIX)\bin\candle.exe" $(TARGET_WSX) -ext WixUtilExtension 
	"$(WIX)\bin\light.exe" -cultures:null -out $@ $(PKGNAME).$(PLATFORM).wixobj -ext WixUtilExtension 

$(TARGET_CAB): $(TARGET_DDF)
	if exist setup.inf del setup.inf
	if exist setup.rpt del setup.rpt
	makecab /F $(TARGET_DDF)

$(TARGET_ZIP): $(TARGET_DDF)
	makecab /F $(TARGET_DDF)
	dotnet "$(MAKEZIP_DLL)" -d $(TARGET_DDF) -o $@
