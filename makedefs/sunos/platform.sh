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
# $Id: platform.sh 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
#


SHLB_REF_STRONG=$OUTDIR/lib:$OUTDIR/implib
SHLB_REF_WEAK=

CONFIG_C=$INTDIR/platform.c
CONFIG_OBJ=$INTDIR/platform.o
CONFIG_CPP=$INTDIR/platform.cpp
CONFIG_EXE=$INTDIR/platform.exe

echo INTDIR=$INTDIR
echo PWD=`pwd`

SHLB_REF_STRONG=$OUTDIR/lib:$OUTDIR/implib

platform_not_member()
{
    not_member_d=$1
	if test "$2"
	then
	    shift
    	for not_member_i in $@
    	do
        	if test "$not_member_i" = "$not_member_d"
        	then
            	return 1
	        fi
    	done
	fi
    return 0;
}

platform_has_member()
{
    not_member_d=$1
	if test "$2"
	then
	    shift
    	for not_member_i in $@
    	do
        	if test "$not_member_i" = "$not_member_d"
        	then
            	return 0
	        fi
    	done
	fi
    return 1;
}

platform_first()
{
	echo "$1"
}

cat >$CONFIG_C <<EOF
#include <stdio.h>
#if TEST_GCC
#	ifdef __GNUC__
#	else
		#error compiler not gcc
#	endif
#else
#	ifdef __cplusplus
class f { int x; };
int main(int argc,char **argv)
#	else
#		ifdef __STDC__
int main(int argc,char **argv)
#		else
int main(argc,argv) int argc; char **argv;
#		endif
#	endif
{ 
#	ifdef __cplusplus
	f *g=new f;
	if (!g) return 1;
#	endif

	return (argc && argv) ? 0 : 1; 
}
#endif
EOF

cp $CONFIG_C $CONFIG_CPP

GCC_OPTS="-Wall -Werror -include ../../makedefs/$PLATFORM_PROTO/protoize.h"

$CC $CFLAGS -c $CONFIG_C -DTEST_GCC -o $CONFIG_OBJ 1>/dev/null 2>&1
CC_RES=$?

if test "$CC_RES" = "0"
then
	PLATFORM_CFLAGS="$PLATFORM_CFLAGS -Wall -Werror $GCC_OPTS"
fi

$CXX $CXXFLAGS -c $CONFIG_CPP -DTEST_GCC -o $CONFIG_OBJ 1>/dev/null 2>&1
CC_RES=$?

if test "$CC_RES" = "0"
then
	PLATFORM_CXXFLAGS="$PLATFORM_CXXFLAGS $GCC_OPTS"
fi

$CC $CFLAGS $PLATFORM_CFLAGS -c $CONFIG_C -o $CONFIG_OBJ

CC_RES=$?

if test "$CC_RES" != "0"
then
	exit "$CC_RES"
fi

$CC $CFLAGS $PLATFORM_CFLAGS $CONFIG_OBJ $PLATFORM_LIBS -o $CONFIG_EXE

CC_RES=$?

if test "$CC_RES" != "0"
then
	exit "$CC_RES"
fi

$CXX $CXXFLAGS $PLATFORM_CXXFLAGS -c $CONFIG_CPP -o $CONFIG_OBJ

CC_RES=$?

if test "$CC_RES" != "0"
then
	exit "$CC_RES"
fi

$CXX $CXXFLAGS $PLATFORM_CXXFLAGS $CONFIG_OBJ $PLATFORM_LIBS -o $CONFIG_EXE

CC_RES=$?

if test "$CC_RES" != "0"
then
	exit "$CC_RES"
fi

for platform_d in $CONFIG_C $CONFIG_OBJ $CONFIG_CPP $CONFIG_EXE
do
	if test -f $platform_d
	then
		rm $platform_d
	fi
done


MAKEDEFS_EXPORTS="$MAKEDEFS_EXPORTS SHLB_REF_STRONG SHLB_REF_WEAK"
MAKEDEFS_EXPORTS="$MAKEDEFS_EXPORTS PLATFORM_CFLAGS PLATFORM_CXXFLAGS"

MAKEDEFS_DEFS="$MAKEDEFS_DEFS CONFIG_CFLAGS CONFIG_CXXFLAGS"
MAKEDEFS_DEFS="$MAKEDEFS_DEFS STDLIB STDLIBXX LINKDLL_TAIL"
