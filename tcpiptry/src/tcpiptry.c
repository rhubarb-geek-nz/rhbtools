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
 * $Id: tcpiptry.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

 
/************************************************
 * Test to see if a TCP port is listening
 */

#include <rhbopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#	ifdef HAVE_WINSOCK2_
#		include <winsock2.h>
#	else
#		include <winsock.h>
#	endif
#else
#	include <sys/types.h>
#	include <sys/time.h>
#	include <sys/socket.h>
#	include <sys/ioctl.h>
#	include <netinet/in.h>
#	include <netinet/tcp.h>		/* for TCP_NODELAY */
#	include <arpa/inet.h>
#	include <netdb.h>
#	include <unistd.h>
#	include <fcntl.h>
#	include <errno.h>
typedef int SOCKET;
#	define INVALID_SOCKET   ((SOCKET)-1)
static int closesocket(SOCKET fd) { return close(fd); }
#endif

#ifndef INADDR_NONE
#	define INADDR_NONE    (0xffffffffU)
#endif

#ifdef _WIN32
static BOOL __stdcall console_handler(DWORD dw)
{
	switch (dw)
	{
	case CTRL_C_EVENT:
		return 1;
	case CTRL_BREAK_EVENT:
		return 1;
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
		return 1;
	case CTRL_SHUTDOWN_EVENT:
		return 1;
	}

	return 0;
}
#endif

static void parse_addr(struct sockaddr_in *addr,
					   const char *host,
					   const char *port)
{
	memset(addr,0,sizeof(*addr));

	addr->sin_family=AF_INET;

	if (port && port[0])
	{
		if (port[0]>='a')
		{
			struct servent *sp=getservbyname(port,"tcp");
			if (!sp) 
			{
				fprintf(stderr,"unknown service \"%s\"\n",port);
				exit(1);
			}
			addr->sin_port=sp->s_port;
		}
		else
		{
			addr->sin_port=(short)atol(port);
			addr->sin_port=htons((addr->sin_port));
		}
	}

	if (host && host[0])
	{
		addr->sin_addr.s_addr=inet_addr(host);

		if (addr->sin_addr.s_addr==INADDR_NONE)
		{
			struct hostent *hp=gethostbyname(host);

			if (hp && (hp->h_length==sizeof(addr->sin_addr)))
			{
				memcpy(&addr->sin_addr,
					hp->h_addr_list[0],sizeof(addr->sin_addr));
			}
			else
			{
				fprintf(stderr,"unknown host \"%s\"\n",host);

				exit(1);
			}
		}
	}
}

int main(argc,argv)
int argc;
char **argv;
{
	const char *bind_addr=NULL;
	const char *bind_port=NULL;
#ifdef _WIN32
	WSADATA wsd;
#endif
	struct sockaddr_in addr;
	SOCKET fd=INVALID_SOCKET;
	int rc=1;

#ifdef _WIN32
	SetConsoleCtrlHandler(console_handler,TRUE);
#endif

	if (argc<3) 
	{
		fprintf(stderr,"usage: %s host port\n",argv[0]);

		return 1;
	}

#ifdef _WIN32
	if (WSAStartup(0x101,&wsd)) return 1;
#endif

	if (argc > 3)
	{
		bind_addr=argv[3];
	}

	if (argc > 4)
	{
		bind_port=argv[4];
	}

	parse_addr(&addr,argv[1],argv[2]);

	fd=socket(AF_INET,SOCK_STREAM,0);

	if (fd==INVALID_SOCKET)
	{
		perror("socket");
	}
	else
	{
		if (bind_addr || bind_port)
		{
			struct sockaddr_in addr2;

			parse_addr(&addr2,bind_addr,bind_port);

			if (bind(fd,(struct sockaddr *)&addr2,sizeof(addr2)))
			{
				perror("bind");

				return 1;
			}
		}

		if (connect(fd,(struct sockaddr *)&addr,sizeof(addr)))
		{
			perror("connect");
		}
		else
		{
			rc=0;
		}

		closesocket(fd);
	}

#ifdef _WIN32
	WSACleanup();
#endif

	return rc;
}
