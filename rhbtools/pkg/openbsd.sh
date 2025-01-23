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
# $Id: openbsd.sh 13 2021-04-18 12:45:24Z rhubarb-geek-nz $
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

PKGNAME=rhbtools-$VERSION.tgz
SRCROOT=$INTDIR/pkg.root
METAROOT=$INTDIR/pkg.meta

clean()
{
	rm -rf $SRCROOT $METAROOT
}

trap clean 0

clean

mkdir -p "$SRCROOT/opt/RHBtools/bin" "$METAROOT"

for d in socket tcpiptry when stat asuser what textconv hexdump ptyexec lockexec not svninfo
do
	find "$OUTDIR/bin" -type f -name $d | while read N
	do
		cp "$N" "$SRCROOT/opt/RHBtools/bin/$d"

		if test "$STRIP" != ""
		then 
			$STRIP "$SRCROOT/opt/RHBtools/bin/$d"
		fi 
	done
done

(
	set -e
	cd $SRCROOT
	find opt -type d | while read N
	do
		echo "@dir $N" 
	done
	find opt -type f
) > "$METAROOT/CONTENTS" 

cat >"$METAROOT/DESC" <<EOF
Set of common tools built for GNU that typically cannot easily be done with a shell script
EOF

COMMENT="Set of common tools built for GNU"
MACHINE_ARCH=$(uname -p)
HOMEPAGE=http://rhbtools.sf.net
MAINTAINER=rhubarb-geek-nz@users.sourceforge.net
FULLPKGPATH=devel/rhbtools
FTP=yes

if pkg_create \
		-A "$MACHINE_ARCH"\
		-d "$METAROOT/DESC" \
		-D "COMMENT=$COMMENT" \
		-D "HOMEPAGE=$HOMEPAGE" \
		-D "MAINTAINER=$MAINTAINER" \
		-D "FULLPKGPATH=$FULLPKGPATH" \
		-D "FTP=$FTP" \
		-f "$METAROOT/CONTENTS" \
		-B "$SRCROOT" \
		-p / \
		"$OUTDIR_DIST/$PKGNAME"
then
	ls -ld "$OUTDIR_DIST/$PKGNAME"
fi
