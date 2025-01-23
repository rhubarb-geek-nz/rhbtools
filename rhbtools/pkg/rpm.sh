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
# $Id: rpm.sh 13 2021-04-18 12:45:24Z rhubarb-geek-nz $
#

RPMBUILD=rpm

if rpmbuild --help >/dev/null
then
	RPMBUILD=rpmbuild
fi

if $RPMBUILD --help >/dev/null
then
	echo RPMBUILD=$RPMBUILD
else
	exit 0
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

case "$INTDIR" in
/* )
	;;
* )
	INTDIR=`pwd`/$INTDIR
	;;
esac

BUILDROOT=$INTDIR/data
TGTPATH=$INTDIR/rpms
SPECFILE=$INTDIR/rpm.spec

PKGNAME=rhbtools
PKGROOT=opt/RHBtools

. ../version.sh

cat >"$SPECFILE" <<EOF 
Summary: RHB Toolkit for GNU
Name: $PKGNAME
Version: $VERSION
Release: 1
Group: Applications/System
License: GPL
Prefix: /$PKGROOT

%description
Set of common tools built for GNU

%files
%defattr(-,root,root)
%dir /$PKGROOT
%dir /$PKGROOT/bin
%attr(755,root,root) /$PKGROOT/bin/socket
%attr(755,root,root) /$PKGROOT/bin/tcpiptry
%attr(755,root,root) /$PKGROOT/bin/when
%attr(755,root,root) /$PKGROOT/bin/stat
%attr(755,root,root) /$PKGROOT/bin/asuser
%attr(755,root,root) /$PKGROOT/bin/what
%attr(755,root,root) /$PKGROOT/bin/textconv
%attr(755,root,root) /$PKGROOT/bin/hexdump
%attr(755,root,root) /$PKGROOT/bin/ptyexec
%attr(755,root,root) /$PKGROOT/bin/lockexec
%attr(755,root,root) /$PKGROOT/bin/not
%attr(755,root,root) /$PKGROOT/bin/svninfo
EOF

mkdir -p "$BUILDROOT/$PKGROOT/bin" 

for d in socket tcpiptry when stat asuser what textconv hexdump ptyexec lockexec not svninfo
do
	find "$OUTDIR/bin" -type f -name $d | while read N
	do
		cp "$N" "$BUILDROOT/$PKGROOT/bin/$d"
		strip "$BUILDROOT/$PKGROOT/bin/$d"
	done
done

rm -rf $TGTPATH
mkdir -p $TGTPATH

$RPMBUILD --buildroot "$BUILDROOT" --define "_rpmdir $TGTPATH" --define "_build_id_links none" -bb "$SPECFILE"

find $TGTPATH -type f -name "*.rpm" | while read N
do
	mv "$N" "$OUTDIR_DIST"
done

rm -rf "$TGTPATH" "$BUILDROOT" "$SPECFILE"

