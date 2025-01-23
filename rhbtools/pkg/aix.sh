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
# $Id: aix.sh 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
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

NAME=rhbtools.rte
. ../version.sh
DESCRIPTION="Rogers Unix Tool Set for AIX - SVN $VERS"
APPLIST="what when stat socket tcpiptry asuser textconv hexdump lockexec ptyexec not svninfo"
PKGROOT=opt/RHBtools

rm -rf "$INTDIR/lpp" "$INTDIR/root" 

if test "$?" != "0"; then exit 1; fi;

mkdir -p "$INTDIR/root/$PKGROOT/bin" "$INTDIR/lpp"

if test "$?" != "0"; then exit 1; fi;

for M in $APPLIST
do
	find ../../products -name "$M" -type f | while read N
	do
		cp "$N" "$INTDIR/root/$PKGROOT/bin/$M"
		chmod 0755 "$INTDIR/root/$PKGROOT/bin/$M"
	done
done

find "$INTDIR/root" -type f | xargs touch

if test "$?" != "0"; then exit 1; fi;

gen_sizes()
{
        for d in *
        do
                if test -d $d
                then
                        du -sk $d | ( read S D ; echo /$D $S; )
                fi
        done
}

generate_lpp()
{
        NAME="$1"
        VERSION="$2"
        DESCRIPTION="$3"
        DISKETTE=01
        BOSBOOT=N
        CONTENT=U
        LANGUAGE=en_US

        mkdir -p "$INTDIR/root/usr/lpp/$NAME"

        if test ! -f "$INTDIR/lpp/$NAME.al"
        then
                (
                        cd "$INTDIR/root"
#                        echo ./lpp_name
                        find . -type f
                        find ./usr/lpp/$NAME -type d
                ) >"$INTDIR/lpp/$NAME.al"
        fi

        touch "$INTDIR/root/usr/lpp/$NAME/liblpp.a"

        (
                cd "$INTDIR/root"
                gen_sizes
        ) >"$INTDIR/lpp/$NAME.size"

        if test ! -f "$INTDIR/lpp/$NAME.copyright"
        then
                YEAR=`date +"%Y"`
                echo "Copyright (C) $YEAR, Roger Brown, All Rights Reserved"  >"$INTDIR/lpp/$NAME.copyright"
        fi

        echo 4 R I $NAME "{"
        echo $NAME $VERSION $DISKETTE $BOSBOOT $CONTENT $LANGUAGE $DESCRIPTION
        echo "["
        echo "%"
        cat "$INTDIR/lpp/$NAME.size"
        echo "%"
        echo "%"
        echo "]"
        echo "}"
}

# do it twice so we count the true size of the liblpp.a

for d in first second
do
        generate_lpp "$NAME" "$VERSION" "$DESCRIPTION" >"$INTDIR/lpp/lpp_name"

        (
                cd "$INTDIR/lpp"
                ar -vq liblpp.a $NAME.*
        )

        mv "$INTDIR/lpp/liblpp.a" "$INTDIR/root/usr/lpp/$NAME/liblpp.a"
done

mv "$INTDIR/lpp/lpp_name" "$INTDIR/root/lpp_name"

stripdot()
{
        while read N
        do
                case "$N" in
                . | .. )
                        ;;
                * )
                        echo "$N"
                        ;;
                esac
        done
}

(
        cd "$INTDIR/root"
        echo ./lpp_name
        find .
) | stripdot >"$INTDIR/lpp/bff.lst"

(
	cd "$INTDIR/root"
	backup -iqf -
) > "$OUTDIR_DIST/$NAME.$VERSION.bff" <"$INTDIR/lpp/bff.lst"

restore -T -qvf "$OUTDIR_DIST/$NAME.$VERSION.bff"

rm -rf "$INTDIR/root" "$INTDIR/lpp"
