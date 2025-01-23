#include <rhbopt.h>
#include <windows.h>
#include <winsock.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pipeexec.h>

#ifndef SD_SEND
#	define SD_RECEIVE      0x00
#	define SD_SEND         0x01
#	define SD_BOTH         0x02
#endif

void hexdump(FILE *fp,const void *pv,size_t len)
{
#ifdef _DEBUG
	if (len)
	{
		const unsigned char *op=pv;

		while (len--)
		{
			int x=*op++;
			fprintf(fp,"%02X ",x);
		}

		fprintf(fp,"\n");
	}
#endif
}

struct connection
{
	HANDLE hProcess;
	SOCKET fdInet;
	SOCKET fdExec;
	LONG usage;
};

static void release(struct connection *c)
{
	if (!InterlockedDecrement(&c->usage))
	{
		closesocket(c->fdInet);
		closesocket(c->fdExec);
		WaitForSingleObject(c->hProcess,INFINITE);
		CloseHandle(c->hProcess);
		free(c);
	}
}

static int sure_send(SOCKET fd,const void *pv,size_t len)
{
	const char *p=pv;
	int retVal=0;

	while (len)
	{
		int j=send(fd,p,(int)len,0);

		if (j > 0)
		{
			p+=j;
			retVal+=j;
			len-=j;
		}
		else
		{
			if (j < 0)
			{
				DWORD d=WSAGetLastError();

				if (d==WSAEWOULDBLOCK)
				{
					fd_set fds;
					FD_ZERO(&fds);
					FD_SET(fd,&fds);

					select(0/*fd+1*/,NULL,&fds,NULL,NULL);
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

	return retVal;
}

static DWORD CALLBACK do_stdin(void *pv)
{
	struct connection *c=pv;

	while (1)
	{
		char buf[4096];
		int i=recv(c->fdInet,buf,sizeof(buf),0);

		if (!i) break;

		if (i < 0)
		{
			DWORD d=WSAGetLastError();
			
			if (d==WSAEWOULDBLOCK)
			{
				fd_set fds;
				FD_ZERO(&fds);
				FD_SET(c->fdInet,&fds);
				select(0 /* c->fdInet+1*/,&fds,NULL,NULL,NULL);
			}
			else
			{
				break;
			}
		}
		else
		{
			hexdump(stderr,buf,i);

			i=sure_send(c->fdExec,buf,i);

			if (i < 1)
			{
				break;
			}
		}
	}

#ifdef _DEBUG
	fprintf(stderr,"shutdown(stdin)\n");
#endif

	shutdown(c->fdExec,SD_SEND);

	release(c);

	return 0;
}

static DWORD CALLBACK do_stdout(void *pv)
{
	struct connection *c=pv;

	while (1)
	{
		char buf[4096];
		int i=recv(c->fdExec,buf,sizeof(buf),0);

		if (!i) break;

		if (i < 0)
		{
			DWORD d=WSAGetLastError();
			
			if (d==WSAEWOULDBLOCK)
			{
				fd_set fds;
				FD_ZERO(&fds);
				FD_SET(c->fdExec,&fds);
				select(0/* c->fdExec+1*/,&fds,NULL,NULL,NULL);
			}
			else
			{
				break;
			}
		}
		else
		{
			hexdump(stderr,buf,i);

			i=sure_send(c->fdInet,buf,i);

			if (i < 1)
			{
				break;
			}
		}
	}

#ifdef _DEBUG
	fprintf(stderr,"shutdown(stdout)\n");
#endif

	shutdown(c->fdInet,SD_SEND);

	release(c);

	return 0;
}

int main(int argc,char **argv)
{
	WSADATA wsd;
	unsigned short port;
	SOCKET fd_listen;
	struct sockaddr_in addr;

	if (WSAStartup(0x101,&wsd)) return 1;

	if (argc < 3)
	{
		fprintf(stderr,"%s: port cmd...\n",argv[0]);

		return 2;
	}

	port=(unsigned short)atol(argv[1]);

	fd_listen=socket(AF_INET,SOCK_STREAM,0);

	if (fd_listen==INVALID_SOCKET)
	{
		return 3;
	}

	memset(&addr,0,sizeof(addr));
	addr.sin_port=htons(port);
	addr.sin_family=AF_INET;

	if (bind(fd_listen,(struct sockaddr *)&addr,sizeof(addr)))
	{
		return 4;
	}

	if (listen(fd_listen,5))
	{
		return 5;
	}

	while (1)
	{
		int addrlen=sizeof(addr);
		SOCKET fdAccept=accept(fd_listen,(struct sockaddr *)&addr,&addrlen);

		if (fdAccept!=INVALID_SOCKET)
		{
			HANDLE h=INVALID_HANDLE_VALUE;
			SOCKET fdChild=pipeexec(argc-2,argv+2,&h);

			if (fdChild!=INVALID_SOCKET)
			{
				struct connection *c=malloc(sizeof(*c));
				DWORD tid;

				c->usage=2;
				c->hProcess=h;
				c->fdExec=fdChild;
				c->fdInet=fdAccept;

				h=CreateThread(NULL,0,do_stdin,c,0,&tid); CloseHandle(h);
				h=CreateThread(NULL,0,do_stdout,c,0,&tid); CloseHandle(h);

			}
			else
			{
				closesocket(fdAccept);
			}
		}
	}

	return 0;
}
