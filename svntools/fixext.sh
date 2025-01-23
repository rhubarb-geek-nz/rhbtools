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
# $Id: fixext.sh 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
#

if test "$SVN" = ""
then
	SVN="svn"
fi

TMPFILE=svn.ext

first()
{
	echo $1
}

svn propget svn:externals . | while read A B C
do
	if test "$C" = ""
	then
		case "$A" in
		http* )
			case "$A" in 
			*@* )
				PARTS=`echo $A | sed y/@/\ /`
				A=`first $PARTS`
				echo $B $A
				;;
			* )
				echo $B $A
				;;
			esac
			;;
		* )
			echo $A $B
			;;
		esac		
	else
		echo error three path line: $A $B $C 1>&2
		exit 1
	fi
done >$TMPFILE

echo TMPFILE=$TMPFILE 1>&2
