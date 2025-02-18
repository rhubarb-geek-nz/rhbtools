#!/bin/sh -x
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
# $Id: version.sh 2 2014-02-07 08:46:03Z rhubarb-geek-nz $
#

APPLIST="asuser hexdump lockexec not ptyexec socket stat svninfo tcpiptry textconv what when"

VERS=`../../toolbox/depdirs.sh rhbtools | ../../svnvers/unix/lastrev.sh`

HIVERS=`echo $VERS / 10000 | bc`
LOVERS=`echo $VERS % 10000 | bc`

if test "$HIVERS" = ""; then HIVERS=0; fi
if test "$LOVERS" = ""; then LOVERS=0; fi

VERSION=1.6.$HIVERS.$LOVERS
