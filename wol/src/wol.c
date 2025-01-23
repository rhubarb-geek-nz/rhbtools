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
 * $Id: wol.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

#include <rhbopt.h>

#if defined(_WIN32)
#	include <windows.h>
#	ifdef HAVE_WINSOCK2_H
#		include <winsock2.h>
#	else
#		ifdef HAVE_WINSOCK_H
#			include <winsock.h>
#		endif
#	endif
#	ifdef HAVE_WS2IP6_H
#		include <ws2ip6.h>
#	endif
#	ifdef HAVE_WS2TCPIP_H
#		include <ws2tcpip.h>
#	endif
#else
#	include <sys/types.h>
#	include <sys/time.h>
#	include <sys/socket.h>
#	ifdef HAVE_SYS_SELECT_H
#		include <sys/select.h>
#	endif
#	include <netinet/in.h>
#	include <netdb.h>
#	include <arpa/inet.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_STRINGS_H
#	include <strings.h>
#endif

#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif

#ifndef INVALID_SOCKET
	typedef int SOCKET;
#	define INVALID_SOCKET   ((SOCKET)-1)
#endif

#ifndef HAVE_SOCKLEN_T
#	define HAVE_SOCKLEN_T
	typedef int socklen_t;
#endif

static int hexchar(char c)
{
	if ((c >= '0')&&(c <='9'))
	{
		return c-'0';
	}

	if (c > 'Z') c-=32;

	if ((c >= 'A')&&(c<='F'))
	{
		return c-='7';
	}

	return -1;
}

#if 0
#	if USE_300GL
			char *ip_addr="10.1.2.15";
			char *enet_addr="00-e0-4c-0a-3f-a8";
#	else
#		ifdef USE_COMPAQ_EPSB
			char *ip_addr="10.1.2.71";
			char *enet_addr="00-e0-4c-0a-3f-45";
#	else
#		ifdef USE_KIDSBOX
			char *ip_addr="10.1.2.40";
			char *enet_addr="00-10-dc-42-8c-cc";
#		else
/*			char *ip_addr="10.1.2.30"; */
			char *ip_addr="10.1.2.255";
			char *enet_addr="00-e0-4c-44-06-b7";
#		endif
#	endif
#endif
#endif

int main(int argc,char **argv)
{
char enet_binary[6];
#if defined(_WIN32)
WSADATA wsd;
#endif
int rc=1;
char *ip_addr="255.255.255.255";
char *enet_addr=0;

	if (argc > 1)
	{
		int i=1;

		while (i < argc)
		{
			char *p=argv[i++];

			if ((*p=='-')||(*p=='/'))
			{
				switch (p[1])
				{
				case 'e':
					enet_addr=argv[i++];
					break;
				case 'i':
					ip_addr=argv[i++];
					break;
				default:
					fprintf(stderr,"%s: unknown arg \"%s\"\n",argv[0],p);

					return 1;
				}
			}
			else
			{
				fprintf(stderr,"%s: unknown arg \"%s\"\n",argv[0],p);

				return 1;
			}
		}
	}

	if (!enet_addr)
	{
		fprintf(stderr,"%s: no ethernet addr defined\n",argv[0]);

		return 1;
	}

#if defined(_WIN32)
	if (WSAStartup(0x101,&wsd)) return 1;
#endif

	{
		char *p=enet_binary;
		const char *q=enet_addr;
		int i=sizeof(enet_binary);

		while (*q && i--)
		{
			int j=0;
			char v=0;

			while (j < 2)
			{
				char c=*q++;
				int h=hexchar(c);
				if (c==0) { i=0; break; } 
				if (h==-1) break;
				v<<=4;
				v|=h;
				j++;
			}

			*p++=v;

			while (*q && (hexchar(*q) < 0))
			{
				q++;
			}
		}
	}

	{
		SOCKET fd=socket(AF_INET,SOCK_DGRAM,0);

		if (fd!=INVALID_SOCKET)
		{
			int b=1;

			if (setsockopt(fd,SOL_SOCKET,SO_BROADCAST,(char *)&b,sizeof(b)))
			{
				perror("setsockopt");
			}
			else
			{
				struct servent *sp=getservbyname("echo","udp");
				struct sockaddr_in addr;

				memset(&addr,0,sizeof(addr));
				addr.sin_family=AF_INET;

				if (bind(fd,(struct sockaddr *)&addr,sizeof(addr)))
				{
					perror("bind");
				}
				else
				{
					char packet[17*sizeof(enet_binary)];
					int packlen=0;
					int i=16;
					struct hostent *hp=gethostbyname(ip_addr);
	
					memset(&addr,0,sizeof(addr));

					addr.sin_family=AF_INET;
					addr.sin_port=sp ? sp->s_port : htons(7);

					if (hp && (hp->h_length==sizeof(addr.sin_addr)))
					{
						memcpy(&addr.sin_addr,
							hp->h_addr_list[0],
							hp->h_length);
					}
					else
					{
						addr.sin_addr.s_addr=inet_addr(ip_addr);
					}

					memset(packet+packlen,0xff,sizeof(enet_binary)); 
				
					packlen+=sizeof(enet_binary);

					while (i--)
					{
						memcpy(packet+packlen,
							enet_binary,sizeof(enet_binary)); 
						packlen+=sizeof(enet_binary);
					}

					i=4;

					while (i--)
					{
						struct timeval tv={0,0};
						fd_set fdr;
						int k;

						FD_ZERO(&fdr);
						FD_SET(fd,&fdr);

						k=sendto(fd,packet,packlen,0,(struct sockaddr *)&addr,sizeof(addr));

						if (k < 0) 
						{
							perror("sendto");

							break;
						}

						rc=0;

						tv.tv_sec=1;

						k=select((int)(fd+1),&fdr,NULL,NULL,&tv);

						if (k > 0)
						{
							if (FD_ISSET(fd,&fdr))
							{
								struct sockaddr_in addr2;
								socklen_t j=sizeof(addr2);
								char buf[1024];

								memset(&addr2,0,sizeof(addr2));

								k=recvfrom(fd,
									buf,sizeof(buf),0,
									(struct sockaddr *)&addr2,&j);

								if ((k > 0)&&(addr2.sin_family==addr.sin_family))
								{
									printf("reply from %s:%d\n",
											inet_ntoa(addr2.sin_addr),
											ntohs(addr2.sin_port));

									hp=gethostbyaddr((char *)&addr2.sin_addr,
													sizeof(addr2.sin_addr),
													addr2.sin_family);

									if (hp && hp->h_name && hp->h_name[0])
									{
										printf("%s\n",hp->h_name);
									}

									if (!memcmp(&addr2.sin_addr,
											&addr.sin_addr,
											sizeof(addr.sin_addr)))
									{
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}

#if defined(_WIN32)
	WSACleanup();
#endif

	return rc;
}
