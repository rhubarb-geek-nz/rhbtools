/**************************************************************************
 *
 *  Copyright 2008, Roger Brown
 *
 *  This file is part of Browser6.
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
 * $Id: textconv.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

#include <rhbopt.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#	include <windows.h>
#endif

static int getChar(void)
{
#ifdef _WIN32
	static DWORD length,offset;
	static unsigned char data[512];

	if (offset==length)
	{
		if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE),
			data,sizeof(data),&length,NULL))
		{
			return -1;
		}

		offset=0;

		if (!length) return -1;
	}

	return data[offset++];
#else
	return getchar();
#endif
}

int main(int argc,char **argv)
{
const char *output=NULL;
const char *terminator="\n";
const char *mode="w";
int i=1;
FILE *fpOut=stdout;
char lastChar=0;

	while (i < argc)
	{
		const char *p=argv[i++];

		if (*p!='-')
		{
			fprintf(stderr,"error with \"%s\"\n",p);

			return 1;
		}

		p++;

		if (!strcmp(p,"cr"))
		{
			terminator="\r";
		}
		else
		{
			if (!strcmp(p,"crlf"))
			{
				terminator="\r\n";
			}
			else
			{
				if (!strcmp(p,"lf"))
				{
					terminator="\n";
				}
				else
				{
					if (!strcmp(p,"o"))
					{
						output=argv[i++];
					}
					else
					{
						if (!strcmp(p,"mode"))
						{
							mode=argv[i++];
						}
						else
						{
							fprintf(stderr,"error with \"%s\"\n",p-1);

							return 1;
						}
					}
				}
			}
		}
	}

	if (output)
	{
		fpOut=fopen(output,mode);

		if (!fpOut)
		{
			perror(output);

			return 1;
		}
	}

	while (1)
	{
		int c=getChar();

		if (c==EOF) 
		{
			break;
		}

		switch (c)
		{
		case '\r':
			if (lastChar=='\r')
			{
				fprintf(fpOut,"%s",terminator);
			}
			break;

		case '\n':
			fprintf(fpOut,"%s",terminator);
			break;

		default:
			if (lastChar=='\r')
			{
				fprintf(fpOut,"%s",terminator);
			}
			fprintf(fpOut,"%c",c);

			break;
		}

		lastChar=c;
	}

	if (lastChar=='\r')
	{
		fprintf(fpOut,"%s",terminator);
	}

	return 0;
}
