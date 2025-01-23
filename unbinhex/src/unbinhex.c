/**************************************************************************
 *
 *  Copyright 2008, Roger Brown
 *
 *  This file is part of Browser6.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#	include <windows.h>
	typedef unsigned char Str255[256];
	typedef struct
	{
		Str255 name;
		short vRefNum;
		long parID;
	} FSSpec;
	typedef DWORD OSErr;
	#define fnfErr -43
	#define dupFNErr -48
	#define smSystemScript   0
	#define fsWrPerm 1
	OSErr FSMakeFSSpec(short,long,Str255,FSSpec *);
	OSErr FSpCreate(FSSpec *,long,long,long);
	OSErr FSClose(HANDLE);
	OSErr FSWrite(HANDLE,long *,void *);
	OSErr FSpOpenDF(FSSpec *,long,HANDLE *);
	OSErr FSpOpenRF(FSSpec *,long,HANDLE *);
	OSErr SetEOF(HANDLE,DWORD);
#else
	#include <Files.h>
	#include <Script.h>
	#include <Errors.h>
#endif

#ifdef _DEBUG
static int debugFlag;
#endif

typedef struct decoder
{
	unsigned short crc;
	FILE *fp;
	FSSpec fsp;
	unsigned short repeat_count;
	unsigned char last_octet,last_count;
	unsigned long shifter;
	unsigned short bit_count;
	unsigned long dlen;
	unsigned long rlen;
} decoder;

typedef unsigned char octet;
typedef char boolean;

static char *myfgets(char *buf,int len,FILE *fp)
{
char *p=buf;
int c=0;

	while (len--)
	{
		int ch=fgetc(fp);

		if (ch==EOF) break;

		if ((ch=='\n')||(ch=='\r'))
		{
			break;
		}

		*p++=ch;

		c++;
	}

	*p++=0;

	if (c) return buf;
	
	return NULL;
}


static char char_map[64]="!\042#$%&'()*+,-012345689@ABCDEFGHIJKLMNPQRSTUVXYZ[`abcdefhijklmpqr";

static boolean get_6(decoder *s,octet *cp)
{
	*cp=0;

	while (1)
	{
		int i=fgetc(s->fp);
		int j=0;

		if (i==EOF) 
		{
			return 0;
		}

		while (j < 64)
		{
			if (i==char_map[j]) 
			{
				*cp=j;

				return 1;
			}

			j++;
		}

		switch (i)
		{
		case 9:
		case 10:
		case 13:
		case 32:
			break;
		case ':':
			break;
		default:
			fprintf(stderr,"Illegal char %x,%d,'%c'\n",i,i,(char)i);
			break;
		}
	}

	return 0;
}

static boolean get_8_1(decoder *s,octet *cp)
{
	*cp=0;

	while (1)
	{
		unsigned char o=0;

		if (s->bit_count >= 8)
		{
			unsigned int j=s->shifter;
			s->bit_count-=8;
			j>>=s->bit_count;

			*cp=j;

#ifdef _DEBUG
			if (debugFlag)
			{
				fprintf(stderr,"%02X ",(unsigned char)j);
				fflush(stderr);
			}
#endif

			return 1;
		}

		if (!get_6(s,&o))
		{
			return 0;
		}

		s->shifter<<=6;
		s->shifter|=o;
		s->bit_count+=6;
	}

	return 0;
}

static boolean get_8_2(decoder *s,octet *cp)
{
	*cp=0;

	while (1)
	{
		if (s->repeat_count)
		{
			*cp=s->last_octet;

			s->repeat_count--;

			if (!s->repeat_count)
			{
				s->last_count=0;
				s->last_octet=0;
			}

			return 1;
		}
		else
		{
			octet c;

			if (!s->last_count)
			{
				if (get_8_1(s,&s->last_octet))
				{
					s->last_count=1;

					if (s->last_octet==0x90)
					{
						get_8_1(s,&c);

						if (c != 0)
						{
							printf("repeat not following data\n");

							return 0;
						}
					}
				}
				else
				{
					return 0;
				}
			}

			if (!get_8_1(s,&c))
			{
				s->last_count=0;
				*cp=s->last_octet;
				s->last_octet=0;

				return 1;
			}

			if (c==0x90)
			{
				octet d=0;

				if (!get_8_1(s,&d))
				{
					void **pv=0;
					*pv=0;
					return 0;
				}

				if (d)
				{
			/*		if (d>=0x90) 
					{
						void **pv=0;
						*pv=0;
					}
			*/
					s->repeat_count=d;
				}
				else
				{
					*cp=s->last_octet;
					s->last_octet=c;
					return 1;
				}
			}
			else
			{
				*cp=s->last_octet;
				s->last_octet=c;

				return 1;
			}
		}

	}

	return 0;
}

static void do_crc(decoder *s,unsigned char v)
{
int i=8;
	
	while (i--)
	{
		int carry=(s->crc & 0x8000);
		
		s->crc<<=1;
		
		if (v & 0x80) s->crc++;
		
		if (carry)
		{
			s->crc^=0x1021;
		}
		
		v<<=1;
	}
}

static boolean get_8(decoder *s,octet *cp)
{
	if (!get_8_2(s,cp)) return 0;

/*	printf("[%x]",*cp);*/

	do_crc(s,*cp);

	return 1;
}

static void check_crc(decoder *s,const char *part)
{
	unsigned short crc=s->crc;
	unsigned short old_crc=s->crc;
	octet c1=0;
	octet c2=0;

	get_8_2(s,&c1);
	get_8_2(s,&c2);

	do_crc(s,0);
	do_crc(s,0);

	crc=(((unsigned short)c1) << 8)|(c2);

	if (s->crc != crc)
	{
		printf("\nchecksum error in %s, %x != %x\n",part,(int)s->crc,(int)crc);
	}

	s->crc=0;
}

static void do_header(decoder *s)
{
	octet i;
	octet *op;
	long fCreator,fType;
	OSErr err;
	Str255 name;
	
	get_8(s,&name[0]);

	i=0;

	while (i < name[0])
	{
		i++;
		get_8(s,&name[i]); /* printf("%c",name[i]);*/
	}

/*	printf("\n");*/
	
	err=FSMakeFSSpec(0,0,name,&s->fsp);

	if ((err!=0)&&(err!=fnfErr)) 
	{
		fprintf(stderr,"failed FSMakeFSSpec %d\n",err);
		fflush(stderr);
		
		exit(1);
	}
	
	get_8(s,&i);

/*	printf("%d\n",i);*/

	i=4;
	
	op=(void *)&fType;

	while (i--)
	{
		get_8(s,op++);
	}

	i=4;
	
	op=(void *)&fCreator;

	while (i--)
	{
		get_8(s,op++);
	}

	err=FSpCreate(&s->fsp,fCreator,fType,smSystemScript);
	
	if (err) 
	{
		fprintf(stderr,"failed FSpCreate %d\n",err);
		fflush(stderr);
		
		exit(1);
	}

	/* FLAG */
	get_8(s,&i);
	get_8(s,&i);

	i=4;

	while (i--)
	{
		octet x=0;
		get_8(s,&x);
		s->dlen<<=8;
		s->dlen+=x;
	}

/*	printf("dlen=%ld\n",s->dlen);*/

	i=4;

	while (i--)
	{
		octet x=0;
		get_8(s,&x);
		s->rlen<<=8;
		s->rlen+=x;
	}

/*	printf("rlen=%ld\n",s->rlen);*/

/*	printf("\n");*/

	check_crc(s,"header");
}


#ifdef _WIN32
OSErr FSMakeFSSpec(short v,long d,Str255 n,FSSpec *f)
{
	int k=n[0];
	memcpy(f->name,n,k+1);
	f->vRefNum=v;
	f->parID=d;
	return 0;
}

OSErr FSpCreate(FSSpec *f,long c,long t,long s)
{
	char name[256];
	char info[256];
	BYTE data[60];
	HANDLE h;
	DWORD dw=60;
	DWORD err=0;

	memcpy(name,f->name+1,f->name[0]);
	name[f->name[0]]=0;

	h=CreateFile(name,GENERIC_WRITE,0,NULL,
			CREATE_NEW,0,INVALID_HANDLE_VALUE);

	if (h==INVALID_HANDLE_VALUE) return GetLastError();

	CloseHandle(h);

	strcpy(info,name);
	strcat(info,":AFP_AfpInfo");

	h=CreateFile(info,GENERIC_WRITE,0,NULL,
			CREATE_ALWAYS,0,INVALID_HANDLE_VALUE);

	if (h==INVALID_HANDLE_VALUE) 
	{
		dw=GetLastError();

		DeleteFile(name);

		return dw;
	}

	memset(data,0,sizeof(data));
	memcpy(data,"AFP",3);
	memcpy(data+0x10,&t,4);
	memcpy(data+0x14,&c,4);

	data[6]=1;
	data[7]=0;
	data[0xF]=0x80;
	data[0x18]=1;

	if (!WriteFile(h,data,sizeof(data),&dw,NULL))
	{
		err=GetLastError();
	}

	CloseHandle(h);

	if (err)
	{
		DeleteFile(name);
	}

	return err;
}

OSErr FSClose(HANDLE r)
{
	CloseHandle(r);

	return 0;
}
OSErr FSWrite(HANDLE r,long *lp,void *pv)
{
	DWORD dw;

	if (!WriteFile(r,pv,*lp,&dw,NULL))
	{
		return GetLastError();
	}

	*lp=dw;

	return 0;
}

OSErr FSpOpenDF(FSSpec *f,long l,HANDLE *r)
{
	char name[256];
	HANDLE h;

	memcpy(name,f->name+1,f->name[0]);
	name[f->name[0]]=0;

	h=CreateFile(name,GENERIC_WRITE,0,NULL,TRUNCATE_EXISTING,0,INVALID_HANDLE_VALUE);

	if (h==INVALID_HANDLE_VALUE) return GetLastError();

	*r=h;

	return 0;
}

OSErr FSpOpenRF(FSSpec *f,long l,HANDLE *r)
{
	char name[256];
	HANDLE h;

	memcpy(name,f->name+1,f->name[0]);
	name[f->name[0]]=0;
	strcat(name,":AFP_Resource");

	h=CreateFile(name,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,INVALID_HANDLE_VALUE);

	if (h==INVALID_HANDLE_VALUE) return GetLastError();

	*r=h;

	return 0;
}

OSErr SetEOF(HANDLE r,DWORD l)
{
	DWORD hi=0;
	DWORD dw=GetFileSize(r,&hi);

	if ((dw==~0)&&(hi==~0))
	{
		return GetLastError();
	}

	if (dw!=l) return 1;

	return 0;
}
#endif

static void write_fork(decoder *s,
#ifdef _WIN32
					   HANDLE ref,
#else
					   short ref,
#endif
					   long len)
{
	SetEOF(ref,0);

	while (len)
	{
		octet buf[1024];
		long l;

		l=0;

		while ((l < len)&&(l < sizeof(buf)))
		{
			get_8(s,&buf[l]);
			l++;
		}

		len-=l;

		if (l)
		{
			FSWrite(ref,&l,buf);
		}
	}
}

static void do_data(decoder *s)
{
	if (s->dlen)
	{
#ifdef _WIN32
		HANDLE ref;
#else
		short ref;
#endif
		OSErr err;
		
		err=FSpOpenDF(&s->fsp,fsWrPerm,&ref);
		
		if (err)
		{
			fprintf(stderr,"write data fork failed, %d\n",err);
			fflush(stderr);
			exit(1);
		}

		write_fork(s,ref,s->dlen);

		FSClose(ref);
	}	
	
	check_crc(s,"data");
}

static void do_resource(decoder *s)
{
	if (s->rlen)
	{
		OSErr err;
#ifdef _WIN32
		HANDLE ref;
#else
		short ref;
#endif
		
		err=FSpOpenRF(&s->fsp,fsWrPerm,&ref);
		
		if (err)
		{
			fprintf(stderr,"write resource fork failed, %d\n",err);
			fflush(stderr);
			exit(1);
		}

		write_fork(s,ref,s->rlen);

		FSClose(ref);
	}

	check_crc(s,"resource");
}

static int unbinhex(FILE *fp)
{
	decoder s;
	char buf[256];

	memset(&s,0,sizeof(s));

	s.fp=fp;

	while (myfgets(buf,sizeof(buf),fp))
	{
		size_t i=strlen(buf);
		if (i > 10)
		{
			if (!memcmp(buf,"(This file must",15))
			{
				i=0;

				while (i!=':')
				{
					i=fgetc(s.fp);

					if (i==EOF) return 1;
				}

#ifdef _DEBUG
				fprintf(stderr,"\n");
#endif

				do_header(&s);
				do_data(&s);
				do_resource(&s);

				{
					octet o;
					while (get_8(&s,&o))
					{
						printf("[%x]",o);
					}
				}

				return 0;
			}
		}
	}

	return 1;
}

static int unbinhex_name(const char *name)
{
	FILE *fp=

#ifdef _WIN32
	fopen(name,"rb");
#else
	fopen(name,"r");
#endif

	if (fp)
	{
		int j=unbinhex(fp);
		fclose(fp);
		return j;
	}

	return 1;
}

int main(int argc,char **argv)
{
	if (argc > 1)
	{
		int i=1;

		while (i < argc)
		{
			int k=unbinhex_name(argv[i]);
			
			if (k) return k;
			
			i++;
		}
	}
	else
	{
		return unbinhex(stdin);
	}

	return 0;
}

