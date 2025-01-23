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
# $Id: slack.sh 30 2022-01-12 19:52:56Z rhubarb-geek-nz $
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

mkdir -p "$INTDIR/data/$PKGROOT/bin"

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

 ../../toolbox/slackpkg.sh "$INTDIR" "$INTDIR/data" "$OUTDIR_DIST" << EOF
$PKGNAME
$VERSION
$PKGROOT
$PKGNAME: rhbtools - ad-hoc tool set for developers
$PKGNAME:
$PKGNAME: Ad hoc set of small tools for developers. Generally what can't be done
$PKGNAME: in a shell script or batch file. 
$PKGNAME:
$PKGNAME:
$PKGNAME:
$PKGNAME:
$PKGNAME:
$PKGNAME:
$PKGNAME:
EOF
