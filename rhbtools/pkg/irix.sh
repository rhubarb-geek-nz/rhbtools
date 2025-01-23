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
# $Id: irix.sh 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
#

PKGNAME=rhbtools
HERE=`pwd`
PKGROOT=opt/RHBtools

. ../version.sh

if test "$VERSION" = ""
then
	VERSION=0.0.0.1
fi

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

rm -rf dist $PKGNAME.tardist

if test "$?" != 0; then exit 1; fi

for APPNAME in $APPLIST
do
	echo f 0755 root sys $PKGROOT/bin/$APPNAME $PKGROOT/bin/$APPNAME $PKGNAME.sw.base
done >$PKGNAME.idb

cat >$PKGNAME.spec <<EOF
product $PKGNAME
    id "RHB Tool Set"
    image sw
        id "Software"
        version 1
        order 9999
        subsys base
            id "Base Software"
            replaces self
            exp $PKGNAME.sw.base
        endsubsys
    endimage
endproduct
EOF

mkdir -p dist root/$PKGROOT/bin

for APPNAME in $APPLIST
do
	find ../../products -name "$APPNAME" | while read N
	do
		cp "$N" "root/$PKGROOT/bin/$APPNAME"
		strip "root/$PKGROOT/bin/$APPNAME"
	done
done

if test "$?" != 0; then exit 1; fi

gendist -rbase 	$HERE/root  		\
		-sbase 	$HERE/root			\
		-idb	$HERE/$PKGNAME.idb		\
		-spec	$HERE/$PKGNAME.spec	\
		-dist	$HERE/dist			\
		-verbose					\
		"$PKGNAME*"

if test "$?" != 0; then exit 1; fi

(
	cd dist

	if test "$?" != 0; then exit 1; fi

	tar cf $PKGNAME.tardist *
)

if test "$?" != 0; then exit 1; fi

mv "dist/$PKGNAME.tardist" "$OUTDIR_DIST/$PKGNAME-$VERSION.tardist"

rm -rf dist root $PKGNAME.spec $PKGNAME.idb

