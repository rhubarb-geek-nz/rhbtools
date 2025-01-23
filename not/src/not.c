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
 * $Id: not.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc,char **argv)
{
int status=0,rc=1;
pid_t pid,p2;

	if (argc <= 1)
	{
		fprintf(stderr,"usage: %s command....\n",argv[0]);

		return 2;
	}

	pid=fork();

	if (pid==-1)
	{
		perror("fork");
		return 3;
	}

	if (!pid)
	{
		argv++;

		execvp(argv[0],argv);

		perror(argv[0]);

		/* exit with zero, so reverse logic will return error! */

		_exit(0);
	}

	p2=wait(&status);

	if (p2!=pid)
	{
		fprintf(stderr,"wait %d != %d\n",(int)pid,(int)p2);

		return 5;
	}	

	if (WIFEXITED(status))
	{
		if (WEXITSTATUS(status))
		{
			rc=0;
		}
	}

	return rc;
}
