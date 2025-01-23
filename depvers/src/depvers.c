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
 * $Id: depvers.c 6 2020-06-06 01:48:03Z rhubarb-geek-nz $
 */

#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#	if !defined(popen) && !defined(pclose)
#		define popen(x,y)			_popen(x,y)
#		define pclose(x)			_pclose(x)
#	endif
#	if !defined(timezone)
		static long myGetTimeZone(void) { long x=0; _get_timezone(&x); return x; }
#		define timezone	myGetTimeZone()
#	endif
#endif

static const char *depvers_type;
static const char *depvers_guid;

static const char *month[]={
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

struct reference
{
	struct reference *next;
	const char *name;
	struct makedefs_cf *makedefs;
};

struct makedefs_cf
{
	struct makedefs_cf *next;
	const char *name;
	const char *filename;
	long depends_count;					/* many others depend on this */
	struct reference *depends;			/* list of components this depends on */
	struct reference *provides;			/* list of what this provides */
	long revision;						/* from SVN */
	long effective_revision;			/* once take dependencies into account */
	const char *version;
	const char *guid;
	const char *description;
	const char *product;
	const char *msi;
	time_t date,effective_date;
	int outputFlag;
};

static int iswhitespace(int c)
{
	return ((c==' ')||(c=='\t')||(c=='\r')||(c=='\n'));
}

static char *striplf(char *p)
{
	char *q=p+strlen(p);

	while (q > p)
	{
		char c=*--q;
		if ((c=='\r')||(c=='\n'))
		{
			*q=0;
		}
		else
		{
			break;
		}
	}

	return p;
}

static struct makedefs_cf *makedefs;

static struct makedefs_cf *provides(const char *name)
{
	struct makedefs_cf *result=NULL;
	struct makedefs_cf *p=makedefs;

	while (p && !result)
	{
		struct reference *d=p->provides;

		while (d)
		{
			if (!strcmp(d->name,name))
			{
				result=p;

				break;
			}

			d=d->next;
		}

		p=p->next;
	}

	return result;
}

static void linkdef(struct makedefs_cf *def)
{
	struct reference *r=def->depends;
	
	while (r)
	{
		r->makedefs=provides(r->name);

		if (r->makedefs)
		{
			r->makedefs->depends_count++;
		}

		r=r->next;
	}
}

static void linkdefs(void)
{
	struct makedefs_cf *p=makedefs;

	while (p)
	{
		linkdef(p);

		p=p->next;
	}
}

static char *namevalue(char *buf)
{
	char *result=NULL;
	
	while (*buf)
	{
		char c=*buf;

		if (iswhitespace(c))
		{
			*buf++=0;

			while (*buf)
			{
				char c=*buf;

				if (iswhitespace(c))
				{
					buf++;
				}
				else
				{
					result=buf;

					break;
				}
			}

			break;
		}
		else
		{
			buf++;
		}
	}

	return result;
}

static struct reference *readRefs(char *p)
{
	struct reference *result=NULL;

	while (p && p[0])
	{
		char *q=namevalue(p);

		if (p[0])
		{
			struct reference *ref=calloc(sizeof(ref[0]),1);

			ref->name=strdup(p);
			ref->next=result;
			result=ref;

			p=q;
		}
		else
		{
			break;
		}
	}

	return result;
}

static void readMakedef(FILE *fp,char *filename)
{
	char buf[256];
	struct makedefs_cf *cf=calloc(sizeof(cf[0]),1);

	cf->filename=strdup(filename);

	while (fgets(buf,sizeof(buf),fp))
	{
		if (striplf(buf))
		{
			char *value=namevalue(buf);

			if (value)
			{
				if (!strcmp(buf,"NAME"))
				{
					cf->name=strdup(value);
				}
				if (!strcmp(buf,"VERSION"))
				{
					cf->version=strdup(value);
				}
				if (!strcmp(buf,"GUID"))
				{
					cf->guid=strdup(value);
				}
				if (!strcmp(buf,"MSI"))
				{
					cf->msi=strdup(value);
				}
				if (!strcmp(buf,"DESCRIPTION"))
				{
					cf->description=strdup(value);
				}
				if (!strcmp(buf,"PRODUCT"))
				{
					cf->product=strdup(value);
				}
				if (!strcmp(buf,"DEPENDS"))
				{
					cf->depends=readRefs(value);
				}
				if (!strcmp(buf,"PROVIDES"))
				{
					cf->provides=readRefs(value);
				}
			}
		}
	}

	if (cf->name)
	{
		cf->next=makedefs;
		makedefs=cf;
	}
	else
	{
		free(cf);
	}
}

static void readMakedefs(FILE *fp)
{
	char buf[512];
	while (fgets(buf,sizeof(buf),fp))
	{
		striplf(buf);
		if (buf[0])
		{
			FILE *fp2=fopen(buf,"r");

			if (!fp2)
			{
				perror(buf);
				exit(1);
			}

			readMakedef(fp2,buf);

			fclose(fp2);
		}
	}
}

static char *getWin32Def(struct makedefs_cf *def,const char *dirname,const char *name)
{
	char *result=NULL;
	char buf[1024];
	FILE *fp;
	_snprintf(buf,sizeof(buf),"%s/win32/%s.def",dirname,def->name);
	fp=fopen(buf,"r");
	if (fp)
	{
		while (fgets(buf,sizeof(buf),fp))
		{
			striplf(buf);
			if (buf[0])
			{
				char *p=namevalue(buf);
				if (p && !strcmp(buf,name))
				{
					result=strdup(p);
					break;
				}
			}
		}

		fclose(fp);
	}
	return result;
}

static time_t convertDate(const char *p)
{
	time_t n=0;
	char plusMinus=0;
	char buf[256]={0};
	int year=0,month=0,day=0,hour=0,minute=0,second=0,offset=0;
	int num=sscanf(p,"%d-%d-%d %d:%d:%d %c%d %s",
			&year,&month,&day,
			&hour,&minute,&second,
			&plusMinus,
			&offset,
			buf);

	if ((num >= 3)&&(year>=1970))
	{
		long tz=timezone;
		struct tm tm;

		memset(&tm,0,sizeof(tm));

		tm.tm_hour=hour;
		tm.tm_mday=day;
		tm.tm_year=(year-1900);
		tm.tm_mon=month-1;
		tm.tm_min=minute;
		tm.tm_sec=second;

		n=mktime(&tm);

		if (num > 6)
		{
			time_t off=(((offset /100)*60)+(offset % 100))*60;

			n-=timezone;

			switch (plusMinus)
			{
			case '+':
				n-=off;
				break;
			case '-':
				n+=off;
				break;
			}
		}

/*		fprintf(stderr,"in date=%s,tz=%ld, outdate=%s\n",p,tz,ctime(&n)); */
	}

	return n;
}

static void versionDef(struct makedefs_cf *def)
{
	if (!(def->revision))
	{
		char *dirname=strdup(def->filename);
		char *p=dirname+strlen(dirname);
		char buf[2048];
		FILE *fp;
#ifdef _WIN32
		char *svn=getenv("SVN_EXE");
#else
		char *svn=getenv("SVN");
#endif
		long n=0;

		if (!svn)
		{
#ifdef _WIN32
			svn="SVN.EXE";
#else
			svn="svn";
#endif
		}

		while (p > dirname)
		{
			if ((*p=='/')||(*p=='\\'))
			{
				*p=0;
				break;
			}
			p--;
		}

#ifdef _WIN32
		_snprintf(buf,sizeof(buf),"\"%s\" info %s",svn,dirname);
#else
		snprintf(buf,sizeof(buf),"%s info %s",svn,dirname);
#endif

		fp=popen(buf,"r");

		if (fp)
		{
			const char *lcRev="Last Changed Rev:";
			size_t lcRevLen=strlen(lcRev);
			const char *lcDate="Last Changed Date:";
			size_t lcDateLen=strlen(lcDate);

			while (fgets(buf,sizeof(buf),fp))
			{
				striplf(buf);
				if (buf[0])
				{
					size_t x=strlen(buf);

					if (x > lcRevLen)
					{
						if (!memcmp(buf,lcRev,lcRevLen))
						{
							const char *q=buf+lcRevLen;

							while (*q && iswhitespace(*q))
							{
								q++;
							}

							if (*q)
							{
								def->revision=atol(q);
							}
						}
					}

					if (x > lcDateLen)
					{
						if (!memcmp(buf,lcDate,lcDateLen))
						{
							const char *q=buf+lcDateLen;

							while (*q && iswhitespace(*q))
							{
								q++;
							}

							if (*q)
							{
								def->date=convertDate(q);
							}
						}
					}
				}
			}

			pclose(fp);
		}
		else
		{
			perror(buf);
			exit(1);
		}

		if (!(def->version))
		{
			def->version=getWin32Def(def,dirname,"VERSION");
		}

		free(dirname);

		if (def->depends)
		{
			struct reference *ref=def->depends;

			while (ref)
			{
				if (ref->makedefs)
				{
					versionDef(ref->makedefs);
				}
				else
				{
					fprintf(stderr,"depvers: %s does not have a resolved reference\n",ref->name);
					fflush(stderr);
					exit(1);
				}

				ref=ref->next;
			}
		}
	}
}

static time_t getDateDef(struct makedefs_cf *def)
{
	time_t date=def->effective_date;
	
	if (!date)
	{
		struct reference *p=def->depends;
		date=def->date;

		while (p)
		{
			time_t r=getDateDef(p->makedefs);

			if (r > date)
			{
				date=r;
			}

			p=p->next;
		}

		def->effective_date=date;
	}

	return date;
}

static long getVersionDef(struct makedefs_cf *def)
{
	long revision=def->effective_revision;
	
	if (!revision)
	{
		struct reference *p=def->depends;
		revision=def->revision;

		while (p)
		{
			long r=getVersionDef(p->makedefs);

			if (r > revision)
			{
				revision=r;
			}

			p=p->next;
		}

		def->effective_revision=revision;
	}

	return revision;
}

static void outputDef(struct makedefs_cf *def)
{
	if (!(def->outputFlag))
	{
		struct reference *ref=def->depends;

		def->outputFlag=1;

		if (!strcmp(depvers_type,"h"))
		{
			while (ref)
			{
				outputDef(ref->makedefs);

				ref=ref->next;
			}
		}

		if (def->version)
		{
			long vers=getVersionDef(def);
			int major=0,minor=0;
			int high=(int)(vers/10000);
			int low=(int)(vers%10000);

			if (sscanf(def->version,"%d.%d",&major,&minor)==2)
			{
				if (!strcmp(depvers_type,"h"))
				{
					printf("#define DEPVERS_%s_INT4 %d,%d,%d,%d\n",def->name,major,minor,high,low);
					printf("#define DEPVERS_%s_STR4 \"%d.%d.%d.%d\\000\\000\"\n",def->name,major,minor,high,low);
					if (def->product)
					{
						printf("#define DEPVERS_%s_PRODUCTNAME \"%s\\000\\000\"\n",def->name,def->product);
					}
				}
				if (!strcmp(depvers_type,"msi"))
				{
					const char *msi=def->msi ? def->msi : def->name;
					const char *product=def->product ? def->product : def->name;
					const char *desc=def->description ? def->description : def->name;
					const char *guid=depvers_guid ? depvers_guid : def->guid;
					time_t now=getDateDef(def);
					struct tm tm,*ptm;
					
					if (now==0)
					{
						time(&now);
					}

					ptm=localtime(&now);

					tm=*ptm;

					printf(";++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
					printf("; ProductName = %s\n",product);
					printf("; DESCRIPTION = %s\n",desc);
					printf("; Installed = WINDOWS_ALL\n");
					printf("; Guid.UpgradeCode = %s\n",guid);
					printf("; MsiName = %s-%d.%d.%d.%d\n",msi,major,minor,high,low);
					printf(";++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
					printf("\n");
					printf(";############################################################################\n");
					printf("VERSION : %d.%d.%d.%d\n",major,minor,high,low);
					printf("DATE : %d %s %d\n",tm.tm_mday,month[tm.tm_mon],tm.tm_year+1900);
					printf("CHANGES : distribution\n");
					printf("\n");
				}
			}
		}
	}
}

int main(int argc,char **argv)
{
	int i=1;
	char *name=NULL;
	FILE *input=NULL;
	input=stdin;

	tzset();

	while (i < argc)
	{
		const char *p=argv[i++];

		if (*p=='-')
		{
			if (!stricmp(p,"--input"))
			{
				const char *a=argv[i++];

				input=fopen(a,"r");

				if (!input)
				{
					perror(a);

					return 1;
				}
			}
			else
			{
				if (!strcmp(p,"--name"))
				{
					name=argv[i++];
				}
				else
				{
					if (!strcmp(p,"--type"))
					{
						depvers_type=argv[i++];
					}
					else
					{
						if (!strcmp(p,"--guid"))
						{
							depvers_guid=argv[i++];
						}
						else
						{
							fprintf(stderr,"unsupported argument \"%s\"\n",p);
							return 1;
						}
					}
				}
			}
		}
		else
		{
			fprintf(stderr,"unsupportef argument \"%s\"\n",p);
			return 1;
		}
	}

	readMakedefs(input);

	linkdefs();

	{
		struct makedefs_cf *p=makedefs;

		while (p)
		{
			if (name)
			{
				if (!strcmp(name,p->name))
				{
					versionDef(p);
				}
			}
			else
			{
				if (!(p->depends_count))
				{
					versionDef(p);
				}
			}

			p=p->next;
		}
	}

	{
		struct makedefs_cf *p=makedefs;

		while (p)
		{
			if (name)
			{
				if (!strcmp(name,p->name))
				{
					outputDef(p);
				}
			}
			else
			{
				if (!(p->depends_count))
				{
					outputDef(p);
				}
			}

			p=p->next;
		}
	}

	return 0;
}
