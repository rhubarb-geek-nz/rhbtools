/**************************************************************************
 *
 *  Copyright 2010, Roger Brown
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
 * $Id: lockexec.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

#include <rhbopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#ifdef HAVE_SYS_FILE_H
#	include <sys/file.h>
#endif

int lockfile(int fd,int fReadOnly)
{
#ifdef HAVE_FLOCK
	int lockType=fReadOnly ? LOCK_SH : LOCK_EX;

	if (flock(fd,lockType|LOCK_NB))
	{
		fd=-1;
	}
#else
	struct flock fls;

	memset(&fls,0,sizeof(fls));

	fls.l_type=fReadOnly ? F_RDLCK : F_WRLCK;
	fls.l_whence=SEEK_SET;
/*	fls.l_start=0;
	fls.l_len=0;
	fls.l_pid=0;
*/
	if (fcntl(fd,F_SETLKW,&fls))
	{
		fd=-1;
	}
#endif
	return fd;
}

int main(int argc,char **argv)
{
	int fd=-1;
	int i;

	if (argc < 3)
	{
		fprintf(stderr,"usage: %s file program...\n",argv[0]);
		return 1;
	}

	fd=open(argv[1],O_RDWR);

	if (fd==-1)
	{
		perror(argv[1]);

		return 2;
	}

	if (lockfile(fd,0)<0)
	{
		perror(argv[1]);

		return 3;
	}

	i=fork();

	if (!i)
	{
		close(fd);

		execvp(argv[2],argv+2);

		perror(argv[2]);

		_exit(4);
	}
	else
	{
		if (i==-1)
		{
			perror(argv[2]);

			i=5;
		}
		else
		{
			int status=0;
			waitpid(i,&status,0);

			if (WIFEXITED(status))
			{
				i=WEXITSTATUS(status);
			}
			else
			{
				i=6;
			}
		}
	}


	return i;
}
