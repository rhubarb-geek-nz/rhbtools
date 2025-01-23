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
 * $Id: hexdump.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <stdio.h>
#ifdef _WIN32
#	include <windows.h>
static HANDLE gHandle=INVALID_HANDLE_VALUE;
static int open(char *name,int m)
{
	gHandle=CreateFile(name,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);

	if (gHandle==INVALID_HANDLE_VALUE) return -1;

	return 3;
}
static int close(int x)
{
	if (x==3) 
	{
		CloseHandle(gHandle);
		gHandle=INVALID_HANDLE_VALUE;
	}
	return 0;
}
static int read(int fd,char *buf,int len)
{
	DWORD dw=len;
	HANDLE h=gHandle;

	if (fd==0) 
	{
		h=GetStdHandle(STD_INPUT_HANDLE);
	}

	if (ReadFile(h,buf,len,&dw,NULL))
	{
		return dw;
	}
	return -1;
}
#else
#	ifdef __OS2__
#		include <fcntl.h>
#	        include <io.h>
#	else
#		include <termios.h>
#		include <unistd.h>
#		include <fcntl.h>
#	endif
#endif

static int do_fd
#if defined(__STDC__) || defined(_MSC_VER)
(int fd)
#else
(fd)
int fd;
#endif
{
	char c[16];
	int k=0;
	while ((k=read(fd,c,16))>0)
	{
		int j=0;
		while (j < 16)
		{
			if (j < k)
			{
				printf("%02x ",0xff & (int)c[j]);
			}
			else
			{
				printf("   ");
			}

			j++;
		}
		j=0;
		while (j < k)
		{
			int o=0xff & (int)c[j];
			if ((o >= ' ') & (o < 0x7f))
			{
				putchar(o);
			}
			else
			{
				putchar('.');
			}
			j++;
		}
		printf("\n");
	}
	return 0;
}

static int do_name
#if defined(__STDC__) || defined(_MSC_VER)
(char *name)
#else
(name)
char *name;
#endif
{
	int fd=open(name,0
#ifdef __OS2__
		|O_BINARY
#endif
		);
	if (fd==-1)
	{
		perror(name); return 1;
	}

		do_fd(fd);
	close(fd);
        return 0;
}

#if defined(TCSANOW) && !defined(HAVE_CFMAKERAW)
static int cfmakeraw
#ifdef __STDC__
(struct termios *tt)
#else
(tt) struct termios *tt;
#endif
{
	tt->c_iflag=0;
	tt->c_oflag&=~OPOST;
	tt->c_lflag&=~(ISIG|ICANON|ECHO
#ifdef XCASE
			|XCASE
#endif
			);
	tt->c_cflag&=~(CSIZE|PARENB);
	tt->c_cflag|=CS8;
	tt->c_cc[VMIN]=1;
	tt->c_cc[VTIME]=1;
	return 0;
}
#endif

#if defined(_MSC_VER) || defined(__STDC__)
int main(int argc,char **argv)
#else
main(argc,argv)
int argc;
char **argv;
#endif
{
		if (argc > 1)
		{
			int i=1;
	
			while (i < argc)
		    {
		if (do_name(argv[i])) return 1;
                i++;
			}
        }
		else
		{
#ifdef TCSANOW
			struct termios t1,t2;
			if (isatty(0))
			{
				tcgetattr(0,&t1);
				tcgetattr(0,&t2);
				cfmakeraw(&t2);
				tcsetattr(0,TCSANOW,&t2);
			}
#endif
			do_fd(0);
#ifdef TCSANOW
			if (isatty(0))
			{
				tcsetattr(0,TCSANOW,&t1);
			}
#endif
		}

        return 0;
}
