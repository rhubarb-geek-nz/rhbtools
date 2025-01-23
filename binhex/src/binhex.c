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
#include <string.h>

#ifdef _WIN32
#include <windows.h>
typedef BYTE Str255[256];
typedef struct
{
	char path[1024];
	Str255 name;
} FSSpec;
typedef DWORD OSErr;
typedef struct FInfo 
{
    long fdType;               
    long fdCreator;            
    unsigned short fdFlags;
    struct { short h,v; } fdLocation;
    short fdFldr;
} FInfo;
#define fsRdPerm     GENERIC_READ
OSErr FSRead(HANDLE,long *,char *);
OSErr GetEOF(HANDLE,long *);
OSErr FSClose(HANDLE);
OSErr FSMakeFSSpec(short,long,BYTE *,FSSpec *);
OSErr FSpGetFInfo(FSSpec *,FInfo *);
OSErr FSpOpenDF(FSSpec *,DWORD,HANDLE *);
OSErr FSpOpenRF(FSSpec *,DWORD,HANDLE *);
#else
#	include <Files.h>
#endif

#ifdef _DEBUG
static int debugFlag;
#endif

static unsigned short crc;

static char char_map[]="!\042#$%&'()*+,-012345689@ABCDEFGHIJKLMNPQRSTUVXYZ[`abcdefhijklmpqr";

static void calc_crc(int v)
{
int i;

	v&=0xff;

	i=8;
	
	while (i--)
	{
		int carry=0;
		
		if ((crc & 0x8000)) carry=1;
		
		crc<<=1;
		
		if (v & 0x80) crc++;
		
		if (carry)
		{
			crc^=0x1021;
		}
		
		v<<=1;
	}
}

static struct
{
	int write_len;
	char write_buffer[128];
} write_stuff;

static struct
{
	unsigned char data_buf[256];
	int data_len;
} data_stuff;

static struct
{
	unsigned char last_char;
	unsigned char last_count;
} stream_stuff;

static void write_flush(void)
{
	if (write_stuff.write_len)
	{
		write_stuff.write_buffer[write_stuff.write_len]=0;
		write_stuff.write_len=0;
		printf("%s\n",write_stuff.write_buffer);
	}
}

static void write_char(char c)
{
	write_stuff.write_buffer[write_stuff.write_len]=c;
	write_stuff.write_len++;
	if (write_stuff.write_len >= 64)
	{
		write_flush();
	}
}

static void data_flush(void)
{
	long l;
	unsigned char *p=data_stuff.data_buf;
	int bits;
	int j;
	
#ifdef _DEBUG
	if (debugFlag && debugFlag)
	{
		static int z=4;
		if (z)
		{
			fprintf(stderr,"[%d=%02X,%02X,%02X,%02X]",
				(int)data_stuff.data_len,
				p[0],
				p[1],
				p[2],
				p[3]);
			fflush(stderr);
			z--;
		}
	}
#endif
		
	l=0;
	bits=0;
	
	while (data_stuff.data_len)
	{
		data_stuff.data_len--;
		l<<=8;
		l+=*p;
/*		printf("[%x]",*p);*/
		p++;
		bits+=8;
	}
	
	j=32-bits;
	if (j)
	{
		l<<=j;
	}
	
#ifdef _DEBUG
	if (debugFlag && debugFlag)
	{
		static int z=4;
		if (z)
		{
			fprintf(stderr,"[%08lx]",l);
			fflush(stderr);
			z--;
		}
	}
#endif
	
	while (bits > 0)
	{
		char c;
		c=(char)(0x3f & (l >> (32-6)));
		write_char(char_map[c]);
		bits-=6;
		l<<=6;
	}
}

static void write_raw(unsigned char byteCh)
{
#ifdef _DEBUG
	static int debugCount=16;
	
	if (debugCount && debugFlag)
	{
		fprintf(stderr,"{%d,%02X}",data_stuff.data_len,byteCh);
		
		debugCount--;
		
		if (debugCount)
		{
			fprintf(stderr," ");
		}
		else
		{
			fprintf(stderr,"\n",byteCh);
		}

		fflush(stderr);
	}
#endif

	data_stuff.data_buf[data_stuff.data_len++]=byteCh;

#ifdef _DEBUG
	if (debugCount && debugFlag)
	{
		fprintf(stderr,"<%d>",data_stuff.data_len);
	}
#endif
	
	if (data_stuff.data_len==3)
	{
		data_flush();
	}
}

static void write_last(void)
{
	if (stream_stuff.last_count)
	{
		if (stream_stuff.last_char==0x90)
		{
			write_raw(stream_stuff.last_char);
			write_raw(0);
		}
		else
		{
			write_raw(stream_stuff.last_char);
		}
		
		if (stream_stuff.last_count != 1)
		{
			write_raw(0x90);
			write_raw(stream_stuff.last_count);
		}
		
		stream_stuff.last_count=0;
	}
}

static void write_byte(unsigned char byte)
{
#ifdef _DEBUG
	static int debugCount=16;
	
	if (debugCount && debugFlag)
	{
		fprintf(stderr,"%02X",byte);
		
		debugCount--;
		
		if (debugCount)
		{
			fprintf(stderr," ");
		}
		else
		{
			fprintf(stderr,"\n",byte);
		}

		fflush(stderr);
	}
#endif

	calc_crc(byte);
	
	if (stream_stuff.last_count)
	{
		if (stream_stuff.last_char==byte)
		{
			if (stream_stuff.last_count < 0x8e)
			{
				stream_stuff.last_count++;
				
				return;
			}
		}
	}

	write_last();
	stream_stuff.last_count=1;
	stream_stuff.last_char=byte;
}

static void write_end(void)
{
	write_last();
	data_flush();
	write_stuff.write_buffer[write_stuff.write_len++]=':';
	write_flush();
}

static void write_buf(void *pv,int len)
{
	unsigned char *op=pv;
	
	while (len--)
	{
		write_byte(*op);
		
		op++;
	}
}

static void write_crc(void)
{
	unsigned char last_crc[2];
	
	write_last();
	calc_crc(0);
	calc_crc(0);
	last_crc[0]=(unsigned char)(crc>>8);
	last_crc[1]=(unsigned char)(crc);
	write_buf(last_crc,sizeof(last_crc));
	crc=0;
}

static void write_fork(
#ifdef _WIN32
		HANDLE f,
#else
		short f,
#endif
		long len)
{
	while (len)
	{
		char buf[256];
		long l=sizeof(buf);
		OSErr err;
		
		if (l > len)
		{
			l=len;
		}
		
		err=FSRead(f,&l,buf);
		
		if (err)
		{
			fprintf(stderr,"read error %d\n",err);
			return;
		}
		
		len-=l;
		write_buf(buf,l);
	}
}

static void write_long(long val)
{
	/* in big endian format */
	char buf[4];

	buf[0]=(char)(val >> 24);
	buf[1]=(char)(val >> 16);
	buf[2]=(char)(val >> 8);
	buf[3]=(char)(val);

	write_buf(buf,sizeof(buf));
}

int main(int argc,char **argv)
{
FSSpec fs;
Str255 str;
char *p;
FInfo fndrInfo;
OSErr err;
#ifdef _WIN32
HANDLE rf=INVALID_HANDLE_VALUE,df=INVALID_HANDLE_VALUE;
#else
	short rf=-1,df=-1;
#endif
long dlen=0,rlen=0;
int resOpen=0,dataOpen=0;

	if (argc !=2)
	{
		fprintf(stderr,"pass exactly one filename\n");
		
	/*	i=0;
		
		while (i < 64)
		{
			printf("%d,%c\n",i,char_map[i]);
			i++;
		}
		*/
		return 1;
	}
	
	p=argv[1];
	str[0]=(unsigned char)strlen(p);
	strcpy((char *)&str[1],p);
	
	err=FSMakeFSSpec(0,0,str,&fs);
	
	if (err)
	{
		fprintf(stderr,"FSMakeFSSpec(%s)==%d\n",p,err);
		return 1;
	}
			
	err=FSpGetFInfo(&fs,&fndrInfo);
	
	if (err)
	{
		fprintf(stderr,"FSpGetFInfo(%s)==%d\n",p,err);
		return 1;
	}

	err=FSpOpenDF(&fs,fsRdPerm,&df);
	
	if (!err)
	{
		dataOpen=1;

		GetEOF(df,&dlen);
	}
	
	err=FSpOpenRF(&fs,fsRdPerm,&rf);
	
	if (!err)
	{
		GetEOF(rf,&rlen);

		resOpen=1;
	}
	
	printf("(This file must be converted with BinHex 4.0)\n");
	
	write_char(':');

	/* header section */

	write_buf(fs.name,fs.name[0]+1);
	write_byte(0);
	write_buf(&fndrInfo.fdType,4);
	write_buf(&fndrInfo.fdCreator,4);
	write_buf(&fndrInfo.fdFlags,2);
	write_long(dlen);
	write_long(rlen);

	write_crc();

	/* data section */
	
	if (dataOpen)
	{
		write_fork(df,dlen);
		FSClose(df);
	}
	
	write_crc();

	/* resource section */

	if (resOpen)
	{
		write_fork(rf,rlen); 

		FSClose(rf);
	}

	write_crc();
 
	write_end();

	return 0;
}

#ifdef _WIN32
OSErr FSRead(HANDLE fd,long *plen,char *buf)
{
	DWORD dw=*plen;

	if (ReadFile(fd,buf,dw,&dw,NULL))
	{
		*plen=dw;

		return 0;
	}

	return GetLastError();
}
OSErr GetEOF(HANDLE fd,long *plen)
{
	DWORD hi=0;
	DWORD dw=GetFileSize(fd,&hi);

	if ((dw==~0)&&(hi==~0))
	{
		return GetLastError();
	}

	*plen=dw;

	return 0;
}

OSErr FSClose(HANDLE fd)
{
	CloseHandle(fd);
	return 0;
}

OSErr FSMakeFSSpec(short vRefNum,long parID,BYTE *p,FSSpec *fsp)
{
	long len=p[0];
	int k=0;

	memset(fsp,0,sizeof(*fsp));

	memcpy(fsp->path,p+1,len);
	fsp->path[len]=0;

	while (k < len)
	{
		char c=p[len-k];

		if ((c=='/')||(c=='\\'))
		{
			break;
		}

		k++;
	}

	if (k)
	{
		memcpy(fsp->name+1,p+1+len-k,k);
		fsp->name[0]=k;
	}

	return 0;
}

OSErr FSpGetFInfo(FSSpec *fsp,FInfo *info)
{
	char buf[1024];
	DWORD dw;
	DWORD err=ERROR_INVALID_DATA;

	memset(info,0,sizeof(*info));

	strcpy(buf,fsp->path);

	dw=GetFileAttributes(buf);

	if (dw==~0)
	{
		err=GetLastError();
	}
	else
	{
		HANDLE h;
		strcat(buf,":AFP_AfpInfo");

		memset(&info->fdType,'*',sizeof(info->fdType));
		memcpy(&info->fdCreator,"NTFS",sizeof(info->fdCreator));

		h=CreateFile(buf,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				0,
				INVALID_HANDLE_VALUE);

		if (h == INVALID_HANDLE_VALUE)
		{
			err=GetLastError();
		}
		else
		{
			if (ReadFile(h,buf,sizeof(buf),&dw,NULL))
			{
				if (dw>=60)
				{
					if (!memcmp(buf,"AFP",4))
					{
						memcpy(info,buf+16,sizeof(*info));

						err=0;
					}
				}
			}
			else
			{
				err=GetLastError();
			}

			CloseHandle(h);
		}
	}

	return err;
}

OSErr FSpOpenDF(FSSpec *fsp,DWORD f,HANDLE *ph)
{
	HANDLE h=CreateFile(fsp->path,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			INVALID_HANDLE_VALUE);

	if (h==INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}

	*ph=h;

	return 0;
}

OSErr FSpOpenRF(FSSpec *fsp,DWORD f,HANDLE *ph)
{
	HANDLE h;
	char path[1024];

	strcpy(path,fsp->path);
	strcat(path,":AFP_Resource");
		
	h=CreateFile(path,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			INVALID_HANDLE_VALUE);

	if (h==INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}

	*ph=h;

	return 0;
}
#endif
