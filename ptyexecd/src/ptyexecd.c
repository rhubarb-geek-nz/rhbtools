/*************************************************************
 *
 * pty2exec - run a child process on master side of pseudo terminal
 *
 * hat tip to W Richard Stevens - 
 *            UNIX Network Programming section 15.6
 *
 * Copyright(C) Roger Brown, 2008
 * All Rights Reserved
 *
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

#ifdef HAVE_SIGINFO_H
#	include <siginfo.h>
#endif

#ifdef HAVE_TERMIOS_H
#	include <termios.h>
#endif

#ifdef HAVE_STRINGS_H
#	include <strings.h>
#endif

#ifdef HAVE__GETPTY
static char *pty_name;
#else
static char pty_name[12];
#endif

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
		_exit(1);
	}
#else
	int ul=1;
	int i=ioctl(fd,FIONBIO,(void *)&ul);
	if (i)
	{
		perror("FIONBIO");
		_exit(1);
	}
#endif
	return i;
}

#if 1
#	define TRACE(x)     { char *_x=x; printf("%s\n",_x); fflush(stdout); }
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
	static char ptychar[]="pqrs";
	char *ptr=ptychar;;
	strcpy(pty_name,"/dev/ptyXY");
	
	while ((*ptr)&&(master_fd==-1))
	{
		struct stat statbuff;
		static char hexdigit[]="0123456789abcdef";
		char *ptx=hexdigit;

		pty_name[8]=*ptr++;
		pty_name[9]=*ptx;

		if (stat(pty_name,&statbuff) < 0) break;

		while (*ptx)
		{
			pty_name[9]=*ptx++;

			if ((master_fd=open(pty_name,O_RDWR))>=0)
			{
				pty_name[5]='t';

				break;
			}
		}
	}
#endif

	set_nonblocking(master_fd);

	return master_fd;
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
		int x;
/*		char str[256];
		int x;*/
		buf[0]=(char)i;
/*		x=sprintf(str,"signal(%d)\n",i);
		write(2,str,x);*/
		x=write(handler_fd,buf,sizeof(buf));
		if (x)
		{
		}
	}
}

struct buffer
{
	const char *read_status;
	const char *write_status;
	int len,off;
	int fd_read,fd_write;
	char buf[256];
};

static void dump_buf(const char *name,
					struct buffer *buf)
{
	printf("%s: fd_read=%d,fd_write=%d,len=%d,off=%d\n",
			name,
			buf->fd_read,
			buf->fd_write,
			buf->len,
			buf->off);
	fflush(stdout);
}

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

	printf("do_read(%s,%d)\n",
		buf->read_status,
		r); fflush(stdout);

	if (r > 0)
	{
		r=read(buf->fd_read,buf->buf+buf->off,r);

		if (r > 0)
		{
			printf("do_read returned %d\n",r);
			fflush(stdout);

			buf->len+=r;
		}
		else
		{
			if (r < 0)
			{
				int e=errno;

				if (e!=EWOULDBLOCK)
				{
					char str[256];
					strncpy(str,"read ",sizeof(str)-1);
					strncat(str,buf->read_status,sizeof(str)-1);
					perror(str);
					buf->fd_read=-1;
				}
				else
				{
					printf("read EWOULDBLOCK\n"); fflush(stdout);
				}
			}
			else
			{
				printf("read EOF\n"); fflush(stdout);

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

	if (buf->len && (buf->fd_write!=-1))
	{
		printf("do_write(%s,%d)\n",
				buf->write_status,
				buf->len); fflush(stdout); 

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
			int e=errno;

			if ((e==EWOULDBLOCK)||(e==EAGAIN))
			{
				TRACE("EWOULDBLOCK");
			
				r=0;
			}
			else
			{
				char str[256];
				strncpy(str,"write ",sizeof(str)-1);
				strncat(str,buf->write_status,sizeof(str)-1);
				perror(str);

				buf->fd_write=-1;
			}
		}
		else
		{
			printf("do_write(%s) returned %d\n",
					buf->write_status,r); fflush(stdout); 

			if (!r)
			{
				r=write(buf->fd_write,buf->buf+buf->off,1);

				if (r >= 0)
				{
					printf("do_write 2nd attempt returned %d\n",r);
					buf->len-=r;
					buf->off+=r;
					if (!buf->len) buf->off=0;
				}
				else
				{
					int e=errno;

					if ((e==EWOULDBLOCK)||(e==EAGAIN))
					{
						r=0;
						TRACE("EWOULDBLOCK");
					}
					else
					{
						char str[256];
						strncpy(str,"write ",sizeof(str)-1);
						strncat(str,buf->write_status,sizeof(str)-1);
						perror(str);
					}
				}
			}
		}
	}
	else
	{
		if (buf->len && (buf->fd_write < 0))
		{
			printf("do_write(%s,%d) - discard\n",
				buf->write_status,
				buf->len); fflush(stdout);

			buf->off=0;
			buf->len=0;
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
	int fd_master=-1;
	int rc=1;
	int pipe_signal[2];
	int pipe_stdin[2];
	int pipe_stdout[2];

	pipe_signal[0]=-1;
	pipe_signal[1]=-1;
	pipe_stdin[0]=-1;
	pipe_stdin[1]=-1;
	pipe_stdout[0]=-1;
	pipe_stdout[1]=-1;

	if (pipe(pipe_signal))
	{
		perror("pipe signal");
		return 1;
	}

	if (pipe(pipe_stdin))
	{
		perror("pipe stdin");
		return 1;
	}

	if (pipe(pipe_stdout))
	{
		perror("pipe stdout");
		return 1;
	}

	fd_master=pty_master();

	if (fd_master < 0) 
	{
		fprintf(stderr,"failed to alloc pty\n");

		return 1;
	}

/*	fprintf(stderr,"pty_master returned \"%s\"\n",pty_name); */

	handler_fd=pipe_signal[1];

	{
		static struct sigaction sa;
		static int siglist[]={
				SIGCHLD,
				SIGINT,
				SIGQUIT,
				SIGINT,
				SIGTERM,
				SIGPIPE,
				SIGHUP,
				SIGIO};
		int i=sizeof(siglist)/sizeof(siglist[0]);
		while (i--)
		{
#ifdef SA_RESTART
			sa.sa_flags=SA_RESTART;
#endif
			sa.sa_handler=handler;

			if (sigaction(siglist[i],&sa,NULL)<0)
			{
				perror("sigaction");

				return 1;
			}
		}
	}

	pid=fork();

	if (pid)
	{
		int status=0;
		struct buffer inp,outp;

		close(pipe_stdin[0]); pipe_stdin[0]=-1;
		close(pipe_stdout[1]); pipe_stdout[1]=-1;

		set_nonblocking(pipe_stdout[0]);
		set_nonblocking(pipe_stdin[1]);

		outp.read_status="stdout";
		outp.write_status=pty_name;

		outp.len=0;
		outp.off=0;
		outp.fd_read=pipe_stdout[0];
		outp.fd_write=fd_master;

		inp.read_status=pty_name;
		inp.write_status="stdin";

		inp.len=0;
		inp.off=0;
		inp.fd_read=fd_master;
		inp.fd_write=pipe_stdin[1];

		while (inp.len ||
			   outp.len || 
				(inp.fd_read != -1) ||
				(outp.fd_read != -1))
		{
			fd_set fdr,fdw,fde;
			int i=fd_master;
			struct timeval tv;

			tv.tv_usec=0;
			tv.tv_sec=10;

			FD_ZERO(&fdr);
			FD_ZERO(&fdw);
			FD_ZERO(&fde);

			dump_buf("inp",&inp);
			dump_buf("outp",&outp);

			printf("select, pid=%d, ",(int)pid);

			if (inp.len && (inp.fd_write!=-1)) 
			{ 
				FD_SET(inp.fd_write,&fdw); 
				if (inp.fd_write > i) i=inp.fd_write;
				printf("[IW]");
			}

			if (outp.len && (outp.fd_write!=-1)) 
			{ 
				FD_SET(outp.fd_write,&fdw); 
				if (outp.fd_write > i) i=outp.fd_write;
				printf("[OW]");
			}

			if ((inp.fd_read!=-1) && (room(&inp)>0))
			{ 
				FD_SET(inp.fd_read,&fdr); 
				if (inp.fd_read > i) i=inp.fd_read;
				printf("[IR]");
			}

			if ((outp.fd_read!=-1) && (room(&outp)>0)) 
			{ 
				FD_SET(outp.fd_read,&fdr); 
				if (outp.fd_read > i) i=outp.fd_read;
				printf("[OR]");
			}
			
			FD_SET(pipe_signal[0],&fdr);

			if (pipe_signal[0]>i) 
			{ 
				i=pipe_signal[0]; 
			}

			FD_SET(fd_master,&fde);

			printf(", i=%d\n",i);

			TRACE("select");

			i=select(i+1,&fdr,&fdw,&fde,&tv);

			if (i > 0)
			{
				if (FD_ISSET(pipe_signal[0],&fdr))
				{
					char sig[1];
					TRACE("signal handler");

					if (sizeof(sig)==read(pipe_signal[0],sig,sizeof(sig)))
					{
						int s=sig[0];

#ifdef HAVE_PSIGNAL
						psignal(s,"signal");
#else
						fprintf(stderr,"signal: %d\n",s);
						fflush(stderr);
#endif

				
						if (s==SIGCHLD)
						{
							while (pid != -1)
							{
								pid_t w=waitpid(pid,&status,WNOHANG);

								if (w==-1)
								{
									int e=errno;

									if (e!=EINTR)
									{
										perror("waitpid");

										pid=-1;
									}
								}
								else
								{
									pid=-1;
								}
							}
						}

						if ((s==SIGQUIT)||(s==SIGTERM))
						{
							break;
						}

						continue;
					}
				}

				if (FD_ISSET(fd_master,&fde))
				{
					TRACE("exception on master");
				}

				if ((inp.fd_write!=-1) && (FD_ISSET(inp.fd_write,&fdw)))
				{
					TRACE("inp.fd_write");
					if (do_write(&inp) < 0) break;

					if ((inp.fd_read==-1)&&
						(inp.fd_write!=-1))
					{
						TRACE("closing stdin");
						inp.fd_write=-1;
						close(pipe_stdin[1]);
						pipe_stdin[1]=-1;
					}

					continue;
				}

				if ((outp.fd_write!=-1) && (FD_ISSET(outp.fd_write,&fdw)))
				{
					TRACE("outp.fd_write");
					if (do_write(&outp) < 0) break;

					continue;
				}

				if ((inp.fd_read !=-1) && (FD_ISSET(inp.fd_read,&fdr)))
				{
					TRACE("inp.fd_read");

					if (do_read(&inp) < 0)
					{
						if (inp.fd_read==-1) break;
					}

					if ((inp.fd_read==-1)&&
						(inp.fd_write!=-1))
					{
						TRACE("closing stdin");
						inp.fd_write=-1;
						close(pipe_stdin[1]);
						pipe_stdin[1]=-1;
					}

					continue;
				}

				if ((outp.fd_read!=-1) && FD_ISSET(outp.fd_read,&fdr))
				{
					TRACE("outp.fd_read");

					if (do_read(&outp) < 0)
					{
						if (outp.fd_read==-1) break;
					}

					continue;
				}

				TRACE("select unknown");
			}
			else
			{
				if (i < 0)
				{
					if (errno != EINTR)
					{
						perror("select");

						break;
					}
					else
					{
						TRACE("select EINTR");
					}
				}
				else
				{	
					TRACE("select timeout");
				}
			}
		}

		TRACE("out of loop");

		dump_buf("inp",&inp);
		dump_buf("outp",&outp);

		if (fd_master!=-1)
		{
			close(fd_master);
			fd_master=-1;
		}

		close(pipe_stdin[1]);
		close(pipe_stdout[0]);

		while (pid != -1)
		{
			pid_t w=waitpid(pid,&status,0);

			if (w==-1)
			{
				int e=errno;

				if (e!=EINTR)
				{
					perror("waitpid");
					pid=-1;
				}
			}
			else
			{
				pid=-1;
			}
		}

		TRACE("waitpid returned");

		if (WIFEXITED(status))
		{
			TRACE("WIFEXITED");

			rc=WEXITSTATUS(status);
		}
	}
	else
	{
		char **args=(char **)malloc(sizeof(*argv)*(argc+1));
		int i=0;

#ifdef SIGCHLD
		signal(SIGCHLD,SIG_DFL);
#else
		signal(SIGCLD,SIG_DFL);
#endif
		handler_fd=-1;

		close(pipe_signal[0]);
		close(pipe_signal[1]);
		close(pipe_stdin[1]);
		close(pipe_stdout[0]);
		close(fd_master);

		while ((i+1) < argc)
		{
			args[i]=argv[i+1];

			i++;
		}

		args[i++]=pty_name;
		args[i++]=0;

#if 0
		i=0;

		while (args[i])
		{
			fprintf(stderr,"\"%s\"",args[i]);

			i++;

			if (args[i]) fprintf(stderr,",");
		}

		fprintf(stderr,"\n");
#endif

		if (dup2(pipe_stdin[0],0)!=0)
		{
			perror("dup2(pipe_stdin[0],0)");

			_exit(1);
		}

		if (dup2(pipe_stdout[1],1)!=1)
		{
			perror("dup2(pipe_stdout[1],1)");

			_exit(1);
		}

		if (dup2(pipe_stdout[1],2)!=2)
		{
			perror("dup2(pipe_stdout[1],2)");

			_exit(1);
		}

		close(pipe_stdout[1]);
		close(pipe_stdin[0]);

		execvp(args[0],args);

		perror("execvp");

		_exit(1);
	}

	return rc;
}
