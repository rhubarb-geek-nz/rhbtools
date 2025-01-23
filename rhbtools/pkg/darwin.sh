#!/bin/sh -e
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
# $Id: darwin.sh 13 2021-04-18 12:45:24Z rhubarb-geek-nz $
#

FLAG=

INTDIR=.

for d in $@
do
	if test "$FLAG" = ""
	then
		FLAG="$d"
	else
		case "$FLAG" in
		-d )
			OUTDIR_DIST="$d"
			;;
		-r )
			OUTDIR="$d"
			;;
		-t )
			INTDIR="$d"
			;;
		* )
			unknown option $d
			exit 1
		esac
		FLAG=
	fi
done

. ../version.sh
PKGNAME=RHBtools
APPLIST="asuser hexdump lockexec not ptyexec socket stat svninfo tcpiptry textconv what when"

cleanup()
{
	rm -rf "$INTDIR/bom"	
}

trap cleanup 0

rm -rf "$INTDIR/bom" 
mkdir -p "$INTDIR/bom/bin"

for d in $APPLIST
do
	find "$OUTDIR/bin" -type f -name "$d" | while read N
	do
		cp "$N" "$INTDIR/bom/bin"
	done
done

rm -rf "$OUTDIR_DIST/$PKGNAME.pkg"

pkgbuild --root "$INTDIR/bom" --identifier nz.geek.rhubarb.rhbtools --version "$VERSION" --install-location "/usr/local/opt/$PKGNAME" "$OUTDIR_DIST/$PKGNAME.pkg"
