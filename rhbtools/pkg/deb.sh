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
# $Id: deb.sh 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
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

if test "$GTAR" = ""
then
	if ../../toolbox/pkgtool.sh gtar
	then
		GTAR=`../../toolbox/pkgtool.sh gtar`
	fi

	if test "$GTAR" = ""
	then
		exit 0
	fi
fi

rm -rf $INTDIR/data $INTDIR/control
mkdir -p $INTDIR/control $INTDIR/data/$PKGROOT/bin

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

(
	cd $INTDIR/data

	if test "$?" != 0
	then
		exit 1
	fi
	
	$GTAR --owner=0 --group=0 --create --file ../data.tar  ./*
)

../../toolbox/pkgtool.sh arch $INTDIR/data/$PKGROOT/bin/socket
../../toolbox/pkgtool.sh dpkg-arch $INTDIR/data/$PKGROOT/bin/socket

ARCH=`../../toolbox/pkgtool.sh dpkg-arch $INTDIR/data/$PKGROOT/bin/socket`
PACKAGE_NAME="$PKGNAME"_"$VERSION"_"$ARCH".deb

(
	cd $INTDIR/control

	if test "$?" != 0
	then
		exit 1
	fi

	cat >control <<EOF
Package: $PKGNAME
Version: $VERSION
Architecture: $ARCH
Maintainer: owner@rhbtools.sf.net
Section: misc
Priority: extra
Description: Set of common tools built for GNU
EOF

	cat >preinst <<EOF
#!/bin/sh -e
EOF

	cat >prerm <<EOF
#!/bin/sh -e
EOF

    cat >postrm <<EOF
#!/bin/sh
EOF

    cat >postinst <<EOF
#!/bin/sh
EOF

	chmod +x preinst prerm postinst postrm

	if test "$?" != 0
	then
		exit 1
	fi

	$GTAR --owner=0 --group=0 --create --file ../control.tar  ./*
)

(
	cd $INTDIR

	for d in control data
	do
		gzip $d.tar
	done

	if test "$?" != 0
	then
		exit 1
	fi

	rm -rf "$PACKAGE_NAME"

	echo "2.0" >debian-binary

	ar r "$PACKAGE_NAME" debian-binary data.tar.gz control.tar.gz

	rm -rf data data.tar.gz control control.tar.gz debian-binary
)

if test "$?" != 0
then
	exit 1
fi

mv $INTDIR/$PACKAGE_NAME $OUTDIR_DIST/$PACKAGE_NAME
