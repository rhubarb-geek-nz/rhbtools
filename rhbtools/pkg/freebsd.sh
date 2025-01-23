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
# $Id: freebsd.sh 13 2021-04-18 12:45:24Z rhubarb-geek-nz $
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
SRCROOT=$INTDIR/pkg.root
MANIFEST=$INTDIR/manifest
PLIST=$INTDIR/plist

cleanup()
{
	rm -rf $SRCROOT $MANIFEST $PLIST	
}

trap cleanup 0

cleanup

PREFIX=
PKGDIR=opt/RHBtools

mkdir -p $SRCROOT/$PREFIX/$PKGDIR/bin

for d in socket tcpiptry when stat asuser what textconv hexdump ptyexec lockexec not svninfo
do
	find "$OUTDIR/bin" -type f -name $d | while read N
	do
		BN=$(basename "$N")
		cp "$N" "$SRCROOT/$PREFIX/$PKGDIR/bin/$BN"
		if test "$STRIP" != ""
		then 
			$STRIP "$SRCROOT/$PREFIX/$PKGDIR/bin/$BN"
		fi 
	done
done

cat > $MANIFEST <<EOF
name $PKGNAME
version $VERSION
desc Set of common tools built for GNU
www http://rhbtools.sf.net
origin devel/rhbtools
comment Set of common tools built for GNU that typically cannot easily be done with a shell script
maintainer rhubarb-geek-nz@users.sourceforge.net
prefix /$PREFIX
EOF

(
	cd $SRCROOT/$PREFIX
	find $PKGDIR/bin -type f
) > $PLIST

if pkg create -M "$MANIFEST" -o "$OUTDIR_DIST" -r "$SRCROOT" -v -p "$PLIST"
then
	pkg info -F "$OUTDIR_DIST/$PKGNAME-$VERSION.txz"
	pkg info -l -F "$OUTDIR_DIST/$PKGNAME-$VERSION.txz"
fi
