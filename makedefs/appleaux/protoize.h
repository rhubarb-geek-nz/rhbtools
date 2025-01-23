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

/***********************************
 * Custom header for A/UX 3.1.1
 *
 * must work with no other includes
 */

#ifdef __cplusplus
extern "C" {
#endif

struct timeval;
struct timezone;
struct sockaddr;
struct sigstack;
struct fd_set;
struct ether_addr;
struct stat;

int getcompat(void);
int setcompat(int);
int setpgrp(int,int);

int ether_hostton(char *,struct ether_addr *);
int vfork(void);
int bzero(char *,int);

int select(int,struct fd_set *,struct fd_set *,struct fd_set *,struct timeval *);
int recv(int,char *,int,int);
int send(int,const char *,int,int);
int accept(int,struct sockaddr *,int *);
int socketpair(int,int,int,int *);
int getsockname(int,struct sockaddr *,int *);
int getpeername(int,struct sockaddr *,int *);
int connect(int,struct sockaddr *,int);
int gethostname(char *,int);
int bind(int,struct sockaddr *,int);
int listen(int,int);
int socket(int,int,int);
int setsockopt(int,int,int,char *,int);
int getsockopt(int,int,int,char *,int*);
int sendto(int,const char *,int,int,const struct sockaddr *,int);
int recvfrom(int,const char *,int,int,struct sockaddr *,int *);
int ioctl(int,int,...);
int sigstack(struct sigstack*,struct sigstack *);
int slot_ether_addr(int,void *);
int gettimeofday(struct timeval *,struct timezone *);
int strcasecmp(const char *,const char *);
int shutdown(int,int);
int setgroups(int,int*);
void openlog(const char *,int,int);
void syslog(int,const char *,...);
void closelog(void);
int setlogmask(int);
int flock(int,int);
int ftruncate(int,int);
int getopt(int,char *const*,const char *);
int setsid(void);
int stat(const char *,struct stat *);
int lstat(const char *,struct stat *);
void perror(const char *);

extern char *optarg;
extern int optind, opterr;

#ifdef __cplusplus
}
#endif
