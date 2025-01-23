/*************************************************************
 *
 * ptyexec - run a child process with a terminal
 *
 * hat tip to W Richard Stevens - 
 *            UNIX Network Programming section 15.6
 *
 * Copyright(C) Roger Brown, 2008
 * All Rights Reserved
 *
 * things were simple until they say use "setsid" instead,
 * and Linux, which is neither BSD or System V, but a hybrid
 */

/* 
 * $Id: ptyexec.c 16 2021-05-06 23:39:34Z rhubarb-geek-nz $
 */


#include <rhbopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <string.h>

#ifdef HAVE_TERMIOS_H
#	include <termios.h>
#endif

#ifdef HAVE_STRINGS_H
#	include <strings.h>
#endif

static char *pty_name;

#ifdef __STDC__
#	define USE_ANSI_C
#endif

static int set_nonblocking
#ifdef USE_ANSI_C
	(int fd)
#else
	(fd) int fd;
#endif
{
#ifdef HAVE_FCNTL_F_SETFL_O_NDELAY
	int i=fcntl(fd,F_SETFL,O_NDELAY);
	if (i < 0)
	{
		perror("F_SETFL,O_NDELAY");
	}
#else
	int ul=1;
	int i=ioctl(fd,FIONBIO,(void *)&ul);
	if (i)
	{
		perror("FIONBIO");
	}
#endif
	return i;
}

#if 0
#	define TRACE(x)     { char *_x=x; fprintf(stderr,"%s\n",_x); fflush(stderr); }
#else
#	define TRACE(x)
#endif

static int pty_master
#ifdef USE_ANSI_C
	(void)
#else
	()
#endif
{
	int master_fd=-1;
#ifdef HAVE__GETPTY
	pty_name=_getpty(&master_fd,O_RDWR|O_NDELAY,0600,0);
#else
#	if defined(HAVE_POSIX_OPENPT)&&defined(HAVE_PTSNAME)
TRACE("posix_openpt")
		master_fd=posix_openpt(O_RDWR);
TRACE("posix_openpt returned")
		if (master_fd!=-1)
		{
TRACE("grantpt")
			grantpt(master_fd);
TRACE("unlockpt")
			unlockpt(master_fd);
TRACE("ptsname")
			pty_name=ptsname(master_fd);
TRACE("ptsname returned")
		}
#	else
#		ifdef HAVE_PTSNAME

TRACE("open /dev/ptym/clone")
	master_fd=open("/dev/ptym/clone",O_RDWR);

	if (master_fd!=-1)
	{
TRACE("ptsname")
    	pty_name=ptsname(master_fd); 
	}
	else
	{
#		ifdef __linux__
TRACE("open /dev/ptmx")
		master_fd=open("/dev/ptmx",O_RDWR);
TRACE("open returned")
#		endif

		if (master_fd!=-1)
		{
TRACE("grantpt")
			if (grantpt(master_fd)<0) perror("grantpt");
TRACE("unlockpt")
			if (unlockpt(master_fd)<0) perror("unlockpt");
TRACE("ptsname")
	      	pty_name=ptsname(master_fd); 
		}
	}

#		endif
	if (master_fd==-1)
	{
		static char ptychar[]="pqrs";
		char *ptr=ptychar;
		static char pty_name_buf[30];

		strcpy(pty_name_buf,"/dev/ptyXY");
TRACE("search begin loop")
	
		while ((*ptr)&&(master_fd==-1))
		{
			struct stat statbuff;
			static char hexdigit[]="0123456789abcdef";
			char *ptx=hexdigit;

TRACE("search iteration")
			pty_name_buf[8]=*ptr++;
			pty_name_buf[9]=*ptx;

TRACE("search stat")
			if (stat(pty_name_buf,&statbuff) < 0) break;
	
			while (*ptx)
			{
				pty_name_buf[9]=*ptx++;

TRACE("search open")
				if ((master_fd=open(pty_name_buf,O_RDWR))>=0)
				{
TRACE("search found")
					pty_name_buf[5]='t';

					pty_name=pty_name_buf;
	
					break;
				}
TRACE("search done")
			}
		}
	}
#	endif
#endif

	if (master_fd!=-1)
	{
		TRACE("open master non-block")

		set_nonblocking(master_fd);
	}

	TRACE("open master complete")

	return master_fd;
}

static int pty_slave
#ifdef USE_ANSI_C
	(int master_fd)
#else
	(master_fd) int master_fd;
#endif
{
	int slave_fd=-1;

	if ((slave_fd=open(pty_name,O_RDWR))<0)
	{
		char buf[256];

		snprintf(buf,sizeof(buf),"open(%s)",pty_name);

		perror(buf);

		close(master_fd);

		return -1;
	}

	return slave_fd;
}

static int handler_fd=-1;

#ifdef USE_ANSI_C
	static void handler(int i)
#else
	static void handler(i) int i;
#endif
{
	if (handler_fd != -1)
	{
		char buf[1];
		int rc;
		buf[0]=(char)i;
		rc=write(handler_fd,buf,sizeof(buf));
		if (rc)
		{
			/* -Werror=unused-but-set-variable */
		}
	}
}

struct buffer
{
     int len,off;
     int fd_read,fd_write;
     char buf[16];
};

static int room
#ifdef USE_ANSI_C
	(struct buffer *buf)
#else
	(buf)
	struct buffer *buf;
#endif
{
	if (buf->fd_read < 0) return -1;

	return sizeof(buf->buf) - buf->off - buf->len;
}

static int do_read
#ifdef USE_ANSI_C
	(struct buffer *buf)
#else
	(buf)
	struct buffer *buf;
#endif
{
	int r=room(buf);

/*	printf("do_read(%d)\n",r); fflush(stdout);*/

	if (r > 0)
	{
		r=read(buf->fd_read,buf->buf+buf->off,r);

		if (r > 0)
		{
			buf->len+=r;
		}
		else
		{
			if (r < 0)
			{
				int e=errno;

/*				perror("do_read");*/

				if (e!=EWOULDBLOCK)
				{
					buf->fd_read=-1;
				}
			}
			else
			{
/*				printf("EOF\n"); fflush(stdout);*/

				buf->fd_read=-1;
			}
		}
	}
	
	return r;
}

static int do_write
#ifdef USE_ANSI_C
	(struct buffer *buf)
#else
	(buf)
	struct buffer *buf;
#endif
{
	int r=0;

	if (buf->len)
	{
/*		printf("do_write(%d)\n",buf->len); fflush(stdout); */

		r=write(buf->fd_write,buf->buf+buf->off,buf->len);

		if (r > 0)
		{
			buf->off+=r;
			buf->len-=r;

			if (!buf->len) 
			{
				buf->off=0;
			}
		}
	
		if (r < 0)
		{
			perror("do_write");
		}
	}

	return r;
}

int main
#ifdef USE_ANSI_C
	(int argc,char **argv)
#else
	(argc,argv)
	int argc;
	char **argv;
#endif
{
	pid_t pid=-1;
	int master=-1;
	int rc=1;
	int pipes[2];

	pipes[0]=-1;
	pipes[1]=-1;

	if (pipe(pipes))
	{
		perror("pipe");
		return 1;
	}

#if 0
	if (set_nonblocking(0) < 0)
	{
		return 1;
	}

	if (set_nonblocking(1) < 0)
	{
		return 1;
	}
#endif

	master=pty_master();

	if (master < 0) 
	{
		fprintf(stderr,"failed to alloc pty\n");

		return 1;
	}

/*	fprintf(stderr,"pty_master returned \"%s\"\n",pty_name); */

#ifdef SIGCHLD
	signal(SIGCHLD,handler);
#else
	signal(SIGCLD,handler);
#endif

	handler_fd=pipes[1];

	pid=fork();

	if (pid)
	{
		int status=0;
		struct buffer inp,outp;

		inp.len=0;
		inp.off=0;
		inp.fd_read=0;
		inp.fd_write=master;

		outp.len=0;
		outp.off=0;
		outp.fd_read=master;
		outp.fd_write=1;

		while (inp.len ||
			   outp.len || 
				(inp.fd_read != -1) ||
				(outp.fd_read != -1))
		{
			fd_set fdr,fdw,fde;
			int i=master;

			FD_ZERO(&fdr);
			FD_ZERO(&fdw);
			FD_ZERO(&fde);

			if (inp.len && (inp.fd_write!=-1)) { FD_SET(inp.fd_write,&fdw); }
			if (outp.len && (outp.fd_write!=-1)) { FD_SET(outp.fd_write,&fdw); }

			if (room(&inp)>0) { FD_SET(inp.fd_read,&fdr); }
			if (room(&outp)>0) { FD_SET(outp.fd_read,&fdr); }
			
			FD_SET(pipes[0],&fdr);

			if (pipes[0]>i) { i=pipes[0]; }

			FD_SET(master,&fde);

			TRACE("select");

			i=select(i+1,&fdr,&fdw,&fde,NULL);

			if (i > 0)
			{
				if (FD_ISSET(master,&fde))
				{
					TRACE("exception on master");
				}

				if ((inp.fd_write!=-1) && FD_ISSET(inp.fd_write,&fdw))
				{
					if (do_write(&inp) < 0) break;

					continue;
				}

				if ((outp.fd_write!=-1) && FD_ISSET(outp.fd_write,&fdw))
				{
					if (do_write(&outp)<0) break;

					continue;
				}

				if ((inp.fd_read >=0) && FD_ISSET(inp.fd_read,&fdr))
				{
					do_read(&inp);

					continue;
				}

				if ((outp.fd_read >=0) && FD_ISSET(outp.fd_read,&fdr))
				{
					do_read(&outp);

					continue;
				}

				if (FD_ISSET(pipes[0],&fdr))
				{
					char sig[1];
					int rc;
					TRACE("signal handler");
					rc=read(pipes[0],sig,sizeof(sig));

					if (rc) 
					{
						/* -Werror=unused-but-set-variable */
					}

					break;
				}
			}
			else
			{
				if (errno != EINTR)
				{
					perror("select");

					break;
				}
			}
		}

		if (master!=-1)
		{
			close(master);
			master=-1;
		}

		TRACE("out of loop");

		waitpid(pid,&status,0);

		pid=-1;

		TRACE("waitpid returned");

		if (WIFEXITED(status))
		{
			TRACE("WIFEXITED");

			rc=WEXITSTATUS(status);
		}
	}
	else
	{
#ifdef SIGCHLD
		signal(SIGCHLD,SIG_DFL);
#else
		signal(SIGCLD,SIG_DFL);
#endif
		handler_fd=-1;
		close(pipes[0]);
		close(pipes[1]);

		{
			/* child */
			int slave=-1;

			TRACE("setsid")

			if (setsid() < 0)
			{
				perror("ptyexec: setsid");

				_exit(1);
			}

			slave=pty_slave(master);

			if (slave < 0) 
			{
				_exit(1);
			}

			TRACE("about to duplicate the slave to stdio");

			if (dup2(slave,0)==-1) { perror("dup stdin") ; _exit(1); }
			if (dup2(slave,1)==-1) { perror("dup stdout"); _exit(1); }
			if (dup2(slave,2)==-1) { perror("dup stderr") ; _exit(1); }

			close(slave); 
			close(master);

			master=-1;
			slave=-1;

/*			printf("[%d] on %s\n",(int)getpid(),ttyname(0));
			fflush(stdout);
*/

#ifdef TIOCSCTTY
			/**************************
			 * Required for BSD, set the controlling terminal 
			 */

			{
				int gtty=-1;

				/* 
					SunOS already sets the controlling terminal
					so check to see if it was set
				*/

				if (ioctl(0,TIOCGPGRP,(char *)&gtty)<0)
				{
					gtty=-1;
				}

				if (gtty != getpid())
				{
					/* it's not been set to us so claim it */
					if (ioctl(0,TIOCSCTTY,NULL) < 0)
					{
						perror("ptyexec: ioctl TIOCSCTTY");
						_exit(1);
					}
				}
			}

#endif /* TIOCSCTTY */

			if (argc==1)
			{
				char *argv2[2];
				char *p=getenv("SHELL");
				if (!p) p="/bin/sh";
				argv2[0]=p;
				argv2[1]=NULL;
				execvp(p,argv2);
				perror("execvp SHELL");
			}
			else
			{
				execvp(argv[1],argv+1);
				perror("execvp");
			}
		}

		_exit(1);
	}

	return rc;
}
