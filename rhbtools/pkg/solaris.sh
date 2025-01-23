#!/bin/sh -ex
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
# $Id: solaris.sh 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
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

PKGNAME=RHBtools
. ../version.sh
DESCRIPTION="Rogers Unix Tool Set for Solaris"
APPLIST="what when stat socket tcpiptry asuser textconv hexdump lockexec ptyexec not svninfo"
PKGROOT="$INTDIR/pkgroot"
PKGTMP="$INTDIR/pkgtmp"

rm -rf "$PKGROOT" "$PKGTMP"

mkdir -p "$PKGROOT/bin" "$PKGTMP"

for d in $APPLIST
do
	find "$OUTDIR/bin" -name "$d" | while read N
	do
		cp "$N" "$PKGROOT/bin/$d"
	done
done

ARCH_P=`uname -p`
ARCH_M=`uname -m`

PKGFILE="$OUTDIR_DIST/$PKGNAME-$VERSION-$ARCH_P.pkg"

rm -rf "$PKGFILE"

(
	echo "ARCH=\"$ARCH_P.$ARCH_M\""
	echo "CATEGORY=\"utility\""
	echo "NAME=\"$DESCRIPTION\""
	echo "PKG=\"$PKGNAME\""
	echo "VERSION=\"$VERSION\""
	echo "BASEDIR=\"/opt/RHBtools\""
) >"$INTDIR/pkginfo"

(
	echo i pkginfo
	echo d none bin 0755 root root
	for d in $APPLIST
	do
		echo f none "bin/$d" 0755 root root
	done
) >"$INTDIR/prototype"

pkgmk -o -r "$PKGROOT" -d "$PKGTMP" -f "$INTDIR/prototype" $PKGNAME

touch "$PKGFILE"

pkgtrans -s "$PKGTMP" "$PKGFILE" "$PKGNAME"

rm -rf $PKGROOT $PKGTMP $INTDIR/prototype $INTDIR/pkginfo

pkginfo -l -d "$PKGFILE"

