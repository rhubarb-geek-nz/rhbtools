#!/bin/sh -e
#
#  Copyright 2021, Roger Brown
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
# $Id: deb.sh 13 2021-04-18 12:45:24Z rhubarb-geek-nz $
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
PKGNAME=rhbtools
PKGROOT=opt/RHBtools

clean()
{
	rm -rf "$INTDIR/data"
}

trap clean 0

clean

mkdir -p "$INTDIR/data/DEBIAN" "$INTDIR/data/$PKGROOT/bin"

for d in socket tcpiptry when stat asuser what textconv hexdump ptyexec lockexec not svninfo
do
	find "$OUTDIR/bin" -type f -name $d | while read N
	do
		cp "$N" "$INTDIR/data/$PKGROOT/bin/$d"
		if test "$STRIP" != ""
		then
			"$STRIP" "$INTDIR/data/$PKGROOT/bin/$d"
		fi
	done
done

if dpkg --print-architecture
then
	ARCH=$(dpkg --print-architecture)
	PACKAGE_NAME="$PKGNAME"_"$VERSION"_"$ARCH".deb

	cat > "$INTDIR/data/DEBIAN/control" <<EOF
Package: $PKGNAME
Version: $VERSION
Architecture: $ARCH
Maintainer: rhubarb-geek-nz@users.sourceforge.net
Section: misc
Priority: extra
Description: Set of common tools built for GNU
EOF

	dpkg-deb --root-owner-group --build "$INTDIR/data" "$OUTDIR_DIST/$PACKAGE_NAME"
	ls -ld "$OUTDIR_DIST/$PACKAGE_NAME"
fi
