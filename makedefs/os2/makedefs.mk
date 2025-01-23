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

!IF "$(PLATFORM)" == ""
PLATFORM=os2
!ENDIF

!IF "$(PLATFORM_PROTO)" == ""
PLATFORM_PROTO=os2
!ENDIF

!IF "$(BUILDTYPE)" == ""
BUILDTYPE=default
!ENDIF

!IF "$(OUTDIR_BASE)" == ""
OUTDIR_BASE=..\..\products\$(PLATFORM)\$(BUILDTYPE)
!ENDIF

OUTDIR_BIN=$(OUTDIR_BASE)\bin
OUTDIR_SBIN=$(OUTDIR_BASE)\sbin
OUTDIR_LIB=$(OUTDIR_BASE)\lib
OUTDIR_DLL=$(OUTDIR_BASE)\dll
OUTDIR_ETC=$(OUTDIR_BASE)\etc
OUTDIR_TOOLS=$(OUTDIR_BASE)\tools

!IF "$(CLEAN)" == ""
CLEAN=..\..\makedefs\$(PLATFORM_PROTO)\clean.cmd
!ENDIF
