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
 * $Id: xargs.c 16 2021-05-06 23:39:34Z rhubarb-geek-nz $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int has_spaces(const char *p)
{
	while (*p)
	{
		switch (*p)
		{
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			return 1;
		}

		p++;
	}

	return 0;
}

static int do_command(int argc,char **argv)
{
	char *args=NULL;
	int i=0;
	size_t argLen=0;
	char *p=0;

	while (i < argc)
	{
		p=argv[i++];

		argLen+=1+strlen(p);

		if (has_spaces(p))
		{
			argLen+=2;
		}
	}

	args=malloc(argLen+1);

	p=args;

	*p=0;

	i=0;

	while (i < argc)
	{
		const char *arg2=argv[i];
		size_t len=strlen(arg2);
		int spaces=has_spaces(arg2);

		if (i)
		{
			*p++=' ';
		}

		if (spaces) 
		{
			*p++='\"';
		}

		if (len)
		{
			memcpy(p,arg2,len);
			p+=len;
		}

		if (spaces) 
		{
			*p++='\"';
		}

		*p=0;

		i++;
	}

#ifdef _DEBUG
	fprintf(stdout,"xargs: %s\n",args);
	fflush(stdout);
#endif

	i=system(args);

#ifdef _DEBUG
	if (i)
	{
		fprintf(stderr,"xargs: error %d: %s\n",i,args);
		fflush(stderr);
	}
#endif

	free(args);

	return i;
}

#define MAX_ARGNUM	32

static char *dup_string(char *p)
{
	char *q=0;

	if (p)
	{
		size_t i=strlen(p)+1;
		q=malloc(i);
		if (q)
		{	
			memcpy(q,p,i);
		}
	}

	return q;
}

#define countOf(x)      (sizeof(x)/sizeof(x[0]))

int main(int argc,char **argv)
{
	char *argList[MAX_ARGNUM];
	int maxVars=countOf(argList)-1;
	int numArgs=0;
	int i=1;
	int rc=0;

	while (i < argc)
	{
		char *p=argv[i];

		if ((p[0]=='-')||(p[0]=='/'))
		{
			i++;
			p++;

#if defined(_WIN32) || defined(__OS2__)
			if (!stricmp(p,"n"))
#else
			if (!strcmp(p,"n"))
#endif
			{
				p=argv[i++];

				maxVars=atol(p);

				if (maxVars < 1)
				{
					maxVars=1;
				}
			}
			else
			{
				fprintf(stderr,"%s: unknown arg \"%s\"\n",argv[0],p-1);

				return 1;
			}
		}
		else
		{
			break;
		}
	}

	while (i < argc)
	{
		argList[numArgs++]=argv[i++];

		if (numArgs == countOf(argList))
		{
			fprintf(stderr,"%s: too many command args\n",argv[0]);

			rc=1;

			break;
		}
	}

	if (numArgs && !rc)
	{
		char buf[256];
		int numVars=0;

		i=countOf(argList)-numArgs;

		if (maxVars > i)
		{
			maxVars=i;
		}

		while (fgets(buf,sizeof(buf),stdin))
		{
			char *p=buf+strlen(buf);

			while (p > buf)
			{
				p--;
				if (*p <= ' ')
				{
					*p=0;
				}
				else
				{
					break;
				}
			}

			if (buf[0]) 
			{
				argList[numArgs+numVars]=dup_string(buf);

				if (argList[numArgs+numVars])
				{
					numVars++;
				}
				else
				{
					rc=2;

					break;
				}
			}

			if (numVars == maxVars)
			{
				rc=do_command(numArgs+numVars,argList);

				while (numVars)
				{
					char *p=argList[numArgs+(--numVars)];
					free(p);
				}

				if (rc) break;
			}
		}

		if (numVars && !rc)
		{
			rc=do_command(numArgs+numVars,argList);
			
			while (numVars)
			{
				char *p=argList[numArgs+(--numVars)];
				free(p);
			}
		}
	}

	return rc;
}

