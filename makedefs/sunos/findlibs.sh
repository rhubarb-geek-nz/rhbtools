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
# $Id: findlibs.sh 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
#

X11CONTENDERS="/usr/X11R6 /opt/X11R6 /usr/openwin"

FINDLIBS_REQUIRED_GLOBALS="char *strerror(int x) { return NULL; }"

. ../../toolbox/findlibs.sh

if test "$X11INCL" != ""
then
	export X11INCL
fi


if test "$X11LIBPATH" != ""
then
	X11LIBPATH=`arglist -L $X11LIBPATH`

	export X11APP
	export X11LIBPATH

	if test "$X11LIBS" != ""
	then
		X11LIBS="$X11LIBPATH $X11LIBS"
	fi
fi

if test "$CONFIG_LIBS" != ""
then
	export CONFIG_LIBS
fi

if test "$X11LIBS" != ""
then
	export X11LIBS
fi


