#!/bin/sh -x
#
#  Copyright 2014, Roger Brown
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
# $Id: hpux.sh 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
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

NAME=rhbtools
VENDOR=RHB
DEPOT_NAME="$OUTDIR_DIST/$NAME-$VERSION.depot"

rm -rf root "$DEPOT_NAME"

mkdir -p root/opt/RHBtools/bin

for d in $APPLIST
do
	find ../../products -name $d -type f | while read N
	do
		cp "$N" "root/opt/RHBtools/bin/$d"
	done
done

getarch()
{
	file $1 | ( read A B C ; echo $B; )
}

ARCH=`getarch root/opt/RHBtools/bin/svninfo`
UNAME_S=`uname -s`
UNAME_R=`uname -r`

case "$ARCH" in
ELF-64 )
	ARCH="32/64"
	;;
* )
	ARCH="32"
	;;
esac

ARCHITECTURE="${UNAME_S}_${UNAME_R}_${ARCH}"

cat >$NAME.psf <<EOF
depot
	layout_version	1.0
vendor
	tag				$VENDOR
	title			Roger Brown
	description		Open Source Developer
category
	tag				utility
	title			rhbtools
	description		rhb tool set
	revision		1.0
product
	tag				$NAME
	revision		$VERSION
	architecture	$ARCHITECTURE
	vendor_tag		$VENDOR
	is_patch		false
	title			rhb tool set
	description		set of simple utilities
	copyright		Copyright (C) 2014, Roger Brown
	machine_type	*
	os_name			$UNAME_S
	os_release		?.11.*
	os_version		?
	directory		/	
	is_locatable	false

	fileset
		tag				commands
		title			Tool Set Commands
		revision		$VERSION
		directory		./root/opt/RHBtools/bin=/opt/RHBtools/bin
		file			-u 755 -o root -g sys asuser
		file			-u 755 -o root -g sys hexdump
		file			-u 755 -o root -g sys lockexec
		file			-u 755 -o root -g sys not
		file			-u 755 -o root -g sys ptyexec
		file			-u 755 -o root -g sys socket
		file			-u 755 -o root -g sys stat
		file			-u 755 -o root -g sys svninfo
		file			-u 755 -o root -g sys tcpiptry
		file			-u 755 -o root -g sys textconv
		file			-u 755 -o root -g sys what
		file			-u 755 -o root -g sys when
	end
end	
EOF

/usr/sbin/swpackage -v -s $NAME.psf -x target_type=tape @ "$DEPOT_NAME"

RC=$?

if test "$RC" = 0
then
	rm $NAME.psf
	rm -rf root
fi

exit $RC


# swinstall -x mount_all_filesystems=false  -s `pwd`/rhbtools.depot rhbtools
# swlist rhbtools
# swremove -x mount_all_filesystems=false rhbtools
