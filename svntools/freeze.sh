#!/bin/sh
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
# $Id: freeze.sh 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
#

EXT_FILE=$$.tmp.txt

get_revision()
{
	grep "Last Changed Rev:" | while read N M O P Q R S
	do
		case "$N" in
		Revision: )
			echo $M
			;;
		Last )
			echo $P
			;;
		* )
			;;
		esac
	done
}

strip_revision()
{
	while read N M O
	do
		if test "$N" != ""
		then
			case "$M" in
			-r* )
				echo "$N" "$O"
				;;
			* )
				echo "$N" "$M"
				;;
			esac
		fi
	done
}

dir_revision()
{
	while read Q R
	do
		if test -d "$Q/.svn"
		then
			REVISION=`svn info "$Q" | get_revision`
			echo "$Q" "-r$REVISION" "$R"
		fi		
	done
}

svn propget svn:externals . | strip_revision | dir_revision >$EXT_FILE

if test "$?" = "0"
then
    svn propset svn:externals -F $EXT_FILE .
fi

rm $EXT_FILE

svn propget svn:externals
