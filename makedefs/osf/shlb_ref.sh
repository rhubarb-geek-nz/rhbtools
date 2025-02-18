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
# $Id: shlb_ref.sh 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
#


for d in `echo $SHLB_REF_STRONG $SHLB_REF_WEAK | sed y/:/\ /`
do
	if test -f $d/lib$2.so
	then
		echo -L$d -l$2
		exit 0
	fi
done

echo failed to find lib $2 in \"$SHLB_REF_STRONG\" or \"$SHLB_REF_WEAK\" 1>&2

