#!/bin/sh -e
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
# $Id: gnuelf.sh 36 2023-12-27 11:08:34Z rhubarb-geek-nz $
#

../pkg/deb.sh $@
../pkg/rpm.sh $@
../pkg/netbsd.sh $@
../pkg/freebsd.sh $@
../pkg/openbsd.sh $@
../pkg/slack.sh $@
../pkg/ipk.sh $@
