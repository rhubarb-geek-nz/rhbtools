#!/bin/sh
#
#  Copyright 2010, Roger Brown
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
# $Id: findexts.sh 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
#

if test "$SVN" = ""
then
	for d in /usr/bin/svn /usr/local/bin/svn
	do
		if test -x "$d"
		then
			SVN="$d"
			break
		fi
	done
	if test "$SVN" = ""
	then
		SVN=svn
	fi
fi

find . -type d | while read N
do
	if test -d "$N/.svn"
	then
		EXT=`$SVN propget svn:externals "$N"`
		if test "$EXT" != ""
		then
			echo "$N"
		fi
	fi
done
