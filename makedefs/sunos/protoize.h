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
 * $Id: protoize.h 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

/**********************************************
 * Custom SunOS 4.1.* header file only
 *
 * must work with no include files at all
 */

#ifdef __cplusplus
extern "C" {
#endif

#define RAND_MAX   32767

#define __PROTOIZE_SIZE_T   unsigned int
#define __PROTOIZE_VA_LIST	char *

struct _iobuf;
struct fd_set;
struct timeval;
struct timezone;
struct sockaddr;
struct rlimit;
struct ether_addr;

int printf(const char *,...);

int fprintf(struct _iobuf *,const char *,...);
int fflush(struct _iobuf *);
int fclose(struct _iobuf *);
int fseek(struct _iobuf *,long,int);
int vfprintf(struct _iobuf *,const char *,__PROTOIZE_VA_LIST);
int setvbuf(struct _iobuf *,char *,int,__PROTOIZE_SIZE_T);

int _filbuf(struct _iobuf *);

__PROTOIZE_SIZE_T fread(void *,__PROTOIZE_SIZE_T,__PROTOIZE_SIZE_T,struct _iobuf *);
__PROTOIZE_SIZE_T fwrite(const void *,__PROTOIZE_SIZE_T,__PROTOIZE_SIZE_T,struct _iobuf *);
int fputc(int,struct _iobuf *);
int fgetc(struct _iobuf *);
int fscanf(struct _iobuf *,const char *,...);

void setbuf(struct _iobuf *,char *);
int ether_hostton(char *hostname, struct ether_addr *e);
void *memmove(void *,const void *,__PROTOIZE_SIZE_T);
int getopt(int,char **,char *);
extern char *optarg;
extern int optind,opterr;
int getrlimit(int,struct rlimit *);

char *sprintf(char *,const char *,...);
char *vsprintf(char *,const char *,__PROTOIZE_VA_LIST);
int sscanf(const char *,const char *,...);

void perror(const char *);
char *strerror(int);

int select(int,struct fd_set *,struct fd_set *,struct fd_set *,struct timeval *);
void bzero(char *,int);
int socketpair(int,int,int,int[2]);
int send(int,const void  *,int,int);
int recv(int,void  *,int,int);
int socket(int,int,int);
int ioctl(int,int,void *);
int bind(int,struct sockaddr *,int);
int listen(int,int);
int accept(int,struct sockaddr *,int *);
int connect(int,const struct sockaddr *,int);
int sendto(int,const void *,int,int,const struct sockaddr *,int);
int recvfrom(int,void *,int,int,struct sockaddr *,int *);
int shutdown(int,int);

int ftruncate(int,unsigned int); /* int,off_t */

int gettimeofday(struct timeval *,struct timezone *);
int vfork(void);
int fork(void);
int getpid(void);

long time(long *);

int getsockname(int,struct sockaddr *,int *);
int getpeername(int,struct sockaddr *,int *);
int gethostname(char *,int);
int getsockopt(int,int,int,char *,int*);
int setsockopt(int,int,int,char *,int);

int strcasecmp(const char *,const char *);

int openlog(const char *,int,int);
int syslog(int,const char *,...);
int closelog(void);
int setlogmask(int);
int system(const char *);

#undef __PROTOIZE_SIZE_T
#undef __PROTOIZE_VA_LIST

#ifdef __cplusplus
}
#endif
