#!/bin/sh -ex

svn propget svn:externals . | while read M N O
do
	if test -d "$M"
	then
		if test ! -f "$M/branch.txt"
		then
			uname -a >"$M/branch.txt"
			svn add "$M/branch.txt"
		fi
		date >"$M/branch.txt"
		svn ci -m "`uname -a`" "$M"
	fi
done

