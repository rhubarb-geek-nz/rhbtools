/**************************************************************************
 *
 *  Copyright 2008, Roger Brown
 *
 *  This file is part of Roger Brown's Toolkit.
 *
 *  Roger Brown's Toolkit is free software: you can redistribute it and/or modify
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
 * $Id: socket.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

#include <rhbopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
#	include <strings.h>
#endif

#ifdef _WIN32
#	ifdef HAVE_WINSOCK2_H
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

#ifndef SD_SEND
#	define SD_RECEIVE      0x00
#	define SD_SEND         0x01
#	define SD_BOTH         0x02
#endif

#ifndef INADDR_NONE
#	define INADDR_NONE    (0xffffffffU)
#endif

#if defined(_WIN32) && defined(USE_THREADS) && !defined(USE_PTHREADS)
#	include <sockpair.h>
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

static HANDLE hThreadStdout,hThreadStdin;

static DWORD CALLBACK do_stdin(void *pv)
{
	SOCKET fd=(SOCKET)pv;
	HANDLE hStdin=GetStdHandle(STD_INPUT_HANDLE);

	while (1)
	{
		char buf[256];
		DWORD dw;
		if (ReadFile(hStdin,buf,sizeof(buf),&dw,NULL))
		{
			char *p=buf;
			if (!dw) break;

			while (dw)
			{
				int i=send(fd,p,dw,0);

				if (i > 0)
				{
					p+=i;
					dw-=i;
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			break;
		}
	}

	shutdown(fd,SD_SEND);

	closesocket(fd);

	CloseHandle(hStdin);

	SetStdHandle(STD_INPUT_HANDLE,INVALID_HANDLE_VALUE);

	return 0;
}

static DWORD CALLBACK do_stdout(void *pv)
{
	SOCKET fd=(SOCKET)pv;
	HANDLE hStdout=GetStdHandle(STD_OUTPUT_HANDLE);

	while (1)
	{
		char buf[256];
		int i=recv(fd,buf,sizeof(buf),0);

		if (i > 0)
		{
			DWORD dw;
			WriteFile(hStdout,buf,i,&dw,NULL);
		}
		else
		{
			break;
		}
	}

	closesocket(fd);

	CloseHandle(hStdout);

	SetStdHandle(STD_OUTPUT_HANDLE,INVALID_HANDLE_VALUE);

	return 0;
}

static SOCKET get_stdin(void)
{
	SOCKET pair[2];
	DWORD tid;

	if (socketpair(AF_UNIX,SOCK_STREAM,0,pair) < 0) return INVALID_SOCKET;

	shutdown(pair[0],SD_SEND);
	shutdown(pair[1],SD_RECEIVE);

	hThreadStdout=CreateThread(NULL,0,do_stdin,(void *)pair[1],0,&tid);

	return pair[0];
}

static SOCKET get_stdout(void)
{
	SOCKET pair[2];
	DWORD tid;

	if (socketpair(AF_UNIX,SOCK_STREAM,0,pair) < 0) return INVALID_SOCKET;

	shutdown(pair[0],SD_SEND);
	shutdown(pair[1],SD_RECEIVE);

	hThreadStdout=CreateThread(NULL,0,do_stdout,(void *)pair[0],0,&tid);

	return pair[1];
}
#else
#	define get_stdin()    (0)
#	define get_stdout()    (1)
#endif

struct stream
{
	char buffer[4096];
	int length;
	SOCKET fd_write;
	SOCKET fd_read;
};

#define IS_ACTIVE(s)     (((s)->fd_write!=INVALID_SOCKET)||((s)->fd_read!=INVALID_SOCKET))

#ifndef _WIN32
#	define send(a,b,c,d)			write(a,b,c)
#	define recv(a,b,c,d)			read(a,b,c)
#endif

static int socket_opts(SOCKET fd)
{
#ifdef _WIN32
	BOOL ul=1;
	int i=ioctlsocket(fd,FIONBIO,(unsigned long *)&ul);
#else
	int ul=1;
#	ifdef HAVE_FCNTL_F_SETFL_O_NDELAY
	int i=fcntl(fd,F_SETFL,O_NDELAY);
	if (i < 0)
	{
		perror("F_SETFL,O_NDELAY");
	}
#	else
	int i=ioctl(fd,FIONBIO,(void *)&ul);
	if (i)
	{
		perror("FIONBIO");
	}
#	endif
#endif

	setsockopt(fd,IPPROTO_TCP,
		TCP_NODELAY,(char *)&ul,sizeof(ul));

	return i;
}

int main(argc,argv)
int argc;
char **argv;
{
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

	memset(&addr,0,sizeof(addr));

	if (*(argv[2])>='a')
	{
		struct servent *sp=getservbyname(argv[2],"tcp");
		if (!sp) 
		{
			fprintf(stderr,"unknown service \"%s\"\n",argv[2]);
			return 1;
		}
		addr.sin_port=sp->s_port;
	}
	else
	{
		addr.sin_port=(short)atol(argv[2]);
		addr.sin_port=htons(addr.sin_port);
	}

	addr.sin_family=AF_INET;

	addr.sin_addr.s_addr=inet_addr(argv[1]);

	if (addr.sin_addr.s_addr==INADDR_NONE)
	{
		struct hostent *hp=gethostbyname(argv[1]);
		if (hp)
		{
			if (hp->h_length==sizeof(addr.sin_addr))
			{
				memcpy(&addr.sin_addr,
					hp->h_addr_list[0],sizeof(addr.sin_addr));
			}
		}
	}

/*	fprintf(stderr,"addr=%s\n",inet_ntoa(addr.sin_addr));*/

	fd=socket(AF_INET,SOCK_STREAM,0);

	if (fd!=INVALID_SOCKET)
	{
		if (connect(fd,(struct sockaddr *)&addr,sizeof(addr)))
		{
			perror("connect");
		}
		else
		{
			SOCKET fd_stdin=get_stdin();
			SOCKET fd_stdout=get_stdout();
			struct stream streams[2];

			socket_opts(fd);
			socket_opts(fd_stdin);
			socket_opts(fd_stdout);

			rc=0;

			memset(streams,0,sizeof(streams));

			streams[0].fd_read=fd_stdin;
			streams[0].fd_write=fd;

			streams[1].fd_read=fd;
			streams[1].fd_write=fd_stdout;

			while (IS_ACTIVE(streams) || IS_ACTIVE(streams+1))
			{
				int k;
				SOCKET n=INVALID_SOCKET;
				fd_set fds_read,fds_write;

				FD_ZERO(&fds_read);
				FD_ZERO(&fds_write);

				k=sizeof(streams)/sizeof(streams[0]);

				while (k--)
				{
					if (streams[k].fd_read != INVALID_SOCKET)
					{
						if (streams[k].length < sizeof(streams[k].buffer))
						{
							FD_SET(streams[k].fd_read,&fds_read);

							if (streams[k].fd_read > n) n=streams[k].fd_read;
						}
					}

					if (streams[k].fd_write != INVALID_SOCKET)
					{
						if (streams[k].length)
						{
							FD_SET(streams[k].fd_write,&fds_write);

							if (streams[k].fd_write > n) n=streams[k].fd_write;
						}
					}
				}

				if (n==INVALID_SOCKET) 
				{
					break;
				}

				if (select((int)(n+1),&fds_read,&fds_write,NULL,NULL) > 0)
				{
					k=sizeof(streams)/sizeof(streams[0]);

					while (k--)
					{
						if (streams[k].fd_read != INVALID_SOCKET)
						{
							if ((streams[k].length < sizeof(streams[k].buffer)) 
								&& FD_ISSET(streams[k].fd_read,&fds_read))
							{
								int i=recv(streams[k].fd_read,
									streams[k].buffer+streams[k].length,
									sizeof(streams[k].buffer)-streams[k].length,0);

								if (i > 0)
								{
									streams[k].length+=i;
								}
								else
								{
									if (i < 0)
									{
#ifdef _WIN32
										int e=WSAGetLastError();
#else
										int e=errno;
#endif
										switch (e)
										{
#ifdef _WIN32
										case WSAEWOULDBLOCK:
#else
										case EWOULDBLOCK:
#endif
											break;
										default:
											i=0;
											break;
										}
									}

									if (!i)
									{
										if (streams[k].fd_read!=fd)
										{
											closesocket(streams[k].fd_read);
										}

										streams[k].fd_read=INVALID_SOCKET;

										if (!streams[k].length)
										{
											shutdown(streams[k].fd_write,SD_SEND);

											if (streams[k].fd_write!=fd)
											{
												closesocket(streams[k].fd_write);
											}

											streams[k].fd_write=INVALID_SOCKET;
										}
									}
								}
							}
						}

						if (streams[k].fd_write != INVALID_SOCKET)
						{
							if (streams[k].length 
								&& FD_ISSET(streams[k].fd_write,&fds_write))
							{
								int i=send(streams[k].fd_write,
										   streams[k].buffer,
										   streams[k].length,
										   0);

								if (i > 0)
								{
									streams[k].length-=i;

									if (streams[k].length)
									{
#ifdef HAVE_MEMMOVE
										memmove(
											streams[k].buffer,
											streams[k].buffer+i,
											streams[k].length);
#else
										char *dst=streams[k].buffer;
										const char *src=streams[k].buffer+i;
										unsigned long len=streams[k].length;
										while (len--)
										{
											*dst++=*src++;
										}
#endif
									}
									else
									{
										if (streams[k].fd_read==INVALID_SOCKET)
										{
											if (streams[k].fd_write==fd)
											{
												shutdown(streams[k].fd_write,SD_SEND);
											}
											else
											{
												closesocket(streams[k].fd_write);
											}

											streams[k].fd_write=INVALID_SOCKET;
										}
									}
								}
								else
								{
									if (streams[k].fd_write!=fd)
									{
										closesocket(streams[k].fd_write);
									}

									streams[k].fd_write=INVALID_SOCKET;
									streams[k].length=0;

									if (streams[k].fd_read!=INVALID_SOCKET)
									{
										if (streams[k].fd_read==fd)
										{
											shutdown(streams[k].fd_read,SD_RECEIVE);
										}
										else
										{
											closesocket(streams[k].fd_read);
										}

										streams[k].fd_read=INVALID_SOCKET;
									}
								}
							}
						}
					}
				}
				else
				{
					break;
				}
			}
		}

		closesocket(fd);
	}

#ifdef _WIN32
	if (hThreadStdout) WaitForSingleObject(hThreadStdout,INFINITE);
	if (hThreadStdin) WaitForSingleObject(hThreadStdin,INFINITE);

	WSACleanup();
#endif

	return rc;
}
