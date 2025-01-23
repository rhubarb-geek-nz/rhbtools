#
#  Copyright 2020, Roger Brown
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
#  $Id: toolbox2.mak 8 2020-06-06 06:46:22Z rhubarb-geek-nz $

!include $(MAKEDEFS)

dist:
	cd ..
	dotnet build -t:build -p:Configuration=$(BUILDTYPE) /p:Platform="Any CPU"

clean:
	cd ..
	dotnet build -t:clean -p:Configuration=$(BUILDTYPE) /p:Platform="Any CPU"

all:


