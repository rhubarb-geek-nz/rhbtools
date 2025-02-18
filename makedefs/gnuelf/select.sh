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
# $Id: select.sh 18 2021-12-05 23:07:32Z rhubarb-geek-nz $
#

case "$1" in
vote )
	case "$2" in
		*-linux*  )
			case `uname -r` in
				1.* ) 	
					exit 1
					;;
				* )	
					echo 50
					;;
			esac
			;;
		*-netbsd* | *-openbsd* | *-freebsd* | *-dragonfly* ) 
			echo 60
			;;
		*-qnx* ) 
			echo 60
			;;
		*-gnu* ) 
			echo 60
			;;
		* )
			exit 1
			;;
	esac
	;;
options )
	echo platform
	case "$2" in
		*-linux* )
			case `uname -r` in
				1.* ) 	
					echo seh
					;;
				* )	
					echo pthread
					;;
			esac
			echo findlibs
			;;
		*-netbsdelf1.* | *-dragonfly* ) 
			echo seh findlibs
			;;
		*-netbsd* | *-qnx* | *-freebsd* | *-openbsd* | *-gnu* ) 
			echo pthread findlibs
			;;
		* )
			;;
	esac
	;;
name )
	echo gnuelf
	;;
* )
	exit 1
	;;
esac
