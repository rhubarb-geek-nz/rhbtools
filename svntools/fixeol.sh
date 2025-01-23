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
# $Id: fixeol.sh 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
#

if test "$SVN" = ""
then
	SVN="svn"
fi

find . -type f | grep -v "/.svn/" | while read N
do
	case "$N" in
	*.c | *.idl | *.prt | *.mak | *.sh | *.xml | *.java | *.h | *.xh | *.exp | *.bat | *.cmd | */Makefile | *.mk | *.def | *.rc | *.txt | *.r | *.cpp | *.cf | *.scs | *.html | *.htm | *.odl | *.hpp | *.dsp | *.plist | *.ppk | *.ver | *.csproj | *.sln | *.cs | *.mm | *.mmh  | *.jsp | *.m )
		SETTING=`"$SVN" propget svn:eol-style "$N"`
		if test "$SETTING" = ""
		then
			"$SVN" propset svn:eol-style native "$N"
		fi
		;;
	* )
		;;
	esac
done
