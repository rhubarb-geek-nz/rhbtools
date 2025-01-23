/**************************************************************************
 *
 *  Copyright 2012, Roger Brown
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
 * $Id: svninfo.c 16 2021-05-06 23:39:34Z rhubarb-geek-nz $
 */


/*****************
 *
 * read only access
 *
 * proplist
 * propget
 * info
 */

/**********************************************
 *
 * file format
 *
 * directory contains ".svn/entries"
 *

special character 0C, eject followed by newline

first entry							file entry

10
<blank>								filename
Node Kind: dir						Node Kind: file
Revision							<blank>
URL									<blank>
Repository Root						<blank>
<blank>								<blank>
<blank>								Text Last Updated:
<blank>								Checksum: 32 hex digits, md5
Last Change Date					Last Change Date
Last Change Rev						Last Change revision
Last Change Author					Last Changed Author
<blank>								has-props
<blank>								<blank>
<blank>
<blank>
<blank>
<blank>
<blank>
<blank>
<blank>
<blank>								copiedfromURL
<blank>								copiedFromRevision
<blank>
<blank>
<blank>
Repository UUID						<blank>	
EJECT								<blank>
									<blank>
									<blank>
									<blank>
									<blank>
									<blank>
									file length in bytes
									EJECT


properties files are key value where 
K namelen
name
V valuelen
value
END

properties for files are in .svn/prop-base/NAME.svn-base
properties for directories are in .svn/dir-prop-base

 could work out timezone using local minus GMT, also timegm/localtime

 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#ifdef _WIN32
#	include <io.h>
#	define OR_O_BINARY		| O_BINARY
#else
#	include <unistd.h>
#	define OR_O_BINARY
#endif

#define SVN_EOL				10
#define SVN_FF				12

static const char SVN_SIGNATURE[]={'1','0'};

#define SVN_ENTRY_NAME						0
#define SVN_ENTRY_NODE_KIND					1
#define SVN_ENTRY_REVISION					2
#define SVN_ENTRY_LAST_CHANGE_DATE			8
#define SVN_ENTRY_LAST_CHANGE_REVISION		9
#define SVN_ENTRY_LAST_CHANGE_AUTHOR		10

static int hasAny(const char *p,char c)
{
	while (*p)
	{
		if (*p++==c) return 1;
	}

	return 0;
}

static char *getDirname(const char *p)
{
	char *result=NULL;
	size_t i=strlen(p);
	int found=0;

	while ((i > 0)&&(!found))
	{
		char c=p[--i];

		if (c=='/') found=1;
#ifdef _WIN32
		if (c=='\\') found=1;
		if (c==':') found=1;
#endif
	}

	if (found)
	{
		result=malloc(i+1);
		if (i)
		{
			memcpy(result,p,i);
		}
		result[i]=0;
	}

	return result;
}

static const char *getBaseName(const char *p)
{
	size_t i=strlen(p);

	while (i)
	{
		int found=0;
		char c=p[--i];

		if (c=='/') found=1;
#ifdef _WIN32
		if (c=='\\') found=1;
		if (c==':') found=1;
#endif
		if (found)
		{
			i++;
			break;
		}
	}

	return p+i;
}

typedef struct entry
{
	struct entry *next;
	int lineCount;
	const char *line[1];
} entry;

typedef struct entries
{
	struct entry *first;
	size_t length;
	char data[1];
} entries;

static char svnEntries[]="/.svn/entries";

void freeEntries(entries *e)
{
	if (e)
	{
		while (e->first)
		{
			entry *p=e->first;

			e->first=p->next;

			free(p);
		}

		free(e);
	}
}

size_t linelength(const char *p,size_t m)
{
	int i=0;

	while ((m--)&&(p[i]!=SVN_EOL))
	{
		i++;
	}

	return i;
}

int countLines(const char *p,size_t m)
{
	int i=0;

	while (m)
	{
		if (*p==SVN_FF) 
		{
			break;
		}
		else
		{
			size_t len=linelength(p,m);

			if (len < m) len++;
			m-=(len);
			p+=(len);

			i++;
		}
	}

	return i;
}

void addEntry(entries *e,entry *f)
{
	f->next=NULL;

	if (e->first)
	{
		entry *p=e->first;
		while (p->next) { p=p->next; }
		p->next=f;
	}
	else
	{
		e->first=f;
	}
}

entry *findEntry(entries *e,const char *name)
{
	entry *p=e->first;

	while (p)
	{
		if (p->lineCount>=2)
		{
			if (!strcmp(p->line[0],name))
			{
				break;
			}
		}

		p=p->next;
	}

	return p;
}

entries *readEntries(const char *p)
{
size_t m=strlen(p);
size_t n=m+sizeof(svnEntries)+1;
char *path=malloc(n);
int fd;
entries *e=NULL;

	memcpy(path,p,m);
	memcpy(path+m,svnEntries,sizeof(svnEntries));
	path[m+sizeof(svnEntries)]=0;

	fd=open(path,O_RDONLY OR_O_BINARY);

	if (fd!=-1)
	{
		struct stat s;

		if (!fstat(fd,&s))
		{
			size_t len=s.st_size;

			if (len>8)
			{
				e=malloc(sizeof(*e)+len);

				e->first=NULL;
	
				e->length=read(fd,e->data,(unsigned int)len);
			}
		}

		close(fd);
	}

	if (e)
	{
		size_t i=linelength(e->data,e->length);
		size_t j=sizeof(SVN_SIGNATURE);

		if ((i==j)&&!memcmp(e->data,SVN_SIGNATURE,i))
		{
			size_t remainder=e->length-(i+1);
			char *data=e->data+(i+1);

			while (remainder)
			{
				int lineCount=countLines(data,remainder);

				if (lineCount)
				{
					int k=0;
					entry *f=malloc(
							sizeof(*f)
							+
							(sizeof(f->line[0])*lineCount)
							);

					f->next=NULL;
					f->lineCount=lineCount;

					while (k < lineCount)
					{
						size_t l=linelength(data,remainder);

						data[l++]=0;

						f->line[k++]=data;
						remainder-=l;
						data+=l;
					}

					addEntry(e,f);

					if ((linelength(data,remainder)==1)&&(SVN_FF==*data))
					{
						remainder--;
						data++;

						if (remainder && (SVN_EOL==*data))
						{
							remainder--;
							data++;
						}
					}
					else
					{
						fprintf(stderr,"file format error\n");
						exit(1);
					}
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			free(e);

			e=NULL;
		}
	}

	free(path);

	return e;
}

const char *getValue(entry *f,int num)
{
	if (num < f->lineCount) return f->line[num];

	return NULL;
}

static int readLine(int fd,char *buf,size_t len)
{
	int i=0;

	while (len--)
	{
		int k=read(fd,buf,1);

		if (k!=1) 
		{
			buf[0]=0;
			break;
		}

		if (buf[0]==SVN_EOL)
		{
			buf[0]=0;
			break;
		}

		buf++;
		i++;
	}

	return i;
}

static int doFile(const char *verb,const char *name,const char *file)
{
struct stat s;
int rc=0;
char *dirName=NULL;
entries *e=NULL;
entry *f=NULL;
/* char *fileType=NULL;*/

	int i=stat(file,&s);

	if (i)
	{
		perror(file);

		return 1;
	}

	switch (s.st_mode & S_IFMT)
	{
		case S_IFDIR:
			e=readEntries(file);
/*			fileType="dir";*/
			if (e)
			{
				f=findEntry(e,"");
			}
			break;
		case S_IFREG:
/*			fileType="file";*/
			if (hasAny(file,'/')
#ifdef _WIN32
				||hasAny(file,'\\')
#endif
				)
			{		
				dirName=getDirname(file);

				e=readEntries(dirName);

				if (e)
				{
					f=findEntry(e,getBaseName(file));
				}
			}
			else
			{
				e=readEntries(".");

				if (e)
				{
					f=findEntry(e,getBaseName(file));
				}
			}

			break;
		default:
			fprintf(stderr,"Unhandled file type for %s\n",file);
			rc=1;
			break;
	}

	if (e)
	{
		if (f)
		{
			if (!strcmp(verb,"info"))
			{
				const char *p;
				printf("Path: %s\n",file);
				p=getValue(f,SVN_ENTRY_NAME);
				if (p && p[0]) printf("Name: %s\n",p);
				p=getValue(f,SVN_ENTRY_REVISION);
				if (p && p[0]) printf("Revision: %s\n",p);
				printf("Node Kind: %s\n",getValue(f,SVN_ENTRY_NODE_KIND));
				p=getValue(f,SVN_ENTRY_LAST_CHANGE_AUTHOR);
				if (p && p[0]) printf("Last Changed Author: %s\n",p);
				p=getValue(f,SVN_ENTRY_LAST_CHANGE_REVISION);
				if (p && p[0]) printf("Last Changed Rev: %s\n",p);
				p=getValue(f,SVN_ENTRY_LAST_CHANGE_DATE);
				if (p && p[0]) printf("Last Changed Date: %s\n",p);
			}
			else if ((!strcmp(verb,"proplist"))||(!strcmp(verb,"propget")))
			{
				char *propFile=NULL;
				const char *p=getValue(f,SVN_ENTRY_NAME);
				int fd=-1;
				int doList=0;

				if (!strcmp(verb,"proplist"))
				{
					doList=1;
				}

				if (dirName && p && p[0])
				{
					const char *ext1="/.svn/prop-base/";
					const char *ext2=".svn-base";
					size_t i=strlen(dirName);
					size_t j=strlen(ext1);
					size_t k=strlen(p);
					size_t l=strlen(ext2);
					propFile=malloc(i+j+k+l+1);
					memcpy(propFile,dirName,i);
					memcpy(propFile+i,ext1,j);
					memcpy(propFile+i+j,p,k);
					memcpy(propFile+i+j+k,ext2,l+1);
				}
				else
				{
					const char *ext="/.svn/dir-prop-base";
					size_t i=strlen(file);
					size_t j=strlen(ext);
					propFile=malloc(i+j+1);
					memcpy(propFile,file,i);
					memcpy(propFile+i,ext,j+1);
				}

				fd=open(propFile,O_RDONLY OR_O_BINARY);

				free(propFile);

				propFile=NULL;

				if (fd!=1)
				{
					int matchProp=0;

					if (doList)
					{
						const char *p1="Properties on \'";
						const char *p2="\':\n";
						int w1=write(1,p1,(unsigned int)strlen(p1));
						int w2=write(1,file,(unsigned int)strlen(file));
						int w3=write(1,p2,(unsigned int)strlen(p2));
						if (w1 || w2 || w3)
						{
							/* -Werror=unused-but-set-variable */
						}
					}

					while (rc==0)
					{
						char buf[256];
						int i=readLine(fd,buf,sizeof(buf)-1);
						if (i <=0) break;
						buf[i]=0;

						if (!strcmp(buf,"END")) break;

						if (buf[0]=='K')
						{
							int len=0;
							if (sscanf(buf,"K %d",&len)==1)
							{
								i=0;
								if (len<sizeof(buf))
								{
									if (len)
									{
										i=read(fd,buf,len);

										if (i==len)
										{
											if (doList)
											{
												int w1=write(1,"  ",2);
												int w2=write(1,buf,len);
												int w3=write(1,"\n",1);
												if (w1 || w2 || w3)
												{
													/* -Werror=unused-but-set-variable */
												}
											}
											else
											{
												if ((i==(int)strlen(name))&&(!memcmp(name,buf,len)))
												{
													matchProp=1;
												}
											}
										}
									}

									readLine(fd,buf,1);
								}
								else
								{
									fprintf(stderr,"bad file format %s\n",buf);
									rc=5;
									break;
								}
							}
						}
						else
						{
							if (buf[0]=='V')
							{
								int len=0;
								if (sscanf(buf,"V %d",&len)==1)
								{
									if (len > 0)
									{
										char *data=malloc(len);
										int i=read(fd,data,len);

										if ((len==i)&&(matchProp))
										{
											int w1=write(1,data,len);

											if (w1)
											{
												/* -Werror=unused-but-set-variable */
											}

											matchProp=0;
										}

										free(data);
									}

									readLine(fd,buf,1);
								}
							}
						}
					}

					close(fd);
				}
			}
			else
			{
				rc=3;
			}
		}
		else
		{
			rc=2;
		}

		freeEntries(e);
	}
	else
	{
		fprintf(stderr,"Failed to read .svn/entries");

		rc=4;
	}

	if (dirName) 
	{
		free(dirName);
	}

	return rc;
}

int main(int argc,char **argv)
{
	const char *verb=NULL;
	const char *name=NULL;
	int i=1;
	int rc=0;
	int anyFiles=0;

	while ((i < argc)&&(!rc))
	{
		const char *p=argv[i++];

		if (*p=='-')
		{
		}
		else
		{
			if (verb)
			{
				if ((!name) && !strcmp(verb,"propget"))
				{
					name=p;
				}
				else
				{
					anyFiles=1;

					rc=doFile(verb,name,p);
				}
			}
			else
			{
				verb=p;
			}
		}
	}

	if (verb)
	{
		if (!anyFiles)
		{
			rc=doFile(verb,name,".");
		}
	}
	else
	{
		rc=1;
		fprintf(stderr,"Usage: %s info|proplist|propget [opts] file...\n",argv[0]);
	}

	return rc;
}
