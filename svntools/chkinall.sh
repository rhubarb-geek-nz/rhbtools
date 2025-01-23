#!/bin/sh

find . -type d | grep -v "\.svn" | while read N
do
	if test -d "$N/.svn"
	then
		svn ci -m chkinall "$N"
	fi
done
