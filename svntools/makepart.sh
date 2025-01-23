#!/bin/sh -x

for d in $@
do
	if test -f "$d.prt"
	then
		echo 1>&2 "$d.prt" exists
	else
		if test -d "$d"
		then
			(
				echo "$d.prt"
				find "$d" -type f | grep -v "/.svn"
			) >"$d.prt"
		else
			echo 1>&2 "$d" does not exist
		fi
	fi
done
