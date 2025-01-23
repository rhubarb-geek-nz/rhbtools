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
# $Id: fixkeywd.sh 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
#

if test "$SVN" = ""
then
	SVN="svn"
fi

find . -type f  | grep -v "/.svn/" | while read N
do
	case "$N" in
	*.c | *.idl | *.mak | *.sh | *.java | *.h | *.xh | *.bat | *.cmd | */Makefile | *.mk | *.def | *.rc | *.r | *.cpp | *.html | *.htm | *.odl | *.hpp | *.mm | *.mmh | *.ver | *.cs | *.jsp | *.m )
		SETTING=`"$SVN" propget svn:keywords "$N"`
		if test "$SETTING" = ""
		then
			case "$N" in
			*/win32vc*/*mak )
				;;
			* )
				"$SVN" propset svn:keywords "Id Revision" "$N"
				;;
			esac
		fi
		;;
	* )
		;;
	esac
done
