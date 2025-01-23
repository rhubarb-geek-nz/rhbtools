/**************************************************************************
 *
 *  Copyright 2008, Roger Brown
 *
 *  This file is part of Roger Brown's Toolkit.
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

/* 
 * $Id: asuser.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

#include <rhbopt.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#else
#	include <process.h>
#endif

#include <errno.h>

#ifdef _WIN32
typedef int gid_t,uid_t;
#endif

int main(int argc,char **argv)
{
	struct stat s;
	const char *app=argv[0];
	int len=(int)strlen(app);
	char *script=malloc(len+4);
#ifdef _WIN32
	uid_t uid=0;
	gid_t gid=0;
#else
	uid_t uid=geteuid();
	gid_t gid=getegid();
#endif

	memcpy(script,app,len);
	memcpy(script+len,".sh",4);

#ifdef _WIN32
	if (stat(app,&s))
	{
		perror(app);

		return 1;
	}
#else
	if (lstat(app,&s))
	{
		perror(app);

		return 1;
	}

	if (!S_ISREG(s.st_mode))
	{
		fprintf(stderr,"%s: not a file\n",app);

		return 1;
	}
#endif

	if (s.st_gid!=gid)
	{
		fprintf(stderr,"%s: wrong group id, %d!=%d\n",
				app,(int)s.st_gid,(int)gid);

		return 1;
	}

	if (s.st_uid!=uid)
	{
		fprintf(stderr,"%s: wrong user id, %d!=%d\n",
				app,(int)s.st_uid,(int)uid);

		return 1;
	}

#ifdef _WIN32
#else
	if (!(s.st_mode & S_ISUID))
	{
		fprintf(stderr,"%s: suid not set\n",app);

		return 1;
	}

	if (!(s.st_mode & S_ISGID))
	{
		fprintf(stderr,"%s: sgid not set\n",app);

		return 1;
	}

	if (lstat(script,&s))
	{
		perror(script);

		return 1;
	}

	if (!S_ISREG(s.st_mode))
	{
		fprintf(stderr,"%s: not a file\n",script);
		return 1;
	}
#endif

	if (s.st_gid!=gid)
	{
		fprintf(stderr,"%s: wrong group id, %d!=%d\n",
				script,(int)s.st_gid,(int)gid);

		return 1;
	}

	if (s.st_uid!=uid)
	{
		fprintf(stderr,"%s: wrong user id, %d!=%d\n",
				script,(int)s.st_uid,(int)uid);

		return 1;
	}

	argv[0]=script;

	execv(script,argv);

	perror(script);	

	return 1;
}
