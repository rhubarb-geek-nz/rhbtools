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
 * $Id: socket.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

#include <windows.h>
#include <winsock.h>

#define SD_RECEIVE	0
#define SD_SEND		1
#define SD_BOTH		2

static DWORD CALLBACK do_stdout(LPVOID pv)
{
	HANDLE hStdout=GetStdHandle(STD_OUTPUT_HANDLE);
	SOCKET fd=(SOCKET)pv;

	while (1)
	{
		BYTE buf[256];
		int j=recv(fd,buf,sizeof(buf),0);

		if (j > 0)
		{
			int i=0;

			while (i < j)
			{
				DWORD dw;

				if (WriteFile(hStdout,buf+i,j-i,&dw,NULL))
				{
					i+=dw;

					if (dw==0)
					{
						j=-1;

						break;
					}
				}
				else
				{
					j=-1;

					break;
				}
			}
		}

		if (j <= 0) break;
	}

	shutdown(fd,SD_RECEIVE);

	return 0;
}

int main(int argc,char **argv)
{
	WSADATA wsd;
	int rc=1;

	if (WSAStartup(0x101,&wsd)) return 10;

	if (argc > 2)
	{
		char *host=argv[1];
		char *port=argv[2];
		struct hostent *hp=gethostbyname(host);
		struct servent *sp=getservbyname(port,"tcp");
		struct sockaddr_in addr;
		int addrlen=sizeof(addr);
		SOCKET fd;

		if (!hp) return 2;
		if (!sp) return 3;

		memset(&addr,0,sizeof(addr));
		addr.sin_family=AF_INET;
		memcpy(&addr.sin_addr,hp->h_addr_list[0],hp->h_length);
		memcpy(&addr.sin_port,&(sp->s_port),sizeof(addr.sin_port));


		fd=socket(AF_INET,SOCK_STREAM,0);

		if (fd==INVALID_SOCKET) return 4;

		rc=5;

		if (!connect(fd,(struct sockaddr *)&addr,addrlen))
		{
			DWORD tid;
			HANDLE h=CreateThread(NULL,0,do_stdout,(LPVOID)fd,0,&tid);
			BYTE buf[256];
			HANDLE hStdin=GetStdHandle(STD_INPUT_HANDLE);
			DWORD dw;

			while (ReadFile(hStdin,buf,sizeof(buf),&dw,NULL))
			{
				unsigned int i=0;

				while (i < dw)
				{
					int j=send(fd,buf+i,dw-i,0);
					if (j < 0)
					{
						dw=0;
					}
					else
					{
						i+=j;
					}
				}

				if (!dw) break;
			}
			
			shutdown(fd,SD_SEND);

			WaitForSingleObject(h,INFINITE);

			rc=0;
		}

		closesocket(fd);
	}

	WSACleanup();

	return rc;
}
